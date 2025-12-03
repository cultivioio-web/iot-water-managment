/*
 * BLE Provisioning Implementation
 * Provides BLE GATT server for device configuration
 */

#include "ble_provision.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

static const char *TAG = "BLE_PROV";

/* ============================================================================
 * BLE CONFIGURATION
 * ============================================================================ */

#define DEVICE_NAME_PREFIX      "Cultivio-"
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_CHAR_UUID_CONFIG  0xFF01
#define GATTS_CHAR_UUID_STATUS  0xFF02
#define GATTS_CHAR_UUID_CMD     0xFF03

#define GATTS_NUM_HANDLE        8
#define PROFILE_NUM             1
#define PROFILE_APP_ID          0

#define ADV_CONFIG_FLAG         (1 << 0)
#define SCAN_RSP_CONFIG_FLAG    (1 << 1)

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

static device_config_t g_device_config = {0};
static prov_state_t g_prov_state = PROV_STATE_NOT_PROVISIONED;
static prov_node_type_t g_node_type = NODE_TYPE_SENSOR;
static void (*g_complete_callback)(const device_config_t *config) = NULL;

// Mutex for thread-safe config access (FIX: BUG #1)
static SemaphoreHandle_t g_config_mutex = NULL;

static uint8_t adv_config_done = 0;
static uint16_t gatts_handle_table[GATTS_NUM_HANDLE];
static uint8_t service_uuid[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* ============================================================================
 * GATT SERVER CALLBACKS
 * ============================================================================ */

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param);

static struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
} gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};

/* ============================================================================
 * FORWARD DECLARATIONS
 * ============================================================================ */

void ble_handle_pump_command(uint8_t command, uint16_t duration_minutes);

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */

static void set_device_name(void) {
    char name[64];  // FIX: BUG #6 - Larger buffer for safety
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    
    // Use custom name if set, otherwise use MAC-based name
    if (strlen(g_device_config.custom_name) > 0) {
        // Format: "Cultivio-CustomName"
        snprintf(name, sizeof(name), "%s%s", 
                 DEVICE_NAME_PREFIX, g_device_config.custom_name);
    } else {
        // Default format: "Cultivio-XXXX" (last 4 hex of MAC)
        snprintf(name, sizeof(name), "%s%02X%02X", 
                 DEVICE_NAME_PREFIX, mac[4], mac[5]);
    }
    
    // FIX: BUG #6 - Ensure null termination and bounds check
    name[sizeof(name) - 1] = '\0';
    
    esp_ble_gap_set_device_name(name);
    
    // Copy to config with bounds checking
    strncpy(g_device_config.device_name, name, sizeof(g_device_config.device_name) - 1);
    g_device_config.device_name[sizeof(g_device_config.device_name) - 1] = '\0';
    
    ESP_LOGI(TAG, "Device name set to: %s", name);
}

