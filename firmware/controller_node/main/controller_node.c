/*
 * Cultivio AquaSense - Controller Node Firmware
 * Receives Water Level Data via Zigbee & Controls Pump
 * BLE Provisioning Support
 * 
 * Copyright (c) 2024 Cultivio
 * Target: ESP32-H2
 * Role: Zigbee Coordinator
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

static const char *TAG = "CONTROLLER";

// Hardware Pins (ESP32-H2 Mini compatible)
#define PUMP_RELAY_PIN          GPIO_NUM_2      // Relay to control pump
#define LED_STATUS_PIN          GPIO_NUM_8      // Status LED
#define LED_PUMP_PIN            GPIO_NUM_9      // Pump running indicator
#define BUTTON_PIN              GPIO_NUM_10     // Button for provisioning mode

// Timing
#define SENSOR_TIMEOUT_MS       30000   // Sensor offline after 30 seconds
#define STATUS_UPDATE_MS        1000    // Status check interval

// Zigbee configuration
#define CONTROLLER_ENDPOINT     1
#define CLUSTER_WATER_LEVEL     0xFC01
#define ATTR_WATER_LEVEL_PCT    0x0000
#define ATTR_WATER_LEVEL_CM     0x0001
#define ATTR_SENSOR_STATUS      0x0002
#define ATTR_PUMP_STATE         0x0003

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

static device_config_t g_config;

// Received sensor data
static uint8_t  g_water_level_percent = 0;
static uint16_t g_water_level_cm = 0;
static uint8_t  g_sensor_status = 0xFF;  // FIX: BUG #13 - Removed unused attribute, will be updated
static uint32_t g_last_sensor_update = 0;

// Signal strength tracking
static int8_t   g_last_rssi = -100;
static uint8_t  g_signal_quality = 0;

// Pump control
static bool     g_pump_running = false;
static uint32_t g_pump_start_time = 0;
static uint8_t  g_pump_state_attr = 0;

// Manual override
static bool     g_manual_override = false;
static uint32_t g_manual_override_end_time = 0;
static uint16_t g_manual_duration_min = 0;

// Zigbee
static bool     g_zigbee_started = false;
static bool     g_sensor_connected = false;
static bool     g_provisioning_mode = false;

// FIX: BUG #12 - Zigbee network formation retry limit
static uint8_t g_formation_retry_count = 0;
#define MAX_FORMATION_RETRIES 10

/* ============================================================================
 * LED FUNCTIONS
 * ============================================================================ */

static void led_init(void)
{
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED_STATUS_PIN) | (1ULL << LED_PUMP_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_conf);
    gpio_set_level(LED_STATUS_PIN, 0);
    gpio_set_level(LED_PUMP_PIN, 0);
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

