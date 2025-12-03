/*
 * Cultivio AquaSense - Sensor Node Firmware
 * Water Level Measurement via Ultrasonic Sensor
 * BLE Provisioning + Zigbee Communication
 * 
 * Copyright (c) 2024 Cultivio
 * Target: ESP32-H2
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

#include "esp_zigbee_core.h"
#include "ha/esp_zigbee_ha_standard.h"

#include "ble_provision.h"
#include "cultivio_brand.h"

/* ============================================================================
 * CONFIGURATION
 * ============================================================================ */

static const char *TAG = "SENSOR_NODE";

// Hardware Pins (ESP32-H2 Mini compatible)
#define ULTRASONIC_TRIG_PIN     GPIO_NUM_2
#define ULTRASONIC_ECHO_PIN     GPIO_NUM_3
#define LED_STATUS_PIN          GPIO_NUM_8
#define LED_PROV_PIN            GPIO_NUM_9      // Provisioning mode LED
#define BUTTON_PIN              GPIO_NUM_10     // Button for provisioning mode

// Ultrasonic sensor settings
#define SOUND_SPEED_CM_US       0.0343f
#define ULTRASONIC_TIMEOUT_US   30000
#define NUM_SAMPLES             5
#define SAMPLE_DELAY_MS         50

// Zigbee configuration
#define SENSOR_ENDPOINT         1
#define CLUSTER_WATER_LEVEL     0xFC01
#define ATTR_WATER_LEVEL_PCT    0x0000
#define ATTR_WATER_LEVEL_CM     0x0001
#define ATTR_SENSOR_STATUS      0x0002

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

static device_config_t g_config;
static uint8_t  g_water_level_percent = 0;
static uint16_t g_water_level_cm = 0;
static uint8_t  g_sensor_status = 0;
static bool     g_zigbee_connected = false;
static bool     g_provisioning_mode = false;

// FIX: BUG #12 - Zigbee join retry limit
static uint8_t g_join_retry_count = 0;
#define MAX_JOIN_RETRIES 10

/* ============================================================================
 * LED FUNCTIONS
 * ============================================================================ */

static void led_init(void)
{
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED_STATUS_PIN) | (1ULL << LED_PROV_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_conf);
    gpio_set_level(LED_STATUS_PIN, 0);
    gpio_set_level(LED_PROV_PIN, 0);
}

static void button_init(void)
{
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_conf);
}