static void parse_config_data(const uint8_t *data, uint16_t len) {
    if (len < 1) return;
    
    // Acquire mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex == NULL || xSemaphoreTake(g_config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire config mutex");
        return;
    }
    
    uint8_t cmd = data[0];
    
    switch (cmd) {
        case 0x00: // Set device role (NEW!)
            if (len >= 2) {
                uint8_t role = data[1];
                if (role == NODE_TYPE_SENSOR || role == NODE_TYPE_CONTROLLER || role == NODE_TYPE_ROUTER) {
                    g_device_config.node_type = (prov_node_type_t)role;
                    g_node_type = (prov_node_type_t)role;
                    ESP_LOGI(TAG, "Device role set to: %s",
                             role == NODE_TYPE_SENSOR ? "SENSOR" :
                             role == NODE_TYPE_CONTROLLER ? "CONTROLLER" : "ROUTER");
                } else {
                    ESP_LOGW(TAG, "Invalid role: %d", role);
                }
            }
            break;
            
        case 0x01: // Set tank config (for sensor)
            if (len >= 7) {
                uint16_t height = (data[1] << 8) | data[2];
                uint16_t diameter = (data[3] << 8) | data[4];
                uint8_t offset = data[5];
                uint16_t interval = (data[6] << 8) | (len > 7 ? data[7] : 5);
                
                // FIX: SEC #3 - Validate all inputs
                bool valid = true;
                
                if (height < 50 || height > 1000) {
                    ESP_LOGW(TAG, "Invalid tank height: %d (must be 50-1000 cm)", height);
                    valid = false;
                }
                if (diameter < 30 || diameter > 500) {
                    ESP_LOGW(TAG, "Invalid tank diameter: %d (must be 30-500 cm)", diameter);
                    valid = false;
                }
                if (offset > 50) {
                    ESP_LOGW(TAG, "Invalid sensor offset: %d (must be 0-50 cm)", offset);
                    valid = false;
                }
                if (interval < 1 || interval > 300) {
                    ESP_LOGW(TAG, "Invalid report interval: %d (must be 1-300 sec)", interval);
                    valid = false;
                }
                
                if (valid) {
                    g_device_config.tank_height_cm = height;
                    g_device_config.tank_diameter_cm = diameter;
                    g_device_config.sensor_offset_cm = offset;
                    g_device_config.report_interval_sec = interval;
                    ESP_LOGI(TAG, "Tank config: H=%d, D=%d, Off=%d, Int=%d",
                             g_device_config.tank_height_cm,
                             g_device_config.tank_diameter_cm,
                             g_device_config.sensor_offset_cm,
                             g_device_config.report_interval_sec);
                } else {
                    ESP_LOGE(TAG, "Tank config rejected due to invalid values");
                }
            }
            break;
            
        case 0x02: // Set pump thresholds (for controller)
            if (len >= 6) {
                uint8_t on_threshold = data[1];
                uint8_t off_threshold = data[2];
                uint16_t timeout = (data[3] << 8) | data[4];
                
                // FIX: SEC #3 - Validate pump settings
                bool valid = true;
                
                if (on_threshold > 100) {
                    ESP_LOGW(TAG, "Invalid ON threshold: %d (must be 0-100%%)", on_threshold);
                    valid = false;
                }
                if (off_threshold > 100) {
                    ESP_LOGW(TAG, "Invalid OFF threshold: %d (must be 0-100%%)", off_threshold);
                    valid = false;
                }
                if (on_threshold >= off_threshold) {
                    ESP_LOGW(TAG, "Logic error: ON threshold (%d%%) must be < OFF threshold (%d%%)", 
                             on_threshold, off_threshold);
                    valid = false;
                }
                if (timeout < 1 || timeout > 120) {
                    ESP_LOGW(TAG, "Invalid pump timeout: %d (must be 1-120 min)", timeout);
                    valid = false;
                }
                
                if (valid) {
                    g_device_config.pump_on_threshold = on_threshold;
                    g_device_config.pump_off_threshold = off_threshold;
                    g_device_config.pump_timeout_minutes = timeout;
                    ESP_LOGI(TAG, "Pump config: ON=%d%%, OFF=%d%%, Timeout=%dmin",
                             g_device_config.pump_on_threshold,
                             g_device_config.pump_off_threshold,
                             g_device_config.pump_timeout_minutes);
                } else {
                    ESP_LOGE(TAG, "Pump config rejected due to invalid values");
                }
            }
            break;
            
        case 0x03: // Set Zigbee config
            if (len >= 4) {
                uint16_t pan_id = (data[1] << 8) | data[2];
                uint8_t channel = data[3];
                
                // FIX: SEC #3 - Validate Zigbee settings
                // Valid 802.15.4 channels: 11-26
                if (channel < 11 || channel > 26) {
                    ESP_LOGW(TAG, "Invalid Zigbee channel: %d (must be 11-26)", channel);
                } else {
                    g_device_config.zigbee_pan_id = pan_id;
                    g_device_config.zigbee_channel = channel;
                    ESP_LOGI(TAG, "Zigbee config: PAN=0x%04X, CH=%d",
                             g_device_config.zigbee_pan_id,
                             g_device_config.zigbee_channel);
                }
            }
            break;
            
        case 0x04: // Manual pump command (for controller status mode)
            if (len >= 4) {
                uint8_t pump_cmd = data[1];
                uint16_t duration = (data[2] << 8) | data[3];
                ble_handle_pump_command(pump_cmd, duration);
            }
            break;
            
        case 0x05: // Set custom device name
            if (len >= 2) {
                // Data format: [0x05, name_length, name_bytes...]
                uint8_t name_len = data[1];
                
                // FIX: BUG #6 - Strict validation to prevent buffer overflow
                if (name_len == 0 || name_len >= MAX_CUSTOM_NAME_LENGTH || len < (2 + name_len)) {
                    ESP_LOGW(TAG, "Invalid custom name length: %d (max: %d)", 
                             name_len, MAX_CUSTOM_NAME_LENGTH - 1);
                    break;
                }
                
                // Clear and copy with bounds checking
                memset(g_device_config.custom_name, 0, sizeof(g_device_config.custom_name));
                memcpy(g_device_config.custom_name, &data[2], name_len);
                
                // Force null termination for safety
                g_device_config.custom_name[MAX_CUSTOM_NAME_LENGTH - 1] = '\0';
                
                // Validate final name won't overflow when combined with prefix
                char temp_name[64];  // Larger temp buffer for validation
                int written = snprintf(temp_name, sizeof(temp_name), "%s%s", 
                                     DEVICE_NAME_PREFIX, g_device_config.custom_name);
                
                if (written >= sizeof(g_device_config.device_name)) {
                    ESP_LOGE(TAG, "Device name too long after prefix! Truncating.");
                    // Truncate custom name to fit
                    size_t max_custom = sizeof(g_device_config.device_name) - strlen(DEVICE_NAME_PREFIX) - 1;
                    g_device_config.custom_name[max_custom] = '\0';
                }
                
                // Update BLE device name immediately
                set_device_name();
                
                ESP_LOGI(TAG, "Custom name set: %s (Full: %s)", 
                         g_device_config.custom_name, g_device_config.device_name);
            }
            break;
            
        case 0x06: // Set password
            if (len >= 2) {
                // Data format: [0x06, password_length, password_bytes...]
                uint8_t pwd_len = data[1];
                if (pwd_len >= 4 && pwd_len < MAX_PASSWORD_LENGTH && len >= (2 + pwd_len)) {
                    memset(g_device_config.password, 0, sizeof(g_device_config.password));
                    memcpy(g_device_config.password, &data[2], pwd_len);
                    g_device_config.password[pwd_len] = '\0';
                    g_device_config.password_enabled = true;
                    
                    ESP_LOGI(TAG, "Password set (length: %d)", pwd_len);
                } else if (pwd_len == 0) {
                    // Disable password
                    g_device_config.password_enabled = false;
                    memset(g_device_config.password, 0, sizeof(g_device_config.password));
                    ESP_LOGI(TAG, "Password disabled");
                }
            }
            break;
            
        case 0x07: // Set location info
            if (len >= 2) {
                uint8_t loc_len = data[1];
                if (loc_len > 0 && loc_len < sizeof(g_device_config.location) && len >= (2 + loc_len)) {
                    memset(g_device_config.location, 0, sizeof(g_device_config.location));
                    memcpy(g_device_config.location, &data[2], loc_len);
                    g_device_config.location[loc_len] = '\0';
                    ESP_LOGI(TAG, "Location set: %s", g_device_config.location);
                }
            }
            break;
            
        case 0x10: // Complete provisioning
            g_device_config.provisioned = true;
            g_device_config.provision_timestamp = esp_log_timestamp();
            ble_provision_save_config(&g_device_config);
            g_prov_state = PROV_STATE_PROVISIONED;
            ESP_LOGI(TAG, "Provisioning complete! Role: %s",
                     g_device_config.node_type == NODE_TYPE_SENSOR ? "SENSOR" :
                     g_device_config.node_type == NODE_TYPE_CONTROLLER ? "CONTROLLER" : "ROUTER");
            
            if (g_complete_callback) {
                g_complete_callback(&g_device_config);
            }
            break;
            
        case 0xFF: // Factory reset
            ble_provision_reset();
            break;
    }
    
    // Release mutex (FIX: BUG #1)
    xSemaphoreGive(g_config_mutex);
}