static void led_set_pump(bool on)
{
    gpio_set_level(LED_PUMP_PIN, on ? 1 : 0);
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
 * PUMP CONTROL FUNCTIONS
 * ============================================================================ */

static void pump_init(void)
{
    gpio_config_t relay_conf = {
        .pin_bit_mask = (1ULL << PUMP_RELAY_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&relay_conf);
    gpio_set_level(PUMP_RELAY_PIN, 0);
    ESP_LOGI(TAG, "Pump relay initialized (OFF)");
}

static void pump_on(void)
{
    if (!g_pump_running) {
        gpio_set_level(PUMP_RELAY_PIN, 1);
        g_pump_running = true;
        g_pump_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
        g_pump_state_attr = 1;
        led_set_pump(true);
        ESP_LOGI(TAG, ">>> PUMP ON <<< Water level: %d%%", g_water_level_percent);
    }
}

static void pump_off(void)
{
    // Clear manual override when pump is turned off
    if (g_manual_override) {
        g_manual_override = false;
        g_manual_override_end_time = 0;
        g_manual_duration_min = 0;
        ESP_LOGI(TAG, "Manual override cleared");
    }
    
    if (g_pump_running) {
        gpio_set_level(PUMP_RELAY_PIN, 0);
        g_pump_running = false;
        g_pump_state_attr = 0;
        led_set_pump(false);
        
        uint32_t runtime = (xTaskGetTickCount() * portTICK_PERIOD_MS / 1000) - g_pump_start_time;
        ESP_LOGI(TAG, ">>> PUMP OFF <<< Runtime: %lu seconds", runtime);
    }
}

// Manual pump command handler - called from BLE
static void manual_pump_cmd_handler(const manual_pump_cmd_t *cmd)
{
    if (cmd->command == PUMP_CMD_START_TIMED && cmd->duration_minutes > 0) {
        // Start manual override
        // FIX: BUG #8 - Use esp_timer for 64-bit time (no overflow after 49 days)
        g_manual_override = true;
        g_manual_duration_min = cmd->duration_minutes;
        g_manual_override_end_time = (esp_timer_get_time() / 1000000) + (cmd->duration_minutes * 60);
        
        ESP_LOGW(TAG, ">>> MANUAL OVERRIDE: Pump ON for %d minutes <<<", cmd->duration_minutes);
        pump_on();
        
        // Triple blink to indicate manual mode
        for (int i = 0; i < 3; i++) {
            gpio_set_level(LED_STATUS_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(100));
            gpio_set_level(LED_STATUS_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    } else {
        // Stop manual override
        ESP_LOGW(TAG, ">>> MANUAL OVERRIDE: Pump STOP <<<");
        g_manual_override = false;
        g_manual_override_end_time = 0;
        g_manual_duration_min = 0;
        pump_off();
    }
}

static void pump_control_logic(void)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    // FIX: BUG #8 - Use esp_timer for manual override time comparisons
    uint32_t now_sec = esp_timer_get_time() / 1000000;
    bool sensor_online = (now - g_last_sensor_update) < SENSOR_TIMEOUT_MS;
    
    // Get thresholds from config
    uint8_t pump_on_threshold = g_config.pump_on_threshold > 0 ? g_config.pump_on_threshold : 20;
    uint8_t pump_off_threshold = g_config.pump_off_threshold > 0 ? g_config.pump_off_threshold : 80;
    
    // FIX: BUG #3 - Use uint32_t to prevent overflow when multiplying by 60
    uint32_t pump_timeout_sec = g_config.pump_timeout_minutes > 0 ? 
                                ((uint32_t)g_config.pump_timeout_minutes * 60) : 3600;
    
    // Safety limit: Max 2 hours (7200 seconds)
    if (pump_timeout_sec > 7200) {
        ESP_LOGW(TAG, "Pump timeout capped at 2 hours for safety (was %lu)", pump_timeout_sec);
        pump_timeout_sec = 7200;
    }
    
    // ========== MANUAL OVERRIDE MODE ==========
    if (g_manual_override) {
        // Check if manual override has expired
        if (now_sec >= g_manual_override_end_time) {
            ESP_LOGW(TAG, "Manual override expired after %d minutes", g_manual_duration_min);
            g_manual_override = false;
            g_manual_override_end_time = 0;
            g_manual_duration_min = 0;
            pump_off();
            return;
        }
        
        // Keep pump running in manual mode
        if (!g_pump_running) {
            pump_on();
        }
        
        // Log remaining time every 30 seconds
        static uint32_t last_log_time = 0;
        if (now_sec - last_log_time >= 30) {
            uint32_t remaining = g_manual_override_end_time - now_sec;
            ESP_LOGI(TAG, "MANUAL MODE: %lu min %lu sec remaining", 
                     remaining / 60, remaining % 60);
            last_log_time = now_sec;
        }
        return; // Skip automatic control in manual mode
    }
    
    // ========== AUTOMATIC MODE ==========
    if (!sensor_online) {
        if (g_sensor_connected) {
            ESP_LOGW(TAG, "Sensor offline!");
            g_sensor_connected = false;
        }
        // In automatic mode, turn off pump if sensor is offline
        // (unless manual override is active - handled above)
        if (g_pump_running) {
            ESP_LOGW(TAG, "Turning pump OFF - sensor timeout");
            pump_off();
        }
        return;
    }
    
    if (!g_sensor_connected) {
        ESP_LOGI(TAG, "Sensor online");
        g_sensor_connected = true;
    }

    // Check pump timeout
    if (g_pump_running) {
        uint32_t runtime = now_sec - g_pump_start_time;
        if (runtime >= pump_timeout_sec) {
            ESP_LOGW(TAG, "Pump timeout after %lu seconds", runtime);
            pump_off();
            return;
        }
    }

    // Control logic
    if (g_water_level_percent <= pump_on_threshold && !g_pump_running) {
        ESP_LOGI(TAG, "Water LOW (%d%% <= %d%%), pump ON", g_water_level_percent, pump_on_threshold);
        pump_on();
    }
    else if (g_water_level_percent >= pump_off_threshold && g_pump_running) {
        ESP_LOGI(TAG, "Water HIGH (%d%% >= %d%%), pump OFF", g_water_level_percent, pump_off_threshold);
        pump_off();
    }
}

/* ============================================================================
 * ZIGBEE FUNCTIONS
 * ============================================================================ */

static esp_zb_cluster_list_t *create_controller_clusters(void)
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
        ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
        &g_water_level_percent);
    
    esp_zb_custom_cluster_add_custom_attr(water_cluster, ATTR_WATER_LEVEL_CM,
        ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
        &g_water_level_cm);
    
    esp_zb_custom_cluster_add_custom_attr(water_cluster, ATTR_PUMP_STATE,
        ESP_ZB_ZCL_ATTR_TYPE_U8, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY,
        &g_pump_state_attr);

    esp_zb_cluster_list_add_custom_cluster(cluster_list, water_cluster, ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);

    return cluster_list;
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
    switch (callback_id) {
        case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID: {
            esp_zb_zcl_set_attr_value_message_t *msg = (esp_zb_zcl_set_attr_value_message_t *)message;
            if (msg->info.cluster == CLUSTER_WATER_LEVEL) {
                if (msg->attribute.id == ATTR_WATER_LEVEL_PCT) {
                    g_water_level_percent = *(uint8_t *)msg->attribute.data.value;
                    g_last_sensor_update = xTaskGetTickCount() * portTICK_PERIOD_MS;
                    ESP_LOGI(TAG, "Received water level: %d%%", g_water_level_percent);
                }
                else if (msg->attribute.id == ATTR_WATER_LEVEL_CM) {
                    g_water_level_cm = *(uint16_t *)msg->attribute.data.value;
                }
                // FIX: BUG #13 - Update sensor status from Zigbee attributes
                else if (msg->attribute.id == ATTR_SENSOR_STATUS) {
                    g_sensor_status = *(uint8_t *)msg->attribute.data.value;
                    ESP_LOGD(TAG, "Sensor status updated: %d", g_sensor_status);
                }
            }
            break;
        }
        
        case ESP_ZB_CORE_REPORT_ATTR_CB_ID: {
            esp_zb_zcl_report_attr_message_t *msg = (esp_zb_zcl_report_attr_message_t *)message;
            if (msg->cluster == CLUSTER_WATER_LEVEL) {
                if (msg->attribute.id == ATTR_WATER_LEVEL_PCT) {
                    g_water_level_percent = *(uint8_t *)msg->attribute.data.value;
                    g_last_sensor_update = xTaskGetTickCount() * portTICK_PERIOD_MS;
                    ESP_LOGI(TAG, "Report - Water: %d%%", g_water_level_percent);
                    led_blink(LED_STATUS_PIN, 1, 50);
                }
                // FIX: BUG #13 - Update sensor status from Zigbee reports
                else if (msg->attribute.id == ATTR_SENSOR_STATUS) {
                    g_sensor_status = *(uint8_t *)msg->attribute.data.value;
                    ESP_LOGD(TAG, "Sensor status: %d", g_sensor_status);
                }
            }
            break;
        }
        
        default:
            break;
    }
    
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
                ESP_LOGI(TAG, "Forming network...");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
            }
            break;

        case ESP_ZB_BDB_SIGNAL_FORMATION:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "Network formed! PAN: 0x%04x, CH: %d",
                         esp_zb_get_pan_id(), esp_zb_get_current_channel());
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
                g_zigbee_started = true;
                g_formation_retry_count = 0;  // Reset counter on success
                led_blink(LED_STATUS_PIN, 3, 100);
            } else {
                // FIX: BUG #12 - Limit formation retries
                g_formation_retry_count++;
                if (g_formation_retry_count < MAX_FORMATION_RETRIES) {
                    ESP_LOGW(TAG, "Formation failed, retry %d/%d", 
                             g_formation_retry_count, MAX_FORMATION_RETRIES);
                    esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                           ESP_ZB_BDB_MODE_NETWORK_FORMATION, 1000);
                } else {
                    ESP_LOGE(TAG, "Max formation retries reached! Cannot form Zigbee network.");
                    ESP_LOGE(TAG, "Possible causes: 1) Channel interference, 2) Hardware failure");
                    // Blink error pattern
                    while (1) {
                        led_blink(LED_STATUS_PIN, 10, 100);
                        vTaskDelay(pdMS_TO_TICKS(3000));
                    }
                }
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "Network open for joining");
            }
            break;

        case ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE: {
            esp_zb_zdo_signal_device_annce_params_t *dev_annce = 
                (esp_zb_zdo_signal_device_annce_params_t *)esp_zb_app_signal_get_params(p_sg_p);
            ESP_LOGI(TAG, "Device joined! Addr: 0x%04x", dev_annce->device_short_addr);
            led_blink(LED_STATUS_PIN, 5, 50);
            break;
        }

        default:
            break;
    }
}