static void led_blink(int pin, int times, int delay_ms)
{
    for (int i = 0; i < times; i++) {
        gpio_set_level(pin, 1);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        gpio_set_level(pin, 0);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/* ============================================================================
 * ULTRASONIC SENSOR FUNCTIONS
 * ============================================================================ */

static void ultrasonic_init(void)
{
    gpio_config_t trig_conf = {
        .pin_bit_mask = (1ULL << ULTRASONIC_TRIG_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&trig_conf);

    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << ULTRASONIC_ECHO_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&echo_conf);

    gpio_set_level(ULTRASONIC_TRIG_PIN, 0);
    ESP_LOGI(TAG, "Ultrasonic sensor initialized");
}

static float measure_distance_cm(void)
{
    gpio_set_level(ULTRASONIC_TRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(ULTRASONIC_TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(ULTRASONIC_TRIG_PIN, 0);

    int64_t start_wait = esp_timer_get_time();
    while (gpio_get_level(ULTRASONIC_ECHO_PIN) == 0) {
        if ((esp_timer_get_time() - start_wait) > ULTRASONIC_TIMEOUT_US) {
            return -1.0f;
        }
        // FIX: BUG #2 - Prevent watchdog timeout and allow task switching
        esp_rom_delay_us(10);
        taskYIELD();
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(ULTRASONIC_ECHO_PIN) == 1) {
        if ((esp_timer_get_time() - echo_start) > ULTRASONIC_TIMEOUT_US) {
            return -1.0f;
        }
        // FIX: BUG #2 - Prevent watchdog timeout and allow task switching
        esp_rom_delay_us(10);
        taskYIELD();
    }
    int64_t echo_end = esp_timer_get_time();

    float duration_us = (float)(echo_end - echo_start);
    float distance_cm = (duration_us * SOUND_SPEED_CM_US) / 2.0f;

    return distance_cm;
}

static void measure_water_level(void)
{
    float total_distance = 0;
    int valid_samples = 0;
    
    uint16_t tank_height = g_config.tank_height_cm > 0 ? g_config.tank_height_cm : 200;
    uint8_t sensor_offset = g_config.sensor_offset_cm;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        float distance = measure_distance_cm();
        if (distance > 0 && distance < tank_height + 50) {
            total_distance += distance;
            valid_samples++;
        }
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_DELAY_MS));
    }

    if (valid_samples > 0) {
        // FIX: BUG #5 - Validate tank height before division
        if (tank_height == 0) {
            ESP_LOGE(TAG, "FATAL: Tank height is 0! Cannot calculate water level.");
            g_sensor_status = 1;
            g_water_level_percent = 0;
            g_water_level_cm = 0;
            return;
        }
        
        float avg_distance = total_distance / valid_samples;
        float water_depth = tank_height - avg_distance - sensor_offset;
        
        if (water_depth < 0) water_depth = 0;
        if (water_depth > tank_height) water_depth = tank_height;

        g_water_level_cm = (uint16_t)water_depth;
        g_water_level_percent = (uint8_t)((water_depth / tank_height) * 100.0f);
        g_sensor_status = 0;

        ESP_LOGI(TAG, "Water Level: %d%% (%d cm)", g_water_level_percent, g_water_level_cm);
    } else {
        g_sensor_status = 1;
        ESP_LOGW(TAG, "Sensor measurement failed");
    }
}

/* ============================================================================
 * ZIGBEE FUNCTIONS
 * ============================================================================ */

static esp_zb_cluster_list_t *create_water_sensor_clusters(void)
{
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();

    esp_zb_basic_cluster_cfg_t basic_cfg = {
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
        .power_source = 0x01,
    };
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&basic_cfg);
    esp_zb_cluster_list_add_basic_cluster(cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_identify_cluster_cfg_t identify_cfg = { .identify_time = 0 };
    esp_zb_attribute_list_t *identify_cluster = esp_zb_identify_cluster_create(&identify_cfg);
    esp_zb_cluster_list_add_identify_cluster(cluster_list, identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_attribute_list_t *water_cluster = esp_zb_zcl_attr_list_create(CLUSTER_WATER_LEVEL);
    
    esp_zb_custom_cluster_add_custom_attr(water_cluster, ATTR_WATER_LEVEL_PCT,
        ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        &g_water_level_percent);
    
    esp_zb_custom_cluster_add_custom_attr(water_cluster, ATTR_WATER_LEVEL_CM,
        ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        &g_water_level_cm);
    
    esp_zb_custom_cluster_add_custom_attr(water_cluster, ATTR_SENSOR_STATUS,
        ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
        &g_sensor_status);

    esp_zb_cluster_list_add_custom_cluster(cluster_list, water_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    return cluster_list;
}

static void send_water_level_report(void)
{
    if (!g_zigbee_connected) return;

    esp_zb_zcl_set_attribute_val(SENSOR_ENDPOINT, CLUSTER_WATER_LEVEL,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_PCT, &g_water_level_percent, false);
    
    esp_zb_zcl_set_attribute_val(SENSOR_ENDPOINT, CLUSTER_WATER_LEVEL,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_CM, &g_water_level_cm, false);

    esp_zb_zcl_report_attr_cmd_t report_cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u.addr_short = 0x0000,
            .dst_endpoint = SENSOR_ENDPOINT,
            .src_endpoint = SENSOR_ENDPOINT,
        },
        .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
        .clusterID = CLUSTER_WATER_LEVEL,
        .attributeID = ATTR_WATER_LEVEL_PCT,
    };
    
    esp_zb_zcl_report_attr_cmd_req(&report_cmd);
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
    return ESP_OK;
}

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask)
{
    ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;

    switch (sig_type) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            ESP_LOGI(TAG, "Zigbee stack initialized");
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
            break;

        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "Joining network...");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "Joined network! PAN: 0x%04x, CH: %d",
                         esp_zb_get_pan_id(), esp_zb_get_current_channel());
                g_zigbee_connected = true;
                g_join_retry_count = 0;  // Reset counter on success
                led_blink(LED_STATUS_PIN, 3, 100);
            } else {
                // FIX: BUG #12 - Limit join retries to prevent battery drain
                g_join_retry_count++;
                if (g_join_retry_count < MAX_JOIN_RETRIES) {
                    ESP_LOGW(TAG, "Steering failed, retry %d/%d", 
                             g_join_retry_count, MAX_JOIN_RETRIES);
                    esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                           ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
                } else {
                    ESP_LOGE(TAG, "Max join retries reached! Network unavailable.");
                    ESP_LOGE(TAG, "Please check: 1) Coordinator is powered on, 2) Device is in range");
                    // Blink error pattern
                    while (1) {
                        led_blink(LED_STATUS_PIN, 5, 200);
                        vTaskDelay(pdMS_TO_TICKS(5000));
                    }
                }
            }
            break;

        default:
            break;
    }
}

