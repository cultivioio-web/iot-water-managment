/**
 * Cultivio AquaSense - Test Firmware for ESP32-WROOM
 * 
 * This firmware is for testing BLE provisioning and controller logic
 * on a standard ESP32-WROOM while waiting for ESP32-H2 boards.
 * 
 * Features tested:
 * - BLE GATT server
 * - Device provisioning via mobile app
 * - Simulated water level (random values)
 * - Simulated pump control (LED on GPIO2)
 * - Manual pump override
 * - Status broadcasting
 * 
 * Hardware:
 * - ESP32-WROOM module
 * - Built-in LED on GPIO2 (simulates pump relay)
 * - Optional: External LED on GPIO4 (status LED)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_random.h"

static const char *TAG = "CULTIVIO_TEST";

// ============================================================================
// GPIO Configuration (ESP32-WROOM)
// ============================================================================
#define GPIO_PUMP_RELAY     2    // Built-in LED on most ESP32 boards
#define GPIO_STATUS_LED     4    // Optional external status LED
#define GPIO_BUTTON         0    // Boot button for testing

// ============================================================================
// BLE Configuration
// ============================================================================
#define DEVICE_NAME_PREFIX  "Cultivio-"
#define SERVICE_UUID        0x00FF
#define CHAR_CONFIG_UUID    0xFF01
#define CHAR_STATUS_UUID    0xFF02
#define CHAR_CMD_UUID       0xFF03

#define GATTS_NUM_HANDLE    12

// ============================================================================
// Device State
// ============================================================================
typedef enum {
    NODE_TYPE_SENSOR = 0x01,
    NODE_TYPE_CONTROLLER = 0x02,
    NODE_TYPE_ROUTER = 0x03
} node_type_t;

typedef struct {
    char device_name[32];
    char custom_name[32];
    char password[16];
    node_type_t node_type;
    bool is_provisioned;
    
    // Tank config (sensor)
    uint16_t tank_height_cm;
    uint16_t tank_diameter_cm;
    uint8_t sensor_offset_cm;
    uint16_t report_interval_sec;
    
    // Pump config (controller)
    uint8_t pump_on_threshold;
    uint8_t pump_off_threshold;
    uint16_t pump_timeout_min;
    
    // Zigbee config
    uint16_t pan_id;
    uint8_t channel;
} device_config_t;

typedef struct {
    uint8_t water_level_percent;
    bool pump_running;
    bool zigbee_connected;      // Simulated
    bool sensor_online;         // Simulated
    uint32_t uptime_sec;
    int8_t signal_strength;     // Simulated RSSI
    bool manual_mode;
    uint16_t manual_remaining_sec;
} device_status_t;

// ============================================================================
// Global Variables
// ============================================================================
static device_config_t g_config = {
    .node_type = NODE_TYPE_CONTROLLER,
    .is_provisioned = false,
    .tank_height_cm = 100,
    .tank_diameter_cm = 50,
    .sensor_offset_cm = 5,
    .report_interval_sec = 30,
    .pump_on_threshold = 20,
    .pump_off_threshold = 90,
    .pump_timeout_min = 30,
    .pan_id = 0x1234,
    .channel = 15
};

static device_status_t g_status = {
    .water_level_percent = 50,
    .pump_running = false,
    .zigbee_connected = true,    // Simulated as always connected
    .sensor_online = true,       // Simulated as always online
    .uptime_sec = 0,
    .signal_strength = -45,      // Simulated good signal
    .manual_mode = false,
    .manual_remaining_sec = 0
};

static esp_gatt_if_t g_gatts_if = ESP_GATT_IF_NONE;
static uint16_t g_conn_id = 0xFFFF;
static bool g_is_connected = false;
static uint16_t g_handle_table[GATTS_NUM_HANDLE];
static TimerHandle_t g_status_timer = NULL;
static TimerHandle_t g_manual_timer = NULL;

// ============================================================================
// Function Prototypes
// ============================================================================
static void init_gpio(void);
static void init_nvs(void);
static void init_ble(void);
static void set_device_name(void);
static void update_pump_state(bool on);
static void simulate_water_level(void);
static void status_timer_callback(TimerHandle_t timer);
static void manual_timer_callback(TimerHandle_t timer);
static void save_config_to_nvs(void);
static void load_config_from_nvs(void);

// ============================================================================
// BLE GATT Database
// ============================================================================
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t char_decl_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_rw = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_rn = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint16_t service_uuid = SERVICE_UUID;
static const uint16_t config_char_uuid = CHAR_CONFIG_UUID;
static const uint16_t status_char_uuid = CHAR_STATUS_UUID;
static const uint16_t cmd_char_uuid = CHAR_CMD_UUID;

static const esp_gatts_attr_db_t gatt_db[] = {
    // Service Declaration
    [0] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid,
            ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(service_uuid), (uint8_t *)&service_uuid}},
    
    // Config Characteristic Declaration
    [1] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&char_decl_uuid,
            ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&char_prop_rw}},
    // Config Characteristic Value
    [2] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&config_char_uuid,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 128, 0, NULL}},
    
    // Status Characteristic Declaration
    [3] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&char_decl_uuid,
            ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&char_prop_rn}},
    // Status Characteristic Value
    [4] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&status_char_uuid,
            ESP_GATT_PERM_READ, 32, 0, NULL}},
    
    // Command Characteristic Declaration
    [5] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&char_decl_uuid,
            ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&char_prop_rw}},
    // Command Characteristic Value
    [6] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&cmd_char_uuid,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 16, 0, NULL}},
};

// ============================================================================
// GPIO Functions
// ============================================================================
static void init_gpio(void)
{
    // Configure pump relay (LED)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_PUMP_RELAY) | (1ULL << GPIO_STATUS_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    // Configure button input
    io_conf.pin_bit_mask = (1ULL << GPIO_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    
    // Initial state - pump off
    gpio_set_level(GPIO_PUMP_RELAY, 0);
    gpio_set_level(GPIO_STATUS_LED, 0);
    
    ESP_LOGI(TAG, "GPIO initialized - Pump: GPIO%d, Status: GPIO%d, Button: GPIO%d",
             GPIO_PUMP_RELAY, GPIO_STATUS_LED, GPIO_BUTTON);
}

static void update_pump_state(bool on)
{
    g_status.pump_running = on;
    gpio_set_level(GPIO_PUMP_RELAY, on ? 1 : 0);
    ESP_LOGI(TAG, "Pump %s", on ? "ON" : "OFF");
}

// ============================================================================
// NVS Functions
// ============================================================================
static void init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    load_config_from_nvs();
}

static void save_config_to_nvs(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("cultivio", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return;
    }
    
    nvs_set_blob(handle, "config", &g_config, sizeof(g_config));
    nvs_commit(handle);
    nvs_close(handle);
    
    ESP_LOGI(TAG, "Configuration saved to NVS");
}

static void load_config_from_nvs(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("cultivio", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No saved config found, using defaults");
        return;
    }
    
    size_t size = sizeof(g_config);
    err = nvs_get_blob(handle, "config", &g_config, &size);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Configuration loaded from NVS");
        ESP_LOGI(TAG, "  Node type: %d", g_config.node_type);
        ESP_LOGI(TAG, "  Provisioned: %s", g_config.is_provisioned ? "Yes" : "No");
    }
    
    nvs_close(handle);
}

// ============================================================================
// Device Name
// ============================================================================
static void set_device_name(void)
{
    char name[32];
    memset(name, 0, sizeof(name));
    
    // DEVICE_NAME_PREFIX is "Cultivio-" (9 chars)
    // Leave room for prefix + custom name + null terminator
    const size_t prefix_len = strlen(DEVICE_NAME_PREFIX);
    const size_t max_custom_len = sizeof(name) - prefix_len - 1;
    
    if (strlen(g_config.custom_name) > 0) {
        // Copy prefix first
        strncpy(name, DEVICE_NAME_PREFIX, sizeof(name) - 1);
        // Append custom name (truncated if needed)
        strncat(name, g_config.custom_name, max_custom_len);
    } else {
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_BT);
        snprintf(name, sizeof(name), "%s%02X%02X", DEVICE_NAME_PREFIX, mac[4], mac[5]);
    }
    
    name[sizeof(name) - 1] = '\0'; // Ensure null termination
    strncpy(g_config.device_name, name, sizeof(g_config.device_name) - 1);
    g_config.device_name[sizeof(g_config.device_name) - 1] = '\0';
    
    esp_ble_gap_set_device_name(name);
    
    ESP_LOGI(TAG, "Device name: %s", name);
}

// ============================================================================
// Simulation Functions
// ============================================================================
static void simulate_water_level(void)
{
    // Simulate water level changes
    static int direction = -1;
    
    if (g_status.pump_running) {
        // Water level increasing when pump is on
        g_status.water_level_percent += 2;
        if (g_status.water_level_percent >= g_config.pump_off_threshold) {
            g_status.water_level_percent = g_config.pump_off_threshold;
            if (!g_status.manual_mode) {
                update_pump_state(false);
            }
        }
    } else {
        // Water level slowly decreasing (usage)
        if (g_status.water_level_percent > 0) {
            g_status.water_level_percent -= 1;
        }
        
        // Auto-start pump if below threshold
        if (g_status.water_level_percent <= g_config.pump_on_threshold && !g_status.manual_mode) {
            update_pump_state(true);
        }
    }
    
    // Simulate signal strength variation
    g_status.signal_strength = -40 - (esp_random() % 20);
}

static void status_timer_callback(TimerHandle_t timer)
{
    g_status.uptime_sec++;
    
    // Update simulated values every 5 seconds
    if (g_status.uptime_sec % 5 == 0) {
        simulate_water_level();
    }
    
    // Blink status LED
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(GPIO_STATUS_LED, led_state);
}

static void manual_timer_callback(TimerHandle_t timer)
{
    if (g_status.manual_remaining_sec > 0) {
        g_status.manual_remaining_sec--;
        
        if (g_status.manual_remaining_sec == 0) {
            // Manual mode expired
            g_status.manual_mode = false;
            update_pump_state(false);
            ESP_LOGI(TAG, "Manual mode expired, pump OFF");
        }
    }
}

// ============================================================================
// BLE Command Handling
// ============================================================================
static void handle_config_write(const uint8_t *data, uint16_t len)
{
    if (len < 1) return;
    
    uint8_t cmd = data[0];
    ESP_LOGI(TAG, "Config command: 0x%02X, len: %d", cmd, len);
    
    switch (cmd) {
        case 0x00: // Set device role
            if (len >= 2) {
                g_config.node_type = data[1];
                ESP_LOGI(TAG, "Role set to: %s",
                         g_config.node_type == NODE_TYPE_SENSOR ? "SENSOR" :
                         g_config.node_type == NODE_TYPE_CONTROLLER ? "CONTROLLER" : "ROUTER");
            }
            break;
            
        case 0x01: // Tank config
            if (len >= 8) {
                g_config.tank_height_cm = (data[1] << 8) | data[2];
                g_config.tank_diameter_cm = (data[3] << 8) | data[4];
                g_config.sensor_offset_cm = data[5];
                g_config.report_interval_sec = (data[6] << 8) | data[7];
                ESP_LOGI(TAG, "Tank config: H=%d, D=%d, Off=%d, Int=%d",
                         g_config.tank_height_cm, g_config.tank_diameter_cm,
                         g_config.sensor_offset_cm, g_config.report_interval_sec);
            }
            break;
            
        case 0x02: // Pump config
            if (len >= 6) {
                g_config.pump_on_threshold = data[1];
                g_config.pump_off_threshold = data[2];
                g_config.pump_timeout_min = (data[3] << 8) | data[4];
                ESP_LOGI(TAG, "Pump config: ON=%d%%, OFF=%d%%, Timeout=%dmin",
                         g_config.pump_on_threshold, g_config.pump_off_threshold,
                         g_config.pump_timeout_min);
            }
            break;
            
        case 0x03: // Zigbee config
            if (len >= 4) {
                g_config.pan_id = (data[1] << 8) | data[2];
                g_config.channel = data[3];
                ESP_LOGI(TAG, "Zigbee config: PAN=0x%04X, CH=%d",
                         g_config.pan_id, g_config.channel);
            }
            break;
            
        case 0x05: // Custom device name
            if (len > 1) {
                memset(g_config.custom_name, 0, sizeof(g_config.custom_name));
                memcpy(g_config.custom_name, &data[1], len - 1 < 31 ? len - 1 : 31);
                set_device_name();
                ESP_LOGI(TAG, "Custom name set: %s", g_config.custom_name);
            }
            break;
            
        case 0x06: // Password
            if (len > 1) {
                memset(g_config.password, 0, sizeof(g_config.password));
                memcpy(g_config.password, &data[1], len - 1 < 15 ? len - 1 : 15);
                ESP_LOGI(TAG, "Password set");
            }
            break;
            
        case 0x10: // Complete provisioning
            g_config.is_provisioned = true;
            save_config_to_nvs();
            ESP_LOGI(TAG, "*** PROVISIONING COMPLETE ***");
            break;
            
        case 0xFF: // Factory reset
            ESP_LOGW(TAG, "Factory reset requested");
            nvs_flash_erase();
            esp_restart();
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown config command: 0x%02X", cmd);
            break;
    }
}

static void handle_cmd_write(const uint8_t *data, uint16_t len)
{
    if (len < 1) return;
    
    uint8_t cmd = data[0];
    ESP_LOGI(TAG, "Pump command: 0x%02X", cmd);
    
    switch (cmd) {
        case 0x00: // Stop pump
            g_status.manual_mode = false;
            g_status.manual_remaining_sec = 0;
            update_pump_state(false);
            ESP_LOGI(TAG, "Manual pump STOP");
            break;
            
        case 0x01: // Start timed pump
            if (len >= 3) {
                uint16_t duration_min = data[1] | (data[2] << 8);
                if (duration_min > 0 && duration_min <= 60) {
                    g_status.manual_mode = true;
                    g_status.manual_remaining_sec = duration_min * 60;
                    update_pump_state(true);
                    ESP_LOGI(TAG, "Manual pump START for %d minutes", duration_min);
                }
            }
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown pump command: 0x%02X", cmd);
            break;
    }
}

static void build_status_response(uint8_t *data, uint16_t *len)
{
    // Status packet format:
    // [0]: Node type
    // [1]: Water level %
    // [2]: Pump state (0=off, 1=on, 2=manual)
    // [3]: Zigbee connected
    // [4]: Sensor online
    // [5-8]: Uptime (32-bit)
    // [9]: Signal strength (signed)
    // [10]: Provisioned
    // [11-12]: Manual remaining seconds
    
    data[0] = g_config.node_type;
    data[1] = g_status.water_level_percent;
    data[2] = g_status.manual_mode ? 2 : (g_status.pump_running ? 1 : 0);
    data[3] = g_status.zigbee_connected ? 1 : 0;
    data[4] = g_status.sensor_online ? 1 : 0;
    data[5] = (g_status.uptime_sec >> 24) & 0xFF;
    data[6] = (g_status.uptime_sec >> 16) & 0xFF;
    data[7] = (g_status.uptime_sec >> 8) & 0xFF;
    data[8] = g_status.uptime_sec & 0xFF;
    data[9] = (uint8_t)g_status.signal_strength;
    data[10] = g_config.is_provisioned ? 1 : 0;
    data[11] = (g_status.manual_remaining_sec >> 8) & 0xFF;
    data[12] = g_status.manual_remaining_sec & 0xFF;
    
    *len = 13;
}

// ============================================================================
// BLE GAP Event Handler
// ============================================================================
static bool adv_data_set = false;
static bool scan_rsp_set = false;

static void start_advertising(void)
{
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x20,       // 20ms
        .adv_int_max = 0x40,       // 40ms
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    esp_ble_gap_start_advertising(&adv_params);
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising data set complete");
            adv_data_set = true;
            if (scan_rsp_set) {
                start_advertising();
            }
            break;
            
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Scan response data set complete");
            scan_rsp_set = true;
            if (adv_data_set) {
                start_advertising();
            }
            break;
            
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "*** BLE ADVERTISING STARTED - Device visible as: %s ***", g_config.device_name);
            } else {
                ESP_LOGE(TAG, "Advertising start failed, status: %d", param->adv_start_cmpl.status);
            }
            break;
            
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising stopped");
            break;
            
        default:
            break;
    }
}

// ============================================================================
// BLE GATTS Event Handler
// ============================================================================
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                 esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            if (param->reg.status == ESP_GATT_OK) {
                g_gatts_if = gatts_if;
                set_device_name();
                
                // Set advertising data with device name
                esp_ble_gap_set_device_name(g_config.device_name);
                
                // Configure advertising data structure
                esp_ble_adv_data_t adv_data = {
                    .set_scan_rsp = false,
                    .include_name = true,           // Include device name!
                    .include_txpower = false,
                    .min_interval = 0x0006,
                    .max_interval = 0x0010,
                    .appearance = 0x00,
                    .manufacturer_len = 0,
                    .p_manufacturer_data = NULL,
                    .service_data_len = 0,
                    .p_service_data = NULL,
                    .service_uuid_len = 0,
                    .p_service_uuid = NULL,
                    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
                };
                esp_ble_gap_config_adv_data(&adv_data);
                
                // Also set scan response with name
                esp_ble_adv_data_t scan_rsp_data = {
                    .set_scan_rsp = true,
                    .include_name = true,
                    .include_txpower = true,
                    .appearance = 0x00,
                    .manufacturer_len = 0,
                    .p_manufacturer_data = NULL,
                    .service_data_len = 0,
                    .p_service_data = NULL,
                    .service_uuid_len = 0,
                    .p_service_uuid = NULL,
                    .flag = 0,
                };
                esp_ble_gap_config_adv_data(&scan_rsp_data);
                
                // Create attribute table
                esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, 
                                              sizeof(gatt_db) / sizeof(gatt_db[0]), 0);
            }
            break;
            
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status == ESP_GATT_OK) {
                memcpy(g_handle_table, param->add_attr_tab.handles,
                       sizeof(g_handle_table));
                esp_ble_gatts_start_service(g_handle_table[0]);
                ESP_LOGI(TAG, "GATT service started");
            }
            break;
            
        case ESP_GATTS_CONNECT_EVT:
            g_conn_id = param->connect.conn_id;
            g_is_connected = true;
            ESP_LOGI(TAG, "Client connected, conn_id: %d", g_conn_id);
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            g_is_connected = false;
            g_conn_id = 0xFFFF;
            ESP_LOGI(TAG, "Client disconnected, restarting advertising");
            esp_ble_gap_start_advertising(&(esp_ble_adv_params_t){
                .adv_int_min = 0x20,
                .adv_int_max = 0x40,
                .adv_type = ADV_TYPE_IND,
                .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
                .channel_map = ADV_CHNL_ALL,
                .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
            });
            break;
            
        case ESP_GATTS_READ_EVT:
            if (param->read.handle == g_handle_table[4]) {
                // Status characteristic read
                esp_gatt_rsp_t rsp = {0};
                rsp.attr_value.handle = param->read.handle;
                build_status_response(rsp.attr_value.value, &rsp.attr_value.len);
                esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                            param->read.trans_id, ESP_GATT_OK, &rsp);
            } else {
                // Default empty response
                esp_gatt_rsp_t rsp = {0};
                rsp.attr_value.handle = param->read.handle;
                rsp.attr_value.len = 0;
                esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                            param->read.trans_id, ESP_GATT_OK, &rsp);
            }
            break;
            
        case ESP_GATTS_WRITE_EVT:
            if (param->write.handle == g_handle_table[2]) {
                // Config characteristic write
                handle_config_write(param->write.value, param->write.len);
            } else if (param->write.handle == g_handle_table[6]) {
                // Command characteristic write
                handle_cmd_write(param->write.value, param->write.len);
            }
            
            if (param->write.need_rsp) {
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                            param->write.trans_id, ESP_GATT_OK, NULL);
            }
            break;
            
        default:
            break;
    }
}

// ============================================================================
// BLE Initialization
// ============================================================================
static void init_ble(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));
    
    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));
    
    ESP_LOGI(TAG, "BLE initialized");
}

// ============================================================================
// Main Application
// ============================================================================
void app_main(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═══════════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║     CULTIVIO AQUASENSE - TEST FIRMWARE                ║");
    ESP_LOGI(TAG, "║     For ESP32-WROOM (BLE Provisioning Test)           ║");
    ESP_LOGI(TAG, "╚═══════════════════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");
    
    // Initialize components
    init_nvs();
    init_gpio();
    init_ble();
    
    // Create status update timer (1 second interval)
    g_status_timer = xTimerCreate("status", pdMS_TO_TICKS(1000), pdTRUE, NULL, status_timer_callback);
    xTimerStart(g_status_timer, 0);
    
    // Create manual pump timer (1 second interval)
    g_manual_timer = xTimerCreate("manual", pdMS_TO_TICKS(1000), pdTRUE, NULL, manual_timer_callback);
    xTimerStart(g_manual_timer, 0);
    
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "=== TEST FIRMWARE READY ===");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Hardware:");
    ESP_LOGI(TAG, "  - Pump LED (simulated relay): GPIO%d", GPIO_PUMP_RELAY);
    ESP_LOGI(TAG, "  - Status LED: GPIO%d", GPIO_STATUS_LED);
    ESP_LOGI(TAG, "  - Button: GPIO%d", GPIO_BUTTON);
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "BLE Device: %s", g_config.device_name);
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Features to test:");
    ESP_LOGI(TAG, "  1. Open provisioning app in Chrome/Edge");
    ESP_LOGI(TAG, "  2. Scan for '%s'", g_config.device_name);
    ESP_LOGI(TAG, "  3. Connect and configure device role");
    ESP_LOGI(TAG, "  4. Set tank/pump parameters");
    ESP_LOGI(TAG, "  5. Test manual pump control");
    ESP_LOGI(TAG, "  6. Monitor simulated water level");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Simulated behavior:");
    ESP_LOGI(TAG, "  - Water level decreases slowly (usage)");
    ESP_LOGI(TAG, "  - Pump turns ON when level <= %d%%", g_config.pump_on_threshold);
    ESP_LOGI(TAG, "  - Pump turns OFF when level >= %d%%", g_config.pump_off_threshold);
    ESP_LOGI(TAG, "  - GPIO%d LED indicates pump state", GPIO_PUMP_RELAY);
    ESP_LOGI(TAG, "");
    
    // Main loop - just monitoring
    while (1) {
        // Check button for manual reset
        if (gpio_get_level(GPIO_BUTTON) == 0) {
            ESP_LOGW(TAG, "Button pressed - hold for 3 sec to factory reset");
            int hold_time = 0;
            while (gpio_get_level(GPIO_BUTTON) == 0 && hold_time < 30) {
                vTaskDelay(pdMS_TO_TICKS(100));
                hold_time++;
            }
            if (hold_time >= 30) {
                ESP_LOGW(TAG, "Factory reset triggered!");
                nvs_flash_erase();
                esp_restart();
            }
        }
        
        // Print status periodically
        static uint32_t last_print = 0;
        if (g_status.uptime_sec - last_print >= 10) {
            last_print = g_status.uptime_sec;
            ESP_LOGI(TAG, "Status: Water=%d%%, Pump=%s, Connected=%s, Uptime=%lus",
                     g_status.water_level_percent,
                     g_status.pump_running ? "ON" : "OFF",
                     g_is_connected ? "Yes" : "No",
                     (unsigned long)g_status.uptime_sec);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