static void zigbee_task(void *pvParameters)
{
    // FIX: BUG #10 - Add this task to watchdog monitoring
    esp_task_wdt_add(NULL);
    
    esp_zb_cfg_t zb_nwk_cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_COORDINATOR,
        .install_code_policy = false,
        .nwk_cfg.zczr_cfg = {
            .max_children = 10,
        },
    };
    esp_zb_init(&zb_nwk_cfg);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_cluster_list_t *cluster_list = create_controller_clusters();
    
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = CONTROLLER_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);
    esp_zb_device_register(ep_list);

    esp_zb_core_action_handler_register(zb_action_handler);
    esp_zb_set_channel_mask(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

    ESP_LOGI(TAG, "Starting Zigbee as Coordinator...");
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/* ============================================================================
 * CONTROL TASK
 * ============================================================================ */

static uint32_t g_uptime_seconds = 0;
static uint32_t g_pump_runtime_total = 0;

static void control_task(void *pvParameters)
{
    while (1) {
        // FIX: BUG #10 - Feed watchdog in control loop
        esp_task_wdt_reset();
        
        if (!g_provisioning_mode) {
            pump_control_logic();
            
            // Calculate manual override remaining time
            // FIX: BUG #8 - Use esp_timer for consistent time calculation
            uint32_t manual_remaining = 0;
            uint32_t current_time_sec = esp_timer_get_time() / 1000000;
            if (g_manual_override && g_manual_override_end_time > current_time_sec) {
                manual_remaining = g_manual_override_end_time - current_time_sec;
            }
            
            // Update BLE status for mobile monitoring
            device_status_t status = {
                .node_type = NODE_TYPE_CONTROLLER,
                .zigbee_connected = g_zigbee_started,
                .uptime_seconds = g_uptime_seconds,
                .water_level_percent = g_water_level_percent,
                .water_level_cm = 0,
                .sensor_status = g_sensor_connected ? 0 : 1,
                .pump_active = g_pump_running,
                .pump_runtime_sec = g_pump_runtime_total,
                .last_water_level = g_water_level_percent,
                .last_update_time = g_uptime_seconds,
                .manual_override = g_manual_override,
                .manual_remaining_sec = manual_remaining,
                .rssi_dbm = g_last_rssi,
                .signal_quality = g_signal_quality
            };
            ble_status_update(&status);
            
            // Track pump runtime
            if (g_pump_running) {
                g_pump_runtime_total++;
            }
            g_uptime_seconds++;
            
            if (g_zigbee_started) {
                if (g_sensor_connected) {
                    gpio_set_level(LED_STATUS_PIN, 1);
                } else {
                    static bool toggle = false;
                    toggle = !toggle;
                    gpio_set_level(LED_STATUS_PIN, toggle);
                }
            }
            
            static int log_counter = 0;
            if (++log_counter >= 10) {
                log_counter = 0;
                ESP_LOGI(TAG, "Water=%d%%, Pump=%s, Sensor=%s",
                         g_water_level_percent,
                         g_pump_running ? "ON" : "OFF",
                         g_sensor_connected ? "Online" : "Offline");
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(STATUS_UPDATE_MS));
    }
}

/* ============================================================================
 * PROVISIONING
 * ============================================================================ */

static void provisioning_complete_cb(const device_config_t *config)
{
    ESP_LOGI(TAG, "Provisioning complete! Restarting...");
    led_blink(LED_STATUS_PIN, 5, 100);
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
}

static void provisioning_led_task(void *pvParameters)
{
    while (g_provisioning_mode) {
        gpio_set_level(LED_STATUS_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED_STATUS_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    gpio_set_level(LED_STATUS_PIN, 0);
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
    if (gpio_get_level(BUTTON_PIN) == 0) {
        // Debounce delay
        vTaskDelay(pdMS_TO_TICKS(50));
        
        // Re-check after debounce
        if (gpio_get_level(BUTTON_PIN) == 0) {
            ESP_LOGI(TAG, "Button pressed...");
            int count = 0;
            while (gpio_get_level(BUTTON_PIN) == 0 && count < 30) {
                vTaskDelay(pdMS_TO_TICKS(100));
                count++;
                if (count % 5 == 0) {
                    gpio_set_level(LED_STATUS_PIN, !gpio_get_level(LED_STATUS_PIN));
                }
            }
            gpio_set_level(LED_STATUS_PIN, 0);
            
            if (count >= 30) {
                return true;
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
    ESP_LOGI(TAG, "  %s - Controller Node", CULTIVIO_PRODUCT_INFO);
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
    pump_init();
    
    led_blink(LED_STATUS_PIN, 2, 200);
    ESP_LOGI(TAG, "Hardware initialized");

    // Initialize provisioning
    ble_provision_init(NODE_TYPE_CONTROLLER);
    ble_provision_get_config(&g_config);

    bool force_provision = check_provisioning_button();

    if (force_provision || !ble_provision_is_provisioned()) {
        start_provisioning_mode();
    } else {
        ESP_LOGI(TAG, "Device provisioned - starting operation");
        ESP_LOGI(TAG, "Pump ON: %d%%, OFF: %d%%, Timeout: %d min",
                 g_config.pump_on_threshold, g_config.pump_off_threshold,
                 g_config.pump_timeout_minutes);
        
        xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
        vTaskDelay(pdMS_TO_TICKS(2000));
        xTaskCreate(control_task, "control_task", 4096, NULL, 4, NULL);
        
        // Register manual pump command callback
        ble_register_pump_cmd_callback(manual_pump_cmd_handler);
        
        // Start BLE status monitoring for mobile app
        vTaskDelay(pdMS_TO_TICKS(1000));
        ble_status_start();
        
        ESP_LOGI(TAG, "Controller Node started");
        ESP_LOGI(TAG, "Mobile app can connect via BLE to view status");
        ESP_LOGI(TAG, "Manual pump control available via mobile app");
    }
}