static void prepare_status_response(uint8_t *data, uint16_t *len) {
    // Clear buffer first to prevent stack memory exposure (FIX: SEC #4)
    memset(data, 0, 64);
    
    // Acquire mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex && xSemaphoreTake(g_config_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        data[0] = g_node_type;
        data[1] = g_prov_state;
        data[2] = g_device_config.provisioned ? 1 : 0;
        data[3] = (g_device_config.tank_height_cm >> 8) & 0xFF;
        data[4] = g_device_config.tank_height_cm & 0xFF;
        data[5] = g_device_config.pump_on_threshold;
        data[6] = g_device_config.pump_off_threshold;
        data[7] = (g_device_config.zigbee_pan_id >> 8) & 0xFF;
        data[8] = g_device_config.zigbee_pan_id & 0xFF;
        data[9] = g_device_config.zigbee_channel;
        *len = 10;
        xSemaphoreGive(g_config_mutex);
    } else {
        ESP_LOGW(TAG, "Could not acquire mutex for status response");
        *len = 0;
    }
}

/* ============================================================================
 * GAP EVENT HANDLER
 * ============================================================================ */

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
            
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
            
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising start failed");
            } else {
                ESP_LOGI(TAG, "Advertising started - ready for provisioning");
                g_prov_state = PROV_STATE_PROVISIONING;
            }
            break;
            
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising stopped");
            break;
            
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG, "Connection params updated");
            break;
            
        default:
            break;
    }
}

