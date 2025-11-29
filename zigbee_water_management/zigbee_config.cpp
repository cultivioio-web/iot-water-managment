/*
 * Zigbee Configuration Implementation for Water Management System
 * Cluster creation and network communication functions
 * Updated for esp-zigbee-lib v1.6.x API
 */

#include "zigbee_config.h"
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_attribute.h>
#include <zcl/esp_zigbee_zcl_command.h>
#include "config.h"

// Global variables for Zigbee communication
static uint16_t my_network_address = 0xFFFF;  // Will be set when joined to network
static uint8_t current_system_status = STATUS_NORMAL;

// Static storage for attribute values (must persist)
static uint16_t water_level_value = 0;
static bool govt_water_detected_value = false;
static bool emergency_pump_active_value = false;
static uint8_t season_mode_value = 0;
static uint8_t emergency_threshold_value = 10;

// Create water management cluster attributes using the new API
esp_zb_attribute_list_t* create_water_management_attributes() {
  esp_zb_attribute_list_t *attr_list = esp_zb_zcl_attr_list_create(WATER_MANAGEMENT_CLUSTER_ID);

  if (attr_list == nullptr) {
    return nullptr;
  }

  // Water level attribute (0-100%)
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_WATER_LEVEL_ID,
    ESP_ZB_ZCL_ATTR_TYPE_U16,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
    &water_level_value);

  // Government water detected attribute (boolean)
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_GOVT_WATER_DETECTED_ID,
    ESP_ZB_ZCL_ATTR_TYPE_BOOL,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
    &govt_water_detected_value);

  // Emergency pump active attribute (boolean)
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_EMERGENCY_PUMP_ACTIVE_ID,
    ESP_ZB_ZCL_ATTR_TYPE_BOOL,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
    &emergency_pump_active_value);

  // System status attribute (bitmap)
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_SYSTEM_STATUS_ID,
    ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
    &current_system_status);

  // Season mode attribute
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_SEASON_MODE_ID,
    ESP_ZB_ZCL_ATTR_TYPE_U8,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
    &season_mode_value);

  // Emergency threshold attribute
  esp_zb_custom_cluster_add_custom_attr(attr_list,
    ATTR_EMERGENCY_THRESHOLD_ID,
    ESP_ZB_ZCL_ATTR_TYPE_U8,
    ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
    &emergency_threshold_value);

  return attr_list;
}

// Send water level update to network
void zigbee_send_water_level_update(float water_level) {
  if (my_network_address == 0xFFFF) return;  // Not joined to network

  water_level_value = (uint16_t)(water_level * 100);  // Convert to 0-10000 range

  // Update local attribute
  esp_zb_zcl_set_attribute_val(
    WATER_SYSTEM_ENDPOINT,
    WATER_MANAGEMENT_CLUSTER_ID,
    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
    ATTR_WATER_LEVEL_ID,
    &water_level_value,
    false);

  // Send report command
  esp_zb_zcl_report_attr_cmd_t report_cmd;
  memset(&report_cmd, 0, sizeof(report_cmd));
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = BROADCAST_ADDRESS;
  report_cmd.zcl_basic_cmd.dst_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.zcl_basic_cmd.src_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  report_cmd.clusterID = WATER_MANAGEMENT_CLUSTER_ID;
  report_cmd.attributeID = ATTR_WATER_LEVEL_ID;

  esp_zb_zcl_report_attr_cmd_req(&report_cmd);
}

// Send government water detection alert
void zigbee_send_govt_water_alert() {
  if (my_network_address == 0xFFFF) return;

  govt_water_detected_value = true;

  esp_zb_zcl_set_attribute_val(
    WATER_SYSTEM_ENDPOINT,
    WATER_MANAGEMENT_CLUSTER_ID,
    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
    ATTR_GOVT_WATER_DETECTED_ID,
    &govt_water_detected_value,
    false);

  esp_zb_zcl_report_attr_cmd_t report_cmd;
  memset(&report_cmd, 0, sizeof(report_cmd));
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = BROADCAST_ADDRESS;
  report_cmd.zcl_basic_cmd.dst_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.zcl_basic_cmd.src_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  report_cmd.clusterID = WATER_MANAGEMENT_CLUSTER_ID;
  report_cmd.attributeID = ATTR_GOVT_WATER_DETECTED_ID;

  esp_zb_zcl_report_attr_cmd_req(&report_cmd);
}