static void zigbee_task(void *pvParameters)
{
    // FIX: BUG #10 - Add this task to watchdog monitoring
    esp_task_wdt_add(NULL);
    
    esp_zb_cfg_t zb_nwk_cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,
        .install_code_policy = false,
        .nwk_cfg.zed_cfg = {
            .ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN,
            .keep_alive = 3000,
        },
    };
    esp_zb_init(&zb_nwk_cfg);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_cluster_list_t *cluster_list = create_water_sensor_clusters();
    
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = SENSOR_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);
    esp_zb_device_register(ep_list);

    esp_zb_core_action_handler_register(zb_action_handler);
    esp_zb_set_channel_mask(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

    ESP_LOGI(TAG, "Starting Zigbee stack...");
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/* ============================================================================
 * SENSOR TASK
 * ============================================================================ */

static uint32_t g_uptime_seconds = 0;

static void sensor_task(void *pvParameters)
{
    uint16_t interval = g_config.report_interval_sec > 0 ? g_config.report_interval_sec : 5;
    
    while (1) {
        // FIX: BUG #10 - Feed watchdog in sensor loop
        esp_task_wdt_reset();
        
        if (!g_provisioning_mode) {
            measure_water_level();
            send_water_level_report();
            
            // Update BLE status for mobile monitoring
            device_status_t status = {
                .node_type = NODE_TYPE_SENSOR,
                .zigbee_connected = g_zigbee_connected,
                .uptime_seconds = g_uptime_seconds,
                .water_level_percent = g_water_level_percent,
                .water_level_cm = g_water_level_cm,
                .sensor_status = g_sensor_status,
                .pump_active = false,
                .pump_runtime_sec = 0,
                .last_water_level = g_water_level_percent,
                .last_update_time = g_uptime_seconds,
                .manual_override = false,
                .manual_remaining_sec = 0,
                .rssi_dbm = -50,  // Sensor doesn't receive, so fixed value
                .signal_quality = g_zigbee_connected ? 80 : 0
            };
            ble_status_update(&status);
            
            if (g_sensor_status == 0) {
                led_blink(LED_STATUS_PIN, 1, 50);
            } else {
                led_blink(LED_STATUS_PIN, 3, 100);
            }
            
            g_uptime_seconds += interval;
        }
        
        vTaskDelay(pdMS_TO_TICKS(interval * 1000));
    }
}

/* ============================================================================
 * PROVISIONING TASK
 * ============================================================================ */

static void provisioning_complete_cb(const device_config_t *config)
{
    ESP_LOGI(TAG, "Provisioning complete! Restarting in 2 seconds...");
    led_blink(LED_PROV_PIN, 5, 100);
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
}

static void provisioning_led_task(void *pvParameters)
{
    while (g_provisioning_mode) {
        gpio_set_level(LED_PROV_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED_PROV_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    gpio_set_level(LED_PROV_PIN, 0);
    vTaskDelete(NULL);
}

static void start_provisioning_mode(void)
{
    g_provisioning_mode = true;
    ESP_LOGI(TAG, "=== PROVISIONING MODE ===");
    ESP_LOGI(TAG, "Open the provisioning app and connect via Bluetooth");
    
    ble_provision_set_complete_callback(provisioning_complete_cb);
    ble_provision_start();
    
    xTaskCreate(provisioning_led_task, "prov_led", 2048, NULL, 3, NULL);
}

static bool check_provisioning_button(void)
{
    // FIX: BUG #11 - Add debounce delay before checking button
    // Check if button is pressed during boot (held for 3 seconds)
    if (gpio_get_level(BUTTON_PIN) == 0) {
        // Debounce delay
        vTaskDelay(pdMS_TO_TICKS(50));
        
        // Re-check after debounce
        if (gpio_get_level(BUTTON_PIN) == 0) {
            ESP_LOGI(TAG, "Button pressed, checking for provisioning mode...");
            int count = 0;
            while (gpio_get_level(BUTTON_PIN) == 0 && count < 30) {
                vTaskDelay(pdMS_TO_TICKS(100));
                count++;
                if (count % 5 == 0) {
                    gpio_set_level(LED_PROV_PIN, !gpio_get_level(LED_PROV_PIN));
                }
            }
            gpio_set_level(LED_PROV_PIN, 0);
            
            if (count >= 30) {
                return true;  // Enter provisioning mode
            }
        }
    }
    return false;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

void app_main(void)
{
    // Print Cultivio brand banner
    PRINT_CULTIVIO_COMPACT();
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  %s - Sensor Node", CULTIVIO_PRODUCT_INFO);
    ESP_LOGI(TAG, "  %s", CULTIVIO_COPYRIGHT);
    ESP_LOGI(TAG, "========================================");

    // Initialize NVS (FIX: BUG #7 - Improved error handling)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS needs erasing (status: %s), erasing...", esp_err_to_name(ret));
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "FATAL: NVS erase failed: %s", esp_err_to_name(ret));
            ESP_LOGE(TAG, "Device cannot operate without NVS. Restarting in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000));
            esp_restart();
        }
        ESP_LOGI(TAG, "NVS erased successfully, reinitializing...");
        ret = nvs_flash_init();
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "FATAL: NVS init failed: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Flash may be corrupted. Attempting factory reset...");
        // Last resort: try one more erase
        nvs_flash_erase();
        ret = nvs_flash_init();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "FATAL: NVS recovery failed. Hardware issue suspected.");
            // Blink error pattern forever
            while (1) {
                led_blink(LED_STATUS_PIN, 10, 100);
                vTaskDelay(pdMS_TO_TICKS(2000));
            }
        }
    }
    
    ESP_LOGI(TAG, "NVS initialized successfully");

    // FIX: BUG #10 - Configure watchdog timer (30 seconds)
    ESP_LOGI(TAG, "Configuring watchdog timer...");
    ret = esp_task_wdt_init(30, true);  // 30 second timeout, panic on timeout
    if (ret == ESP_OK) {
        esp_task_wdt_add(NULL);  // Add current task to watchdog
        ESP_LOGI(TAG, "Watchdog timer configured: 30 seconds");
    } else {
        ESP_LOGW(TAG, "Failed to init watchdog: %s", esp_err_to_name(ret));
    }

    // Initialize hardware
    led_init();
    button_init();
    
    // Startup indication
    led_blink(LED_STATUS_PIN, 2, 200);
    ESP_LOGI(TAG, "Hardware initialized");

    // Initialize provisioning
    ble_provision_init(NODE_TYPE_SENSOR);
    ble_provision_get_config(&g_config);

    // Check if button is pressed for provisioning mode
    bool force_provision = check_provisioning_button();

    if (force_provision || !ble_provision_is_provisioned()) {
        // Enter provisioning mode
        start_provisioning_mode();
    } else {
        // Normal operation
        ESP_LOGI(TAG, "Device provisioned - starting normal operation");
        ESP_LOGI(TAG, "Tank Height: %d cm, Report Interval: %d sec",
                 g_config.tank_height_cm, g_config.report_interval_sec);
        
        ultrasonic_init();
        
        // Start Zigbee
        xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Start sensor task
        xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 4, NULL);
        
        // Start BLE status monitoring for mobile app
        vTaskDelay(pdMS_TO_TICKS(1000));
        ble_status_start();
        
        ESP_LOGI(TAG, "Sensor Node started successfully");
        ESP_LOGI(TAG, "Mobile app can connect via BLE to view status");
    }
}