/* ============================================================================
 * GATTS EVENT HANDLER
 * ============================================================================ */

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param) {
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGE(TAG, "Reg app failed, app_id %04x, status %d",
                     param->reg.app_id, param->reg.status);
            return;
        }
    }

    for (int idx = 0; idx < PROFILE_NUM; idx++) {
        if (gatts_if == ESP_GATT_IF_NONE ||
            gatts_if == gl_profile_tab[idx].gatts_if) {
            if (gl_profile_tab[idx].gatts_cb) {
                gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
            }
        }
    }
}

static const uint8_t char_prop_rw = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_r = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const esp_gatts_attr_db_t gatt_db[GATTS_NUM_HANDLE] = {
    // Service Declaration
    [0] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){ESP_GATT_UUID_PRI_SERVICE},
            ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID), (uint8_t *)&(uint16_t){GATTS_SERVICE_UUID}}},
    
    // Config Characteristic Declaration
    [1] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){ESP_GATT_UUID_CHAR_DECLARE},
            ESP_GATT_PERM_READ, sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_rw}},
    
    // Config Characteristic Value
    [2] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){GATTS_CHAR_UUID_CONFIG},
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 512, 0, NULL}},
    
    // Status Characteristic Declaration
    [3] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){ESP_GATT_UUID_CHAR_DECLARE},
            ESP_GATT_PERM_READ, sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_r}},
    
    // Status Characteristic Value
    [4] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){GATTS_CHAR_UUID_STATUS},
            ESP_GATT_PERM_READ, 64, 0, NULL}},
    
    // Command Characteristic Declaration
    [5] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){ESP_GATT_UUID_CHAR_DECLARE},
            ESP_GATT_PERM_READ, sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_rw}},
    
    // Command Characteristic Value
    [6] = {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&(uint16_t){GATTS_CHAR_UUID_CMD},
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 64, 0, NULL}},
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            set_device_name();
            esp_ble_gap_config_adv_data(&adv_data);
            adv_config_done |= ADV_CONFIG_FLAG;
            esp_ble_gap_config_adv_data(&scan_rsp_data);
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, GATTS_NUM_HANDLE, 0);
            break;
            
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "Create attr table failed, error code=0x%x", param->add_attr_tab.status);
            } else {
                memcpy(gatts_handle_table, param->add_attr_tab.handles, sizeof(gatts_handle_table));
                esp_ble_gatts_start_service(gatts_handle_table[0]);
            }
            break;
            
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "Client connected, conn_id=%d", param->connect.conn_id);
            gl_profile_tab[PROFILE_APP_ID].conn_id = param->connect.conn_id;
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "Client disconnected, reason=0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);
            break;
            
        case ESP_GATTS_READ_EVT: {
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            
            if (param->read.handle == gatts_handle_table[4]) {
                // Status read
                prepare_status_response(rsp.attr_value.value, &rsp.attr_value.len);
            }
            
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                       param->read.trans_id, ESP_GATT_OK, &rsp);
            break;
        }
            
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep) {
                if (param->write.handle == gatts_handle_table[2] ||
                    param->write.handle == gatts_handle_table[6]) {
                    parse_config_data(param->write.value, param->write.len);
                }
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

/* ============================================================================
 * PUBLIC API IMPLEMENTATION
 * ============================================================================ */

esp_err_t ble_provision_init(prov_node_type_t node_type) {
    g_node_type = node_type;
    
    // Create mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex == NULL) {
        g_config_mutex = xSemaphoreCreateMutex();
        if (g_config_mutex == NULL) {
            ESP_LOGE(TAG, "Failed to create config mutex!");
            return ESP_ERR_NO_MEM;
        }
    }
    
    // Set defaults
    memset(&g_device_config, 0, sizeof(g_device_config));
    g_device_config.node_type = node_type;
    g_device_config.tank_height_cm = 200;
    g_device_config.tank_diameter_cm = 100;
    g_device_config.sensor_offset_cm = 5;
    g_device_config.pump_on_threshold = 20;
    g_device_config.pump_off_threshold = 80;
    g_device_config.pump_timeout_minutes = 60;
    g_device_config.zigbee_pan_id = 0x1234;
    g_device_config.zigbee_channel = 15;
    g_device_config.report_interval_sec = 5;
    g_device_config.provisioned = false;
    
    // Set default password
    strncpy(g_device_config.password, DEFAULT_DEVICE_PASSWORD, MAX_PASSWORD_LENGTH - 1);
    g_device_config.password_enabled = false;  // Disabled by default, user can enable
    
    // Custom name empty by default (will use MAC-based name)
    g_device_config.custom_name[0] = '\0';
    g_device_config.location[0] = '\0';
    
    // Try to load existing config
    ble_provision_load_config(&g_device_config);
    
    if (g_device_config.provisioned) {
        g_prov_state = PROV_STATE_PROVISIONED;
        ESP_LOGI(TAG, "Device already provisioned: %s", g_device_config.device_name);
        if (strlen(g_device_config.custom_name) > 0) {
            ESP_LOGI(TAG, "Custom name: %s", g_device_config.custom_name);
        }
        if (strlen(g_device_config.location) > 0) {
            ESP_LOGI(TAG, "Location: %s", g_device_config.location);
        }
    }
    
    ESP_LOGI(TAG, "BLE Provisioning initialized");
    ESP_LOGI(TAG, "Default password: %s (change after setup!)", DEFAULT_DEVICE_PASSWORD);
    
    return ESP_OK;
}

