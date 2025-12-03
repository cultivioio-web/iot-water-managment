/*
 * Cultivio AquaSense - Router Node Firmware
 * Zigbee Range Extender / Repeater
 * 
 * Copyright (c) 2024 Cultivio
 * Target: ESP32-H2
 * Role: Zigbee Router (Mesh Repeater)
 * 
 * This node simply relays Zigbee messages between
 * the Sensor Node and Controller Node to extend range.
 * No sensors or actuators needed - just power!
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_zigbee_core.h"
#include "ha/esp_zigbee_ha_standard.h"

#include "cultivio_brand.h"

/* ============================================================================
 * CONFIGURATION
 * ============================================================================ */

static const char *TAG = "ROUTER_NODE";

// Hardware Pins (ESP32-H2 Mini)
#define LED_STATUS_PIN          GPIO_NUM_8      // Status LED
#define LED_ACTIVITY_PIN        GPIO_NUM_9      // Network activity LED

// Zigbee configuration
#define ROUTER_ENDPOINT         1

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

static bool g_zigbee_connected = false;
static uint32_t g_packets_relayed = 0;
static uint32_t g_uptime_seconds = 0;

/* ============================================================================
 * LED FUNCTIONS
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

static void led_blink(gpio_num_t pin, int count, int delay_ms)
{
    for (int i = 0; i < count; i++) {
        gpio_set_level(pin, 1);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        gpio_set_level(pin, 0);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

static void led_activity_pulse(void)
{
    // Quick pulse on activity LED to show packet relay
    gpio_set_level(LED_ACTIVITY_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(LED_ACTIVITY_PIN, 0);
}

/* ============================================================================
 * ZIGBEE FUNCTIONS
 * ============================================================================ */

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
                ESP_LOGI(TAG, "Searching for network to join...");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
                led_blink(LED_STATUS_PIN, 3, 200);
            }
            break;

        case ESP_ZB_BDB_SIGNAL_STEERING:
            if (err_status == ESP_OK) {
                esp_zb_ieee_addr_t extended_pan_id;
                esp_zb_get_extended_pan_id(extended_pan_id);
                ESP_LOGI(TAG, "Successfully joined network!");
                ESP_LOGI(TAG, "PAN ID: 0x%04x, Channel: %d",
                         esp_zb_get_pan_id(), esp_zb_get_current_channel());
                g_zigbee_connected = true;
                
                // Solid LED = connected
                gpio_set_level(LED_STATUS_PIN, 1);
                led_blink(LED_ACTIVITY_PIN, 5, 100);
            } else {
                ESP_LOGW(TAG, "Network steering failed (status: %d), retrying...", err_status);
                esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                       ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
            }
            break;

        case ESP_ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
            if (err_status == ESP_OK) {
                uint8_t *permit_join = (uint8_t *)esp_zb_app_signal_get_params(p_sg_p);
                ESP_LOGI(TAG, "Network %s for joining", *permit_join ? "open" : "closed");
            }
            break;

        case ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE: {
            // A device joined through this router!
            esp_zb_zdo_signal_device_annce_params_t *dev_annce = 
                (esp_zb_zdo_signal_device_annce_params_t *)esp_zb_app_signal_get_params(p_sg_p);
            ESP_LOGI(TAG, "Device joined via router! Addr: 0x%04x", dev_annce->device_short_addr);
            led_blink(LED_ACTIVITY_PIN, 3, 100);
            break;
        }

        case ESP_ZB_COMMON_SIGNAL_CAN_SLEEP:
            // Router should not sleep - always ready to relay
            break;

        default:
            ESP_LOGD(TAG, "ZDO signal: %d, status: %d", sig_type, err_status);
            break;
    }
}

static void zigbee_task(void *pvParameters)
{
    // Configure as ROUTER - this enables message relaying!
    esp_zb_cfg_t zb_nwk_cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER,  // KEY: Router role for mesh
        .install_code_policy = false,
        .nwk_cfg.zczr_cfg = {
            .max_children = 10,  // Can have child devices
        },
    };
    esp_zb_init(&zb_nwk_cfg);

    // Create minimal endpoint (required but no clusters needed for routing)
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
    
    // Basic cluster for identification
    esp_zb_basic_cluster_cfg_t basic_cfg = {
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
        .power_source = 0x01, // Mains powered
    };
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&basic_cfg);
    esp_zb_cluster_list_add_basic_cluster(cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = ROUTER_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);
    esp_zb_device_register(ep_list);

    ESP_LOGI(TAG, "Zigbee Router configured - will relay messages");
    ESP_LOGI(TAG, "Starting Zigbee stack...");
    
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/* ============================================================================
 * STATUS TASK
 * ============================================================================ */

static void status_task(void *pvParameters)
{
    while (1) {
        g_uptime_seconds++;
        
        if (g_zigbee_connected) {
            // Heartbeat blink every 10 seconds
            if (g_uptime_seconds % 10 == 0) {
                led_activity_pulse();
            }
            
            // Log status every minute
            if (g_uptime_seconds % 60 == 0) {
                ESP_LOGI(TAG, "Router Status: Uptime=%lu min, Connected=YES, PAN=0x%04x, CH=%d",
                         g_uptime_seconds / 60,
                         esp_zb_get_pan_id(),
                         esp_zb_get_current_channel());
            }
        } else {
            // Blink status LED when not connected
            static bool toggle = false;
            toggle = !toggle;
            gpio_set_level(LED_STATUS_PIN, toggle);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

void app_main(void)
{
    // Print Cultivio brand banner
    PRINT_CULTIVIO_COMPACT();
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  %s - Router Node", CULTIVIO_PRODUCT_INFO);
    ESP_LOGI(TAG, "  Zigbee Range Extender / Repeater");
    ESP_LOGI(TAG, "  %s", CULTIVIO_COPYRIGHT);
    ESP_LOGI(TAG, "========================================");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize LEDs
    led_init();
    led_blink(LED_STATUS_PIN, 3, 100);
    led_blink(LED_ACTIVITY_PIN, 3, 100);
    
    ESP_LOGI(TAG, "Hardware initialized");
    ESP_LOGI(TAG, "Starting Zigbee Router...");
    ESP_LOGI(TAG, "This node will extend range between Sensor and Controller");

    // Start Zigbee (Router role)
    xTaskCreate(zigbee_task, "zigbee_task", 4096, NULL, 5, NULL);
    
    // Start status monitoring
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreate(status_task, "status_task", 2048, NULL, 3, NULL);
    
    ESP_LOGI(TAG, "Router Node started - waiting for network...");
}

