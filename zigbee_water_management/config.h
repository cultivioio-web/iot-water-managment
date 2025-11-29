/*
 * Configuration file for 3-Node Zigbee Water Management System
 * Hardware pin definitions for different node roles
 */

// Primary Sensor Node Pins
#define ULTRASONIC_TRIG_PIN 12
#define ULTRASONIC_ECHO_PIN 13
#define GOVT_WATER_SENSOR_PIN 14
#define LED_GREEN_PIN 15
#define LED_YELLOW_PIN 16
#define LED_BLUE_PIN 17

// Government Water Pump Controller Node Pins
#define GOVT_WATER_PUMP_RELAY_PIN 18
#define LED_RED_PIN 19
#define BUZZER_PIN 21

// Emergency/Alternative Water Pump Controller Node Pins
#define EMERGENCY_PUMP_RELAY_PIN 22
#define ALERT_BUTTON_PIN 23
#define BACKUP_LED_RED_PIN 25
#define BACKUP_LED_YELLOW_PIN 26
#define BACKUP_LED_GREEN_PIN 27
#define BACKUP_BUZZER_PIN 32

// Multi-Node Status Display Pins (additional LEDs for showing other nodes' status)
#define NODE2_STATUS_LED 33
#define NODE3_STATUS_LED 34

// Common Configuration
#define SERIAL_BAUD_RATE 115200
#define SETUP_WIFI_SSID "Cultivio-Wifi-Setup"
#define SETUP_WIFI_PASSWORD "cultivio123"

// Default Tank Specifications (configurable via web interface)
#define DEFAULT_TANK_HEIGHT_CM 200
#define DEFAULT_TANK_DIAMETER_CM 150

// Sensor Calibration
#define ULTRASONIC_TIMEOUT_US 30000
#define SOUND_SPEED_CM_PER_US 0.0343

// Timing Constants
#define MEASUREMENT_INTERVAL_MAINS_MS 30000    // 30s on mains power
#define MEASUREMENT_INTERVAL_BATTERY_MS 60000  // 60s on battery power
#define ALERT_ACKNOWLEDGE_TIMEOUT_MS 500       // Button debounce
#define PUMP_SAFETY_TIMEOUT_HOURS 2            // 2 hours max pump runtime

// Zigbee Configuration
#define WATER_SYSTEM_ENDPOINT 1
#define ZIGBEE_CHANNEL_MASK ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK
#define ZIGBEE_PAN_ID 0x1234

// Alert Patterns
#define ALERT_PATTERN_OFF 0            // No alert
#define ALERT_PATTERN_SINGLE_BEEP 1    // Govt water detection
#define ALERT_PATTERN_MEDIUM_BEEP 2    // Low water warning
#define ALERT_PATTERN_FAST_BEEP 3      // Emergency pump active

// LED Status Codes
#define LED_STATUS_NORMAL 0x01         // Green
#define LED_STATUS_WARNING 0x02        // Yellow
#define LED_STATUS_EMERGENCY 0x04      // Red
#define LED_STATUS_GOVT_WATER 0x08     // Blue

// Duty Bitmasks (for dynamic assignment)
#define DUTY_WATER_LEVEL        0x01  // Water level measurement
#define DUTY_GOVT_WATER_DETECT  0x02  // Government water detection
#define DUTY_DECISION_MAKING    0x04  // Control logic and decisions
#define DUTY_GOVT_PUMP_CONTROL  0x08  // Government water pump activation
#define DUTY_ALT_PUMP_CONTROL   0x10  // Alternative water pump activation
#define DUTY_ALERT_SYSTEM       0x20  // LED/buzzer alerts
#define DUTY_STATUS_DISPLAY     0x40  // Multi-node status display

// Network Configuration
#define MAX_NODES 3                   // Maximum nodes in network
#define NETWORK_DISCOVERY_INTERVAL_MS 30000  // 30 seconds
#define NODE_TIMEOUT_MS 120000        // 2 minutes offline timeout
#define HEARTBEAT_INTERVAL_MS 10000   // 10 seconds

// Status Display Patterns (for multi-node status)
#define STATUS_LED_PATTERN_SOLID 0    // Solid LED
#define STATUS_LED_PATTERN_BLINK_SLOW 1  // Slow blink
#define STATUS_LED_PATTERN_BLINK_FAST 2  // Fast blink
#define STATUS_LED_PATTERN_OFF 3      // LED off
