/*
 * Cultivio AquaSense - Unified Firmware
 * Single firmware for all device roles: Sensor, Controller, Router
 * Role is selected during BLE provisioning
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

static const char *TAG = "UNIFIED";

// Hardware Pins (ESP32-H2 Mini compatible)
// These pins are shared across all roles
#define GPIO_PIN_2              GPIO_NUM_2      // Trig (Sensor) / Relay (Controller)
#define GPIO_PIN_3              GPIO_NUM_3      // Echo (Sensor) / Unused (Controller)
#define LED_STATUS_PIN          GPIO_NUM_8      // Status LED (all roles)
#define LED_ACTIVITY_PIN        GPIO_NUM_9      // Activity LED / Pump LED
#define BUTTON_PIN              GPIO_NUM_10     // Provisioning button

// Ultrasonic sensor settings (Sensor role)
#define SOUND_SPEED_CM_US       0.0343f
#define ULTRASONIC_TIMEOUT_US   30000
#define NUM_SAMPLES             5
#define SAMPLE_DELAY_MS         50

// Timing (Controller role)
#define SENSOR_TIMEOUT_MS       30000
#define STATUS_UPDATE_MS        1000

// Zigbee configuration
#define DEVICE_ENDPOINT         1
#define CLUSTER_WATER_LEVEL     0xFC01
#define ATTR_WATER_LEVEL_PCT    0x0000
#define ATTR_WATER_LEVEL_CM     0x0001
#define ATTR_SENSOR_STATUS      0x0002
#define ATTR_PUMP_STATE         0x0003

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

static device_config_t g_config;
static bool g_provisioning_mode = false;
static bool g_zigbee_connected = false;
static uint32_t g_uptime_seconds = 0;

// Sensor-specific globals
static uint8_t  g_water_level_percent = 0;
static uint16_t g_water_level_cm = 0;
static uint8_t  g_sensor_status = 0;

// Controller-specific globals
static uint32_t g_last_sensor_update = 0;
static bool     g_pump_running = false;
static uint32_t g_pump_start_time = 0;
static uint8_t  g_pump_state_attr = 0;
static bool     g_sensor_connected = false;
static bool     g_manual_override = false;
static uint32_t g_manual_override_end_time = 0;
static uint16_t g_manual_duration_min = 0;
static uint32_t g_pump_runtime_total = 0;
static int8_t   g_last_rssi = -100;
static uint8_t  g_signal_quality = 0;

/* ============================================================================
 * COMMON LED FUNCTIONS
 * ============================================================================ */

static void led_init(void)
{
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED_STATUS_PIN) | (1ULL << LED_ACTIVITY_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_conf);
    gpio_set_level(LED_STATUS_PIN, 0);
    gpio_set_level(LED_ACTIVITY_PIN, 0);
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
 * SENSOR ROLE - ULTRASONIC FUNCTIONS
 * ============================================================================ */

static void ultrasonic_init(void)
{
    gpio_config_t trig_conf = {
        .pin_bit_mask = (1ULL << GPIO_PIN_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&trig_conf);

    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << GPIO_PIN_3),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&echo_conf);

    gpio_set_level(GPIO_PIN_2, 0);
    ESP_LOGI(TAG, "Ultrasonic sensor initialized");
}