esp_err_t ble_provision_start(void) {
    esp_err_t ret;
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "BT controller init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "BT controller enable failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "GATTS register callback failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "GAP register callback failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // FIX: SEC #2 - Enable BLE security (encryption and authentication)
    ESP_LOGI(TAG, "Configuring BLE security...");
    
    // Set authentication requirements: Secure Connections + MITM protection + Bonding
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
    ret = esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    if (ret) {
        ESP_LOGW(TAG, "Set auth req failed: %s", esp_err_to_name(ret));
    }
    
    // Set IO capability (display-only for PIN display)
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    ret = esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    if (ret) {
        ESP_LOGW(TAG, "Set IO cap failed: %s", esp_err_to_name(ret));
    }
    
    // Set key size (128-bit)
    uint8_t key_size = 16;
    ret = esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    if (ret) {
        ESP_LOGW(TAG, "Set key size failed: %s", esp_err_to_name(ret));
    }
    
    // Enable encryption
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    ret = esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    if (ret) {
        ESP_LOGW(TAG, "Set init key failed: %s", esp_err_to_name(ret));
    }
    
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    ret = esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
    if (ret) {
        ESP_LOGW(TAG, "Set rsp key failed: %s", esp_err_to_name(ret));
    }
    
    ESP_LOGI(TAG, "BLE security configured - pairing required for connections");
    
    ret = esp_ble_gatts_app_register(PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "GATTS app register failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    esp_ble_gatt_set_local_mtu(500);
    
    ESP_LOGI(TAG, "BLE Provisioning started");
    return ESP_OK;
}

esp_err_t ble_provision_stop(void) {
    esp_ble_gap_stop_advertising();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    ESP_LOGI(TAG, "BLE Provisioning stopped");
    return ESP_OK;
}

bool ble_provision_is_provisioned(void) {
    return g_device_config.provisioned;
}

esp_err_t ble_provision_get_config(device_config_t *config) {
    if (!config) return ESP_ERR_INVALID_ARG;
    
    // Acquire mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex == NULL || xSemaphoreTake(g_config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire config mutex in get_config");
        return ESP_ERR_TIMEOUT;
    }
    
    memcpy(config, &g_device_config, sizeof(device_config_t));
    
    xSemaphoreGive(g_config_mutex);
    return ESP_OK;
}

