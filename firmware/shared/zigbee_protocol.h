/*
 * Shared Zigbee Protocol Definitions - Phase 1
 * Water Level Measurement System
 * 
 * Communication between Sensor Node and Controller/Actuator Node
 */

#ifndef ZIGBEE_PROTOCOL_H
#define ZIGBEE_PROTOCOL_H

#include <stdint.h>

/* ============================================================================
 * NETWORK CONFIGURATION
 * ============================================================================ */

#define ZIGBEE_PAN_ID               0x1234
#define ZIGBEE_CHANNEL              15
#define ZIGBEE_CHANNEL_MASK         (1l << ZIGBEE_CHANNEL)

/* ============================================================================
 * NODE IDENTIFICATION
 * ============================================================================ */

typedef enum {
    NODE_TYPE_SENSOR = 0x01,        // Sensor node (End Device)
    NODE_TYPE_CONTROLLER = 0x02     // Controller/Actuator (Coordinator)
} NodeType_t;

/* ============================================================================
 * ZIGBEE ENDPOINTS & CLUSTERS
 * ============================================================================ */

#define ENDPOINT_SENSOR             1
#define ENDPOINT_CONTROLLER         1

// Custom cluster for water management
#define CLUSTER_WATER_LEVEL         0xFC01

/* ============================================================================
 * WATER LEVEL CLUSTER ATTRIBUTES
 * ============================================================================ */

#define ATTR_WATER_LEVEL_PERCENT    0x0000  // uint8_t: 0-100%
#define ATTR_WATER_LEVEL_CM         0x0001  // uint16_t: water depth in cm
#define ATTR_SENSOR_STATUS          0x0002  // uint8_t: sensor health
#define ATTR_PUMP_STATE             0x0003  // uint8_t: pump on/off

// Sensor status
#define SENSOR_STATUS_OK            0x00
#define SENSOR_STATUS_ERROR         0x01
#define SENSOR_STATUS_TIMEOUT       0x02

/* ============================================================================
 * COMMANDS
 * ============================================================================ */

// Sensor -> Controller
#define CMD_WATER_LEVEL_REPORT      0x01    // Report water level

// Controller -> Actuator (internal)
#define CMD_PUMP_ON                 0x10
#define CMD_PUMP_OFF                0x11

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================ */

// Water level report packet
typedef struct __attribute__((packed)) {
    uint8_t  water_level_percent;   // 0-100%
    uint16_t water_level_cm;        // cm
    uint8_t  sensor_status;         // status flags
} WaterLevelReport_t;

/* ============================================================================
 * DEFAULT THRESHOLDS
 * ============================================================================ */

#define DEFAULT_TANK_HEIGHT_CM      200     // Tank height
#define DEFAULT_LOW_THRESHOLD       20      // 20% - turn pump ON
#define DEFAULT_HIGH_THRESHOLD      80      // 80% - turn pump OFF
#define DEFAULT_PUMP_TIMEOUT_MIN    60      // 60 minutes max pump runtime

#endif // ZIGBEE_PROTOCOL_H