static float measure_distance_cm(void)
{
    gpio_set_level(GPIO_PIN_2, 0);
    esp_rom_delay_us(2);
    gpio_set_level(GPIO_PIN_2, 1);
    esp_rom_delay_us(10);
    gpio_set_level(GPIO_PIN_2, 0);

    int64_t start_wait = esp_timer_get_time();
    while (gpio_get_level(GPIO_PIN_3) == 0) {
        if ((esp_timer_get_time() - start_wait) > ULTRASONIC_TIMEOUT_US) {
            return -1.0f;
        }
        // FIX: BUG #2 - Prevent watchdog timeout and allow task switching
        esp_rom_delay_us(10);
        taskYIELD();
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(GPIO_PIN_3) == 1) {
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
 * CONTROLLER ROLE - PUMP FUNCTIONS
 * ============================================================================ */

static void pump_init(void)
{
    gpio_config_t relay_conf = {
        .pin_bit_mask = (1ULL << GPIO_PIN_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&relay_conf);
    gpio_set_level(GPIO_PIN_2, 0);
    ESP_LOGI(TAG, "Pump relay initialized (OFF)");
}

static void pump_on(void)
{
    if (!g_pump_running) {
        gpio_set_level(GPIO_PIN_2, 1);
        g_pump_running = true;
        g_pump_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
        g_pump_state_attr = 1;
        gpio_set_level(LED_ACTIVITY_PIN, 1);
        ESP_LOGI(TAG, ">>> PUMP ON <<< Water level: %d%%", g_water_level_percent);
    }
}

static void pump_off(void)
{
    if (g_manual_override) {
        g_manual_override = false;
        g_manual_override_end_time = 0;
        g_manual_duration_min = 0;
        ESP_LOGI(TAG, "Manual override cleared");
    }
    
    if (g_pump_running) {
        gpio_set_level(GPIO_PIN_2, 0);
        g_pump_running = false;
        g_pump_state_attr = 0;
        gpio_set_level(LED_ACTIVITY_PIN, 0);
        
        uint32_t runtime = (xTaskGetTickCount() * portTICK_PERIOD_MS / 1000) - g_pump_start_time;
        ESP_LOGI(TAG, ">>> PUMP OFF <<< Runtime: %lu seconds", runtime);
    }
}

static void manual_pump_cmd_handler(const manual_pump_cmd_t *cmd)
{
    if (cmd->command == PUMP_CMD_START_TIMED && cmd->duration_minutes > 0) {
        g_manual_override = true;
        g_manual_duration_min = cmd->duration_minutes;
        g_manual_override_end_time = (xTaskGetTickCount() * portTICK_PERIOD_MS / 1000) 
                                     + (cmd->duration_minutes * 60);
        
        ESP_LOGW(TAG, ">>> MANUAL OVERRIDE: Pump ON for %d minutes <<<", cmd->duration_minutes);
        pump_on();
        led_blink(LED_STATUS_PIN, 3, 100);
    } else {
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
    uint32_t now_sec = now / 1000;
    bool sensor_online = (now - g_last_sensor_update) < SENSOR_TIMEOUT_MS;
    
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
    
    // Manual override mode
    if (g_manual_override) {
        if (now_sec >= g_manual_override_end_time) {
            ESP_LOGW(TAG, "Manual override expired after %d minutes", g_manual_duration_min);
            g_manual_override = false;
            g_manual_override_end_time = 0;
            g_manual_duration_min = 0;
            pump_off();
            return;
        }
        
        if (!g_pump_running) {
            pump_on();
        }
        return;
    }
    
    // Automatic mode
    if (!sensor_online) {
        if (g_sensor_connected) {
            ESP_LOGW(TAG, "Sensor offline!");
            g_sensor_connected = false;
        }
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

    if (g_pump_running) {
        uint32_t runtime = now_sec - g_pump_start_time;
        if (runtime >= pump_timeout_sec) {
            ESP_LOGW(TAG, "Pump timeout after %lu seconds", runtime);
            pump_off();
            return;
        }
    }

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
 * ZIGBEE - SENSOR ROLE
 * ============================================================================ */

static esp_zb_cluster_list_t *create_sensor_clusters(void)
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

    esp_zb_zcl_set_attribute_val(DEVICE_ENDPOINT, CLUSTER_WATER_LEVEL,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_PCT, &g_water_level_percent, false);
    
    esp_zb_zcl_set_attribute_val(DEVICE_ENDPOINT, CLUSTER_WATER_LEVEL,
        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_CM, &g_water_level_cm, false);

    esp_zb_zcl_report_attr_cmd_t report_cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u.addr_short = 0x0000,
            .dst_endpoint = DEVICE_ENDPOINT,
            .src_endpoint = DEVICE_ENDPOINT,
        },
        .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
        .clusterID = CLUSTER_WATER_LEVEL,
        .attributeID = ATTR_WATER_LEVEL_PCT,
    };
    
    esp_zb_zcl_report_attr_cmd_req(&report_cmd);
}

/* ============================================================================
 * ZIGBEE - CONTROLLER ROLE
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

/* ============================================================================
 * ZIGBEE - ROUTER ROLE
 * ============================================================================ */

static esp_zb_cluster_list_t *create_router_clusters(void)
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

    return cluster_list;
}

/* ============================================================================
 * ZIGBEE CALLBACKS
 * ============================================================================ */

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
    // Only controller needs to handle incoming data
    if (g_config.node_type != NODE_TYPE_CONTROLLER) {
        return ESP_OK;
    }
    
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
                if (g_config.node_type == NODE_TYPE_CONTROLLER) {
                    ESP_LOGI(TAG, "Forming network (Coordinator)...");
                    esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
                } else {
                    ESP_LOGI(TAG, "Joining network...");
                    esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
                }
            }
            break;

        case ESP_ZB_BDB_SIGNAL_FORMATION:
            if (err_status == ESP_OK) {
                ESP_LOGI(TAG, "Network formed! PAN: 0x%04x, CH: %d",
                         esp_zb_get_pan_id(), esp_zb_get_current_channel());
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
                g_zigbee_connected = true;
                led_blink(LED_STATUS_PIN, 3, 100);
            } else {
                ESP_LOGW(TAG, "Formation failed, retrying...");
                esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                       ESP_ZB_BDB_MODE_NETWORK_FORMATION, 1000);
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err_status == ESP_OK) {
                if (g_config.node_type == NODE_TYPE_CONTROLLER) {
                    ESP_LOGI(TAG, "Network open for joining");
                } else {
                    ESP_LOGI(TAG, "Joined network! PAN: 0x%04x, CH: %d",
                             esp_zb_get_pan_id(), esp_zb_get_current_channel());
                    g_zigbee_connected = true;
                    led_blink(LED_STATUS_PIN, 3, 100);
                }
            } else {
                ESP_LOGW(TAG, "Steering failed, retrying...");
                esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                       ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
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

/* ============================================================================
 * ZIGBEE TASK - ROLE BASED
 * ============================================================================ */

static void zigbee_task(void *pvParameters)
{
    esp_zb_cfg_t zb_nwk_cfg;
    memset(&zb_nwk_cfg, 0, sizeof(zb_nwk_cfg));
    
    // Configure based on role
    switch (g_config.node_type) {
        case NODE_TYPE_SENSOR:
            ESP_LOGI(TAG, "Configuring Zigbee as End Device (Sensor)");
            zb_nwk_cfg.esp_zb_role = ESP_ZB_DEVICE_TYPE_ED;
            zb_nwk_cfg.install_code_policy = false;
            zb_nwk_cfg.nwk_cfg.zed_cfg.ed_timeout = ESP_ZB_ED_AGING_TIMEOUT_64MIN;
            zb_nwk_cfg.nwk_cfg.zed_cfg.keep_alive = 3000;
            break;
            
        case NODE_TYPE_CONTROLLER:
            ESP_LOGI(TAG, "Configuring Zigbee as Coordinator (Controller)");
            zb_nwk_cfg.esp_zb_role = ESP_ZB_DEVICE_TYPE_COORDINATOR;
            zb_nwk_cfg.install_code_policy = false;
            zb_nwk_cfg.nwk_cfg.zczr_cfg.max_children = 10;
            break;
            
        case NODE_TYPE_ROUTER:
            ESP_LOGI(TAG, "Configuring Zigbee as Router");
            zb_nwk_cfg.esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER;
            zb_nwk_cfg.install_code_policy = false;
            zb_nwk_cfg.nwk_cfg.zczr_cfg.max_children = 10;
            break;
            
        default:
            ESP_LOGE(TAG, "Unknown node type!");
            vTaskDelete(NULL);
            return;
    }
    
    esp_zb_init(&zb_nwk_cfg);

    // Create endpoint with appropriate clusters
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_cluster_list_t *cluster_list;
    
    switch (g_config.node_type) {
        case NODE_TYPE_SENSOR:
            cluster_list = create_sensor_clusters();
            break;
        case NODE_TYPE_CONTROLLER:
            cluster_list = create_controller_clusters();
            break;
        case NODE_TYPE_ROUTER:
        default:
            cluster_list = create_router_clusters();
            break;
    }
    
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = DEVICE_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);
    esp_zb_device_register(ep_list);

    esp_zb_core_action_handler_register(zb_action_handler);
    esp_err_t ret_channel = esp_zb_set_channel_mask(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
    if (ret_channel != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set channel mask: %s", esp_err_to_name(ret_channel));
    }

    ESP_LOGI(TAG, "Starting Zigbee stack...");
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/* ============================================================================
 * ROLE-SPECIFIC TASKS
 * ============================================================================ */

static void sensor_task(void *pvParameters)
{
    uint16_t interval = g_config.report_interval_sec > 0 ? g_config.report_interval_sec : 5;
    
    while (1) {
        if (!g_provisioning_mode) {
            measure_water_level();
            send_water_level_report();
            
            esp_zb_lock_acquire(portMAX_DELAY);
            esp_zb_zcl_set_attribute_val(DEVICE_ENDPOINT, CLUSTER_WATER_LEVEL,
                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_PCT, 
                &g_water_level_percent, false);
            esp_zb_zcl_set_attribute_val(DEVICE_ENDPOINT, CLUSTER_WATER_LEVEL,
                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_WATER_LEVEL_CM, 
                &g_water_level_cm, false);
            esp_zb_zcl_set_attribute_val(DEVICE_ENDPOINT, CLUSTER_WATER_LEVEL,
                ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_SENSOR_STATUS, 
                &g_sensor_status, false);
            esp_zb_lock_release();

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
                .rssi_dbm = -50,
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

static void controller_task(void *pvParameters)
{
    while (1) {
        if (!g_provisioning_mode) {
            pump_control_logic();
            
            uint32_t manual_remaining = 0;
            if (g_manual_override && g_manual_override_end_time > g_uptime_seconds) {
                manual_remaining = g_manual_override_end_time - (xTaskGetTickCount() * portTICK_PERIOD_MS / 1000);
            }
            
            device_status_t status = {
                .node_type = NODE_TYPE_CONTROLLER,
                .zigbee_connected = g_zigbee_connected,
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
            
            if (g_pump_running) {
                g_pump_runtime_total++;
            }
            g_uptime_seconds++;
            
            if (g_zigbee_connected) {
                if (g_sensor_connected) {
                    gpio_set_level(LED_STATUS_PIN, 1);
                } else {
                    static bool toggle = false;
                    toggle = !toggle;
                    gpio_set_level(LED_STATUS_PIN, toggle);
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(STATUS_UPDATE_MS));
    }
}

static void router_task(void *pvParameters)
{
    while (1) {
        if (!g_provisioning_mode) {
            device_status_t status = {
                .node_type = NODE_TYPE_ROUTER,
                .zigbee_connected = g_zigbee_connected,
                .uptime_seconds = g_uptime_seconds,
                .water_level_percent = 0,
                .water_level_cm = 0,
                .sensor_status = 0,
                .pump_active = false,
                .pump_runtime_sec = 0,
                .last_water_level = 0,
                .last_update_time = 0,
                .manual_override = false,
                .manual_remaining_sec = 0,
                .rssi_dbm = -50,
                .signal_quality = g_zigbee_connected ? 90 : 0
            };
            ble_status_update(&status);
            
            g_uptime_seconds++;
            
            // Router just blinks slowly when connected
            if (g_zigbee_connected) {
                static int counter = 0;
                if (++counter >= 5) {
                    counter = 0;
                    led_blink(LED_STATUS_PIN, 1, 50);
                }
            } else {
                static bool toggle = false;
                toggle = !toggle;
                gpio_set_level(LED_STATUS_PIN, toggle);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ============================================================================
 * PROVISIONING
 * ============================================================================ */

static void provisioning_complete_cb(const device_config_t *config)
{
    ESP_LOGI(TAG, "Provisioning complete!");
    ESP_LOGI(TAG, "Device role: %s", 
             config->node_type == NODE_TYPE_SENSOR ? "SENSOR" :
             config->node_type == NODE_TYPE_CONTROLLER ? "CONTROLLER" : "ROUTER");
    led_blink(LED_ACTIVITY_PIN, 5, 100);
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
}

static void provisioning_led_task(void *pvParameters)
{
    while (g_provisioning_mode) {
        gpio_set_level(LED_ACTIVITY_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED_ACTIVITY_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    gpio_set_level(LED_ACTIVITY_PIN, 0);
    vTaskDelete(NULL);
}

static void start_provisioning_mode(void)
{
    g_provisioning_mode = true;
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║          PROVISIONING MODE ACTIVE              ║");
    ESP_LOGI(TAG, "╠════════════════════════════════════════════════╣");
    ESP_LOGI(TAG, "║  1. Open the provisioning app                  ║");
    ESP_LOGI(TAG, "║  2. Connect to 'Cultivio-XXXX' via Bluetooth   ║");
    ESP_LOGI(TAG, "║  3. Select device role: Sensor/Controller/Router║");
    ESP_LOGI(TAG, "║  4. Configure settings and save                ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");
    
    ble_provision_set_complete_callback(provisioning_complete_cb);
    ble_provision_start();
    
    xTaskCreate(provisioning_led_task, "prov_led", 2048, NULL, 3, NULL);
}

static bool check_provisioning_button(void)
{
    if (gpio_get_level(BUTTON_PIN) == 0) {
        ESP_LOGI(TAG, "Button pressed, hold for 3 seconds to enter provisioning...");
        int count = 0;
        while (gpio_get_level(BUTTON_PIN) == 0 && count < 30) {
            vTaskDelay(pdMS_TO_TICKS(100));
            count++;
            if (count % 5 == 0) {
                gpio_set_level(LED_ACTIVITY_PIN, !gpio_get_level(LED_ACTIVITY_PIN));
            }
        }
        gpio_set_level(LED_ACTIVITY_PIN, 0);
        
        if (count >= 30) {
            return true;
        }
    }
    return false;
}

static const char* get_role_name(prov_node_type_t type)
{
    switch (type) {
        case NODE_TYPE_SENSOR:     return "SENSOR";
        case NODE_TYPE_CONTROLLER: return "CONTROLLER";
        case NODE_TYPE_ROUTER:     return "ROUTER";
        default:                   return "UNKNOWN";
    }
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

void app_main(void)
{
    // Print Cultivio brand banner
    PRINT_CULTIVIO_COMPACT();
    
    ESP_LOGI(TAG, "╔════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║     CULTIVIO AquaSense - UNIFIED FIRMWARE      ║");
    ESP_LOGI(TAG, "║           Single Firmware, All Roles           ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════════════╝");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // FIX: BUG #10 - Configure watchdog timer (30 seconds)
    ESP_LOGI(TAG, "Configuring watchdog timer...");
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 30000,
        .trigger_panic = true,
    };
    ret = esp_task_wdt_init(&wdt_config);
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

    // Initialize provisioning (loads config from NVS)
    ble_provision_init(NODE_TYPE_SENSOR);  // Default type, will be overwritten if provisioned
    ble_provision_get_config(&g_config);

    // Check if button is pressed for provisioning mode
    bool force_provision = check_provisioning_button();

    if (force_provision || !ble_provision_is_provisioned()) {
        // Enter provisioning mode
        start_provisioning_mode();
    } else {
        // Normal operation based on configured role
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "╔════════════════════════════════════════════════╗");
        ESP_LOGI(TAG, "║  Device provisioned - Starting as %-10s  ║", get_role_name(g_config.node_type));
        ESP_LOGI(TAG, "╚════════════════════════════════════════════════╝");
        ESP_LOGI(TAG, "");
        
        // Role-specific initialization and tasks
        switch (g_config.node_type) {
            case NODE_TYPE_SENSOR:
                ESP_LOGI(TAG, "Tank Height: %d cm, Report Interval: %d sec",
                         g_config.tank_height_cm, g_config.report_interval_sec);
                ultrasonic_init();
                xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
                vTaskDelay(pdMS_TO_TICKS(2000));
                xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 4, NULL);
                break;
                
            case NODE_TYPE_CONTROLLER:
                ESP_LOGI(TAG, "Pump ON: %d%%, OFF: %d%%, Timeout: %d min",
                         g_config.pump_on_threshold, g_config.pump_off_threshold,
                         g_config.pump_timeout_minutes);
                pump_init();
                xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
                vTaskDelay(pdMS_TO_TICKS(2000));
                xTaskCreate(controller_task, "control_task", 4096, NULL, 4, NULL);
                ble_register_pump_cmd_callback(manual_pump_cmd_handler);
                break;
                
            case NODE_TYPE_ROUTER:
                ESP_LOGI(TAG, "Router mode - extending Zigbee network range");
                xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
                vTaskDelay(pdMS_TO_TICKS(2000));
                xTaskCreate(router_task, "router_task", 4096, NULL, 4, NULL);
                break;
                
            default:
                ESP_LOGE(TAG, "Unknown node type! Entering provisioning mode...");
                start_provisioning_mode();
                return;
        }
        
        // Start BLE status monitoring for mobile app
        vTaskDelay(pdMS_TO_TICKS(1000));
        ble_status_start();
        
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "Device started successfully!");
        ESP_LOGI(TAG, "Mobile app can connect via BLE to view status");
        ESP_LOGI(TAG, "Hold button for 3 seconds to re-provision");
    }
}