esp_err_t ble_provision_save_config(const device_config_t *config) {
    nvs_handle_t nvs;
    esp_err_t ret;
    
    // Acquire mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex == NULL || xSemaphoreTake(g_config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire config mutex in save_config");
        return ESP_ERR_TIMEOUT;
    }
    
    ret = nvs_open("provision", NVS_READWRITE, &nvs);
    if (ret != ESP_OK) {
        xSemaphoreGive(g_config_mutex);
        return ret;
    }
    
    ret = nvs_set_blob(nvs, "config", config, sizeof(device_config_t));
    if (ret == ESP_OK) {
        ret = nvs_commit(nvs);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(ret));
        }
    }
    nvs_close(nvs);
    
    xSemaphoreGive(g_config_mutex);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Configuration saved to NVS");
    }
    return ret;
}

esp_err_t ble_provision_load_config(device_config_t *config) {
    nvs_handle_t nvs;
    esp_err_t ret;
    
    // Acquire mutex for thread-safe config access (FIX: BUG #1)
    if (g_config_mutex == NULL || xSemaphoreTake(g_config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire config mutex in load_config");
        return ESP_ERR_TIMEOUT;
    }
    
    ret = nvs_open("provision", NVS_READONLY, &nvs);
    if (ret != ESP_OK) {
        xSemaphoreGive(g_config_mutex);
        return ret;
    }
    
    size_t len = sizeof(device_config_t);
    ret = nvs_get_blob(nvs, "config", config, &len);
    nvs_close(nvs);
    
    xSemaphoreGive(g_config_mutex);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Configuration loaded from NVS");
    }
    return ret;
}