// Send emergency pump command
void zigbee_send_emergency_pump_command(bool activate) {
  if (my_network_address == 0xFFFF) return;

  uint16_t controller_addr = get_node_address_by_role(MAIN_CONTROLLER_NODE_ID);
  if (controller_addr == 0xFFFF) return;

  emergency_pump_active_value = activate;

  // Update local attribute
  esp_zb_zcl_set_attribute_val(
    WATER_CONTROLLER_ENDPOINT_ID,
    WATER_MANAGEMENT_CLUSTER_ID,
    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
    ATTR_EMERGENCY_PUMP_ACTIVE_ID,
    &emergency_pump_active_value,
    false);

  // Send custom cluster command
  esp_zb_zcl_custom_cluster_cmd_req_t cmd_req;
  memset(&cmd_req, 0, sizeof(cmd_req));
  cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = controller_addr;
  cmd_req.zcl_basic_cmd.dst_endpoint = WATER_CONTROLLER_ENDPOINT_ID;
  cmd_req.zcl_basic_cmd.src_endpoint = WATER_SYSTEM_ENDPOINT;
  cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  cmd_req.cluster_id = WATER_MANAGEMENT_CLUSTER_ID;
  cmd_req.custom_cmd_id = MSG_EMERGENCY_PUMP_COMMAND;
  cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  cmd_req.data.type = ESP_ZB_ZCL_ATTR_TYPE_BOOL;
  cmd_req.data.size = sizeof(bool);
  cmd_req.data.value = (uint8_t*)&activate;

  esp_zb_zcl_custom_cluster_cmd_req(&cmd_req);
}

// Send system status update
void zigbee_send_system_status(uint8_t status) {
  if (my_network_address == 0xFFFF) return;

  current_system_status = status;

  esp_zb_zcl_set_attribute_val(
    WATER_SYSTEM_ENDPOINT,
    WATER_MANAGEMENT_CLUSTER_ID,
    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
    ATTR_SYSTEM_STATUS_ID,
    &current_system_status,
    false);

  esp_zb_zcl_report_attr_cmd_t report_cmd;
  memset(&report_cmd, 0, sizeof(report_cmd));
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = BROADCAST_ADDRESS;
  report_cmd.zcl_basic_cmd.dst_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.zcl_basic_cmd.src_endpoint = WATER_SYSTEM_ENDPOINT;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  report_cmd.clusterID = WATER_MANAGEMENT_CLUSTER_ID;
  report_cmd.attributeID = ATTR_SYSTEM_STATUS_ID;

  esp_zb_zcl_report_attr_cmd_req(&report_cmd);
}

// Request configuration synchronization from coordinator
void zigbee_request_configuration_sync() {
  if (my_network_address == 0xFFFF) return;
  // Implementation depends on specific message format
}

// Get node address by role
uint16_t get_node_address_by_role(uint8_t role) {
  switch (role) {
    case PRIMARY_SENSOR_NODE_ID: return 0x0001;
    case MAIN_CONTROLLER_NODE_ID: return 0x0002;
    case BACKUP_CONTROLLER_NODE_ID: return 0x0003;
    default: return 0xFFFF;
  }
}

// Check if node is online
bool is_node_online(uint16_t address) {
  return (address >= 0x0001 && address <= 0x0003);
}

// Network maintenance function
void zigbee_network_maintenance() {
  // Check network connectivity, send heartbeats, etc.
}

// Attribute update callback
void zigbee_attribute_update_callback(uint16_t cluster_id, uint16_t attr_id, void *value, uint16_t value_size) {
  if (cluster_id != WATER_MANAGEMENT_CLUSTER_ID) return;

  switch (attr_id) {
    case ATTR_EMERGENCY_PUMP_ACTIVE_ID:
      break;
    case ATTR_SEASON_MODE_ID:
      break;
    case ATTR_EMERGENCY_THRESHOLD_ID:
      break;
    default:
      break;
  }
}

// Command callback for incoming commands
void zigbee_command_callback(uint16_t cluster_id, uint8_t command_id, esp_zb_device_cb_common_info_t *info) {
  // Handle custom commands
}

// Set network address (called when joining network)
void zigbee_set_network_address(uint16_t address) {
  my_network_address = address;
}

// Get current network address
uint16_t zigbee_get_network_address() {
  return my_network_address;
}
