/*
 * BLE Provisioning System for Water Management Nodes
 * ESP32-H2 Compatible
 */

#ifndef BLE_PROVISION_H
#define BLE_PROVISION_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* ============================================================================
 * PROVISIONING CONFIGURATION
 * ============================================================================ */

// Node types
typedef enum {
    NODE_TYPE_SENSOR = 0x01,
    NODE_TYPE_CONTROLLER = 0x02,
    NODE_TYPE_ROUTER = 0x03
} prov_node_type_t;

// Provisioning states
typedef enum {
    PROV_STATE_NOT_PROVISIONED = 0,
    PROV_STATE_PROVISIONING,
    PROV_STATE_PROVISIONED
} prov_state_t;

/* ============================================================================
 * SECURITY & NAMING
 * ============================================================================ */

// Default password for new devices (user should change this!)
#define DEFAULT_DEVICE_PASSWORD     "1234"
#define MAX_PASSWORD_LENGTH         16
#define MAX_CUSTOM_NAME_LENGTH      20  // "Cultivio-" + custom name

/* ============================================================================
 * CONFIGURATION STRUCTURE
 * ============================================================================ */

typedef struct {
    // Device identification
    char device_name[32];                     // Full BLE name: "Cultivio-Flat301"
    char custom_name[MAX_CUSTOM_NAME_LENGTH]; // User part: "Flat301" or "3rdFloor"
    char password[MAX_PASSWORD_LENGTH];       // BLE connection password
    char location[32];                        // e.g., "Building A, 3rd Floor"
    bool password_enabled;                    // Require password to connect?
    prov_node_type_t node_type;
    
    // Tank configuration (for sensor node)
    uint16_t tank_height_cm;
    uint16_t tank_diameter_cm;
    uint8_t  sensor_offset_cm;
    
    // Thresholds (for controller node)
    uint8_t  pump_on_threshold;      // % to turn pump ON
    uint8_t  pump_off_threshold;     // % to turn pump OFF
    uint16_t pump_timeout_minutes;
    
    // Network settings
    uint16_t zigbee_pan_id;
    uint8_t  zigbee_channel;
    
    // Reporting interval (seconds)
    uint16_t report_interval_sec;
    
    // Flags
    bool provisioned;
    uint32_t provision_timestamp;
} device_config_t;

/* ============================================================================
 * API FUNCTIONS
 * ============================================================================ */

/**
 * Initialize BLE provisioning system
 * @param node_type The type of node (sensor or controller)
 * @return ESP_OK on success
 */
esp_err_t ble_provision_init(prov_node_type_t node_type);

/**
 * Start BLE advertising for provisioning
 * @return ESP_OK on success
 */
esp_err_t ble_provision_start(void);

/**
 * Stop BLE provisioning
 * @return ESP_OK on success
 */
esp_err_t ble_provision_stop(void);

/**
 * Check if device is provisioned
 * @return true if provisioned
 */
bool ble_provision_is_provisioned(void);

/**
 * Get current device configuration
 * @param config Pointer to store configuration
 * @return ESP_OK on success
 */
esp_err_t ble_provision_get_config(device_config_t *config);

/**
 * Save configuration to NVS
 * @param config Configuration to save
 * @return ESP_OK on success
 */
esp_err_t ble_provision_save_config(const device_config_t *config);

/**
 * Load configuration from NVS
 * @param config Pointer to store loaded configuration
 * @return ESP_OK on success
 */
esp_err_t ble_provision_load_config(device_config_t *config);

/**
 * Reset provisioning (factory reset)
 * @return ESP_OK on success
 */
esp_err_t ble_provision_reset(void);

/**
 * Get current provisioning state
 * @return Current state
 */
prov_state_t ble_provision_get_state(void);

/**
 * Set callback for provisioning complete
 * @param callback Function to call when provisioning completes
 */
void ble_provision_set_complete_callback(void (*callback)(const device_config_t *config));

/* ============================================================================
 * BLE STATUS MONITORING (Post-Provisioning)
 * ============================================================================ */

/**
 * Real-time device status structure
 * This is broadcast via BLE for mobile monitoring
 */
typedef struct {
    // Common status
    uint8_t  node_type;           // NODE_TYPE_SENSOR or NODE_TYPE_CONTROLLER
    bool     zigbee_connected;    // Is Zigbee network connected
    uint32_t uptime_seconds;      // Device uptime
    
    // Sensor node specific
    uint8_t  water_level_percent; // 0-100%
    uint16_t water_level_cm;      // Current water depth in cm
    uint8_t  sensor_status;       // 0=OK, 1=Error
    
    // Controller node specific
    bool     pump_active;         // Is pump running
    uint32_t pump_runtime_sec;    // Total pump runtime today
    uint8_t  last_water_level;    // Last received water level
    uint32_t last_update_time;    // Timestamp of last sensor update
    
    // Manual override status
    bool     manual_override;     // Is manual override active
    uint32_t manual_remaining_sec;// Seconds remaining in manual mode
    
    // Signal strength (for range monitoring)
    int8_t   rssi_dbm;            // Last RSSI in dBm (-100 to 0)
    uint8_t  signal_quality;      // Signal quality 0-100%
} device_status_t;

/**
 * Start BLE status advertising (for post-provisioning monitoring)
 * This allows mobile app to connect and read device status
 * @return ESP_OK on success
 */
esp_err_t ble_status_start(void);

/**
 * Stop BLE status advertising
 * @return ESP_OK on success
 */
esp_err_t ble_status_stop(void);

/**
 * Update device status (called by main application)
 * This updates the BLE characteristics for mobile app to read
 * @param status Current device status
 * @return ESP_OK on success
 */
esp_err_t ble_status_update(const device_status_t *status);

/**
 * Check if status monitoring is active
 * @return true if BLE status advertising is running
 */
bool ble_status_is_active(void);

/* ============================================================================
 * MANUAL PUMP CONTROL (Emergency Override)
 * ============================================================================ */

/**
 * Manual pump control command structure
 * Sent from mobile app to controller node via BLE
 */
typedef struct {
    uint8_t  command;           // 0=STOP, 1=START_TIMED
    uint16_t duration_minutes;  // Duration in minutes (0 for stop)
} manual_pump_cmd_t;

// Manual pump command types
#define PUMP_CMD_STOP           0x00
#define PUMP_CMD_START_TIMED    0x01

// Preset durations (in minutes)
#define PUMP_DURATION_10_MIN    10
#define PUMP_DURATION_15_MIN    15
#define PUMP_DURATION_20_MIN    20
#define PUMP_DURATION_30_MIN    30
#define PUMP_DURATION_45_MIN    45
#define PUMP_DURATION_60_MIN    60

/**
 * Callback for manual pump control commands
 * Controller node should register this to receive manual pump commands
 */
typedef void (*manual_pump_cmd_callback_t)(const manual_pump_cmd_t *cmd);

/**
 * Register callback for manual pump commands
 * @param callback Function to call when manual pump command is received
 */
void ble_register_pump_cmd_callback(manual_pump_cmd_callback_t callback);

/**
 * Get current manual override status
 * @param remaining_seconds Output: seconds remaining in manual mode (0 if not active)
 * @return true if manual override is active
 */
bool ble_get_manual_override_status(uint32_t *remaining_seconds);

#endif // BLE_PROVISION_H