esp_err_t ble_provision_reset(void) {
    nvs_handle_t nvs;
    esp_err_t ret = nvs_open("provision", NVS_READWRITE, &nvs);
    if (ret == ESP_OK) {
        nvs_erase_all(nvs);
        nvs_commit(nvs);
        nvs_close(nvs);
    }
    
    g_device_config.provisioned = false;
    g_prov_state = PROV_STATE_NOT_PROVISIONED;
    
    ESP_LOGW(TAG, "Factory reset complete");
    return ESP_OK;
}

prov_state_t ble_provision_get_state(void) {
    return g_prov_state;
}

void ble_provision_set_complete_callback(void (*callback)(const device_config_t *config)) {
    g_complete_callback = callback;
}

/* ============================================================================
 * BLE STATUS MONITORING IMPLEMENTATION
 * ============================================================================ */

static device_status_t g_device_status = {0};
static bool g_status_mode_active = false;

esp_err_t ble_status_start(void) {
    if (g_status_mode_active) {
        ESP_LOGW(TAG, "Status mode already active");
        return ESP_OK;
    }
    
    // BLE should already be initialized from provisioning
    // Just start advertising again
    g_status_mode_active = true;
    
    // Start advertising for status monitoring
    esp_ble_gap_start_advertising(&adv_params);
    
    ESP_LOGI(TAG, "BLE Status monitoring started - connect with mobile app to view status");
    return ESP_OK;
}

esp_err_t ble_status_stop(void) {
    if (!g_status_mode_active) {
        return ESP_OK;
    }
    
    esp_ble_gap_stop_advertising();
    g_status_mode_active = false;
    
    ESP_LOGI(TAG, "BLE Status monitoring stopped");
    return ESP_OK;
}

esp_err_t ble_status_update(const device_status_t *status) {
    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    memcpy(&g_device_status, status, sizeof(device_status_t));
    return ESP_OK;
}

bool ble_status_is_active(void) {
    return g_status_mode_active;
}

// Function to get status for BLE characteristic read
const device_status_t* ble_get_current_status(void) {
    return &g_device_status;
}

/* ============================================================================
 * MANUAL PUMP CONTROL IMPLEMENTATION
 * ============================================================================ */

static manual_pump_cmd_callback_t g_pump_cmd_callback = NULL;
static bool g_manual_override_active = false;
static uint32_t g_manual_override_end_time = 0;

void ble_register_pump_cmd_callback(manual_pump_cmd_callback_t callback) {
    g_pump_cmd_callback = callback;
    ESP_LOGI(TAG, "Manual pump command callback registered");
}

bool ble_get_manual_override_status(uint32_t *remaining_seconds) {
    if (!g_manual_override_active) {
        if (remaining_seconds) *remaining_seconds = 0;
        return false;
    }
    
    // FIX: BUG #8 - Use esp_timer for consistent time
    uint32_t current_time = esp_timer_get_time() / 1000000;
    if (current_time >= g_manual_override_end_time) {
        g_manual_override_active = false;
        if (remaining_seconds) *remaining_seconds = 0;
        return false;
    }
    
    if (remaining_seconds) {
        *remaining_seconds = g_manual_override_end_time - current_time;
    }
    return true;
}

// Called when manual pump command is received via BLE
void ble_handle_pump_command(uint8_t command, uint16_t duration_minutes) {
    manual_pump_cmd_t cmd = {
        .command = command,
        .duration_minutes = duration_minutes
    };
    
    if (command == PUMP_CMD_START_TIMED && duration_minutes > 0) {
        // FIX: BUG #8 - Use esp_timer for 64-bit time (no overflow after 49 days)
        g_manual_override_active = true;
        g_manual_override_end_time = (esp_timer_get_time() / 1000000) + (duration_minutes * 60);
        ESP_LOGI(TAG, "Manual pump override: START for %d minutes", duration_minutes);
    } else {
        g_manual_override_active = false;
        g_manual_override_end_time = 0;
        ESP_LOGI(TAG, "Manual pump override: STOP");
    }
    
    if (g_pump_cmd_callback) {
        g_pump_cmd_callback(&cmd);
    }
}

