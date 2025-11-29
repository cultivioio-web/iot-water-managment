/*
 * Zigbee Configuration for Water Management System
 * Cluster definitions and network configuration
 * Updated for esp-zigbee-lib v1.6.x API
 */

#ifndef ZIGBEE_CONFIG_H
#define ZIGBEE_CONFIG_H

#include <esp_zigbee_core.h>
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_endpoint.h>
#include <zcl/esp_zigbee_zcl_common.h>

// Custom cluster IDs for water management system
#define WATER_MANAGEMENT_CLUSTER_ID 0xFC00  // Manufacturer specific cluster

// Water management cluster attributes
#define ATTR_WATER_LEVEL_ID 0x0000
#define ATTR_GOVT_WATER_DETECTED_ID 0x0001
#define ATTR_EMERGENCY_PUMP_ACTIVE_ID 0x0002
#define ATTR_SYSTEM_STATUS_ID 0x0003
#define ATTR_SEASON_MODE_ID 0x0004
#define ATTR_EMERGENCY_THRESHOLD_ID 0x0005

// System status bitmask
#define STATUS_NORMAL 0x00
#define STATUS_LOW_WATER 0x01
#define STATUS_EMERGENCY_PUMP 0x02
#define STATUS_GOVT_WATER 0x04
#define STATUS_SYSTEM_ERROR 0x08
#define STATUS_BATTERY_LOW 0x10

// Zigbee network configuration
#define ZIGBEE_COORDINATOR_ADDRESS 0x0000
#define BROADCAST_ADDRESS 0xFFFF

// Message types for inter-node communication
enum ZigbeeMessageType {
  MSG_WATER_LEVEL_UPDATE = 0x01,
  MSG_GOVT_WATER_DETECTED = 0x02,
  MSG_EMERGENCY_PUMP_COMMAND = 0x03,
  MSG_SYSTEM_STATUS_UPDATE = 0x04,
  MSG_SEASON_CHANGE = 0x05,
  MSG_CONFIGURATION_UPDATE = 0x06,
  MSG_HEARTBEAT = 0x07
};

// Node identification within the network
#define PRIMARY_SENSOR_NODE_ID 0x01
#define MAIN_CONTROLLER_NODE_ID 0x02
#define BACKUP_CONTROLLER_NODE_ID 0x03

// Zigbee endpoint configuration for different device types
#define WATER_SENSOR_ENDPOINT_ID 0x01
#define WATER_CONTROLLER_ENDPOINT_ID 0x02
#define WATER_BACKUP_ENDPOINT_ID 0x03
// WATER_SYSTEM_ENDPOINT is defined in config.h

// Custom cluster for water management - returns attribute list for custom cluster
esp_zb_attribute_list_t* create_water_management_attributes();

// Network management functions
void zigbee_send_water_level_update(float water_level);
void zigbee_send_govt_water_alert();
void zigbee_send_emergency_pump_command(bool activate);
void zigbee_send_system_status(uint8_t status);
void zigbee_request_configuration_sync();

// Callback function declarations
void zigbee_attribute_update_callback(uint16_t cluster_id, uint16_t attr_id, void *value, uint16_t value_size);
void zigbee_command_callback(uint16_t cluster_id, uint8_t command_id, esp_zb_device_cb_common_info_t *info);

// Utility functions
uint16_t get_node_address_by_role(uint8_t role);
bool is_node_online(uint16_t address);
void zigbee_network_maintenance();

// Network address management
void zigbee_set_network_address(uint16_t address);
uint16_t zigbee_get_network_address();

#endif // ZIGBEE_CONFIG_H
