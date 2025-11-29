/*
 * Dynamic Water Management System - ESP32/ESP32-C6/ESP32-H2 Firmware
 *
 * Architecture: Dynamic Duty Assignment (1-3 nodes)
 * Communication: 
 *   - ESP32: WiFi only (HTTP/MQTT)
 *   - ESP32-C6/H2: Zigbee mesh network (50ft range)
 * Operation: Self-sufficient after HTTP setup
 *
 * Build targets:
 *   ESP32:     idf.py set-target esp32 && idf.py build
 *   ESP32-C6:  idf.py set-target esp32c6 && idf.py build
 *   ESP32-H2:  idf.py set-target esp32h2 && idf.py build
 */

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "esp_err.h"

// Conditional Zigbee support
#if ZIGBEE_ENABLED
  #include "zigbee_config.h"
  #include "esp_zigbee_core.h"
  #include "esp_zigbee_cluster.h"
  #include "esp_zigbee_endpoint.h"
  #define COMM_MODE "Zigbee"
#else
  #define COMM_MODE "WiFi"
#endif

// Duty bitmask system
#define DUTY_WATER_LEVEL        0x01
#define DUTY_GOVT_WATER_DETECT  0x02
#define DUTY_DECISION_MAKING    0x04
#define DUTY_GOVT_PUMP_CONTROL  0x08
#define DUTY_ALT_PUMP_CONTROL   0x10
#define DUTY_ALERT_SYSTEM       0x20
#define DUTY_STATUS_DISPLAY     0x40

// Network status structure
struct NodeStatus {
  uint16_t nodeId;
  uint32_t lastSeen;
  uint8_t duties;
  float waterLevel;
  bool govtWaterDetected;
  bool emergencyPumpActive;
  uint8_t systemStatus;
  bool isOnline;
};

struct NetworkStatus {
  uint8_t totalNodes;
  NodeStatus nodes[MAX_NODES];
  uint32_t lastNetworkScan;
};

// System state
enum Season { MONSOON, DRY_SEASON };
enum SystemState { SETUP_MODE, AUTONOMOUS_MODE };
enum NetworkMode { SINGLE_NODE, DUAL_NODE, TRIPLE_NODE };

uint8_t currentDuties = 0;
NetworkMode networkMode = SINGLE_NODE;
Season currentSeason = DRY_SEASON;
SystemState systemState = SETUP_MODE;

// Hardware pins
struct PinConfig {
  int ultrasonicTrig = -1;
  int ultrasonicEcho = -1;
  int govtWaterSensor = -1;
  int govtWaterPumpRelay = -1;
  int emergencyPumpRelay = -1;
  int ledRed = -1;
  int ledYellow = -1;
  int ledGreen = -1;
  int ledBlue = -1;
  int buzzer = -1;
  int alertButton = -1;
  int node2StatusLed = -1;
  int node3StatusLed = -1;
} pins;

// System configuration
struct SystemConfig {
  uint8_t duties;
  Season season;
  float tankHeight;
  float tankDiameter;
  float emergencyThresholdDry;
  float emergencyThresholdMonsoon;
  int govtWaterWaitHoursDry;
  int govtWaterWaitHoursMonsoon;
  bool mainsPower;
} config;

// Global network status
NetworkStatus networkStatus = {};

// Runtime variables
float currentWaterLevel = 0.0;
bool govtWaterDetected = false;
bool govtPumpRunning = false;
bool altPumpRunning = false;
bool alertAcknowledged = true;
bool lowWaterConditionActive = false;
unsigned long lastMeasurement = 0;
unsigned long govtWaterLastDetected = 0;
unsigned long govtPumpStartTime = 0;
unsigned long altPumpStartTime = 0;

// Web server for setup
WebServer server(80);

// Preferences for configuration storage
Preferences preferences;

// Forward declarations
void initializeHardware();
void setupWiFiAndWebServer();
void setupWebInterface();
void handleSetupPost();
void loadConfiguration();
void saveConfiguration();
void enterAutonomousMode();
void performMeasurements();
void executeControlLogic();
void handleAlerts();
void updateSeasonalLogic();
float measureWaterLevel();
bool detectGovtWater();
void controlPump(bool enable);
void controlGovtPump(bool enable);
void controlEmergencyPump(bool enable);
void updateLEDs();
void activateBuzzer(int pattern);
String getSetupPageHTML();

#if ZIGBEE_ENABLED
void setupZigbeeNetwork();
void createZigbeeEndpoint();
esp_err_t zigbee_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message);
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Water Management System ===");
  Serial.printf("Communication Mode: %s\n", COMM_MODE);
  #if ZIGBEE_ENABLED
  Serial.println("Target: ESP32-C6/H2 with Zigbee support");
  #else
  Serial.println("Target: ESP32 WiFi-only mode");
  #endif

  preferences.begin("water_system", false);
  loadConfiguration();
  initializeHardware();

  if (systemState == SETUP_MODE) {
    setupWiFiAndWebServer();
    Serial.println("System in SETUP MODE - Access web interface at http://<IP>/setup");
  } else {
    #if ZIGBEE_ENABLED
    setupZigbeeNetwork();
    Serial.println("System in AUTONOMOUS MODE - Zigbee mesh active");
    #else
    // WiFi mode - keep server running for monitoring
    setupWiFiAndWebServer();
    Serial.println("System in AUTONOMOUS MODE - WiFi monitoring active");
    #endif
  }

  Serial.printf("Node Duties: 0x%02X | Season: %s\n",
                currentDuties,
                currentSeason == MONSOON ? "MONSOON" : "DRY_SEASON");
}

void loop() {
  if (systemState == SETUP_MODE) {
    server.handleClient();

    if (pins.ledBlue != -1) {
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 500) {
        bool state = digitalRead(pins.ledBlue);
        digitalWrite(pins.ledBlue, !state);
        lastBlink = millis();
      }
    }
  } else {
    #if !ZIGBEE_ENABLED
    // WiFi mode: keep handling web requests
    server.handleClient();
    #endif

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {
      performMeasurements();
      executeControlLogic();
      handleAlerts();
      updateSeasonalLogic();
      lastUpdate = millis();
    }

    if (pins.alertButton != -1 && digitalRead(pins.alertButton) == LOW) {
      alertAcknowledged = true;
      Serial.println("Alert acknowledged by user");
      delay(200);
    }
  }
}

void initializeHardware() {
  if (currentDuties & DUTY_WATER_LEVEL) {
    pins.ultrasonicTrig = ULTRASONIC_TRIG_PIN;
    pins.ultrasonicEcho = ULTRASONIC_ECHO_PIN;
  }

  if (currentDuties & DUTY_GOVT_WATER_DETECT) {
    pins.govtWaterSensor = GOVT_WATER_SENSOR_PIN;
  }

  if (currentDuties & DUTY_GOVT_PUMP_CONTROL) {
    pins.govtWaterPumpRelay = GOVT_WATER_PUMP_RELAY_PIN;
  }

  if (currentDuties & DUTY_ALT_PUMP_CONTROL) {
    pins.emergencyPumpRelay = EMERGENCY_PUMP_RELAY_PIN;
  }

  if (currentDuties & DUTY_ALERT_SYSTEM) {
    pins.ledGreen = LED_GREEN_PIN;
    pins.ledYellow = LED_YELLOW_PIN;
    pins.ledRed = LED_RED_PIN;
    pins.buzzer = BUZZER_PIN;
    pins.alertButton = ALERT_BUTTON_PIN;
  }

  if (currentDuties & DUTY_STATUS_DISPLAY) {
    pins.ledBlue = LED_BLUE_PIN;
    pins.node2StatusLed = NODE2_STATUS_LED;
    pins.node3StatusLed = NODE3_STATUS_LED;
  }

  // Set pin modes
  if (pins.ultrasonicTrig != -1) pinMode(pins.ultrasonicTrig, OUTPUT);
  if (pins.ultrasonicEcho != -1) pinMode(pins.ultrasonicEcho, INPUT);
  if (pins.govtWaterSensor != -1) pinMode(pins.govtWaterSensor, INPUT);
  if (pins.govtWaterPumpRelay != -1) pinMode(pins.govtWaterPumpRelay, OUTPUT);
  if (pins.emergencyPumpRelay != -1) pinMode(pins.emergencyPumpRelay, OUTPUT);
  if (pins.ledRed != -1) pinMode(pins.ledRed, OUTPUT);
  if (pins.ledYellow != -1) pinMode(pins.ledYellow, OUTPUT);
  if (pins.ledGreen != -1) pinMode(pins.ledGreen, OUTPUT);
  if (pins.ledBlue != -1) pinMode(pins.ledBlue, OUTPUT);
  if (pins.buzzer != -1) pinMode(pins.buzzer, OUTPUT);
  if (pins.alertButton != -1) pinMode(pins.alertButton, INPUT_PULLUP);
  if (pins.node2StatusLed != -1) pinMode(pins.node2StatusLed, OUTPUT);
  if (pins.node3StatusLed != -1) pinMode(pins.node3StatusLed, OUTPUT);

  // Initialize outputs to safe state
  if (pins.govtWaterPumpRelay != -1) digitalWrite(pins.govtWaterPumpRelay, LOW);
  if (pins.emergencyPumpRelay != -1) digitalWrite(pins.emergencyPumpRelay, LOW);
  if (pins.buzzer != -1) digitalWrite(pins.buzzer, LOW);
  updateLEDs();
}

void setupWiFiAndWebServer() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("WaterSystem-Setup", "watersystem123");
  Serial.println("WiFi AP started: WaterSystem-Setup");

  IPAddress IP = WiFi.softAPIP();
  Serial.printf("Setup AP IP: %s\n", IP.toString().c_str());

  setupWebInterface();
  server.begin();
  Serial.println("Web server started on port 80");
}

void setupWebInterface() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getSetupPageHTML());
  });

  server.on("/setup", HTTP_GET, []() {
    server.send(200, "text/html", getSetupPageHTML());
  });

  server.on("/configure", HTTP_POST, handleSetupPost);

  server.on("/api/config", HTTP_GET, []() {
    JsonDocument doc;
    doc["duties"] = currentDuties;
    doc["season"] = currentSeason;
    doc["tankHeight"] = config.tankHeight;
    doc["tankDiameter"] = config.tankDiameter;
    doc["emergencyThresholdDry"] = config.emergencyThresholdDry;
    doc["emergencyThresholdMonsoon"] = config.emergencyThresholdMonsoon;
    doc["govtWaterWaitHoursDry"] = config.govtWaterWaitHoursDry;
    doc["govtWaterWaitHoursMonsoon"] = config.govtWaterWaitHoursMonsoon;
    doc["mainsPower"] = config.mainsPower;
    doc["commMode"] = COMM_MODE;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // Status API for monitoring
  server.on("/api/status", HTTP_GET, []() {
    JsonDocument doc;
    doc["waterLevel"] = currentWaterLevel;
    doc["govtWaterDetected"] = govtWaterDetected;
    doc["govtPumpRunning"] = govtPumpRunning;
    doc["altPumpRunning"] = altPumpRunning;
    doc["lowWater"] = lowWaterConditionActive;
    doc["season"] = currentSeason == MONSOON ? "monsoon" : "dry";
    doc["mode"] = systemState == SETUP_MODE ? "setup" : "autonomous";
    doc["commMode"] = COMM_MODE;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
}

String getSetupPageHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Water Management System Setup</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; }
        .form-group { margin: 15px 0; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        select, input[type="number"] { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
        .checkbox-group label { font-weight: normal; display: block; margin: 5px 0; }
        button { background: #007bff; color: white; padding: 12px 24px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }
        button:hover { background: #0056b3; }
        .status { margin-top: 20px; padding: 15px; border-radius: 4px; }
        .success { background: #d4edda; color: #155724; }
        .error { background: #f8d7da; color: #721c24; }
        .info { background: #e7f3ff; color: #004085; padding: 10px; border-radius: 4px; margin-bottom: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Water Management Setup</h1>
        <div class="info" id="commMode">Loading...</div>
        
        <form id="setupForm">
            <div class="form-group">
                <label>Node Duties:</label>
                <div class="checkbox-group">
                    <label><input type="checkbox" name="duties" value="1"> Water Level Measurement</label>
                    <label><input type="checkbox" name="duties" value="2"> Government Water Detection</label>
                    <label><input type="checkbox" name="duties" value="4"> Decision Making</label>
                    <label><input type="checkbox" name="duties" value="8"> Government Pump Control</label>
                    <label><input type="checkbox" name="duties" value="16"> Emergency Pump Control</label>
                    <label><input type="checkbox" name="duties" value="32"> Alert System</label>
                    <label><input type="checkbox" name="duties" value="64"> Status Display</label>
                </div>
            </div>

            <div class="form-group">
                <label for="season">Current Season:</label>
                <select id="season" name="season">
                    <option value="1">Dry Season</option>
                    <option value="0">Monsoon Season</option>
                </select>
            </div>

            <div class="form-group">
                <label for="tankHeight">Tank Height (cm):</label>
                <input type="number" id="tankHeight" name="tankHeight" value="200">
            </div>

            <div class="form-group">
                <label for="tankDiameter">Tank Diameter (cm):</label>
                <input type="number" id="tankDiameter" name="tankDiameter" value="150">
            </div>

            <div class="form-group">
                <label for="emergencyThresholdDry">Emergency Threshold - Dry (%):</label>
                <input type="number" id="emergencyThresholdDry" name="emergencyThresholdDry" value="10" step="0.5">
            </div>

            <div class="form-group">
                <label for="emergencyThresholdMonsoon">Emergency Threshold - Monsoon (%):</label>
                <input type="number" id="emergencyThresholdMonsoon" name="emergencyThresholdMonsoon" value="15" step="0.5">
            </div>

            <div class="form-group">
                <label for="govtWaterWaitDry">Govt Water Wait - Dry (hours):</label>
                <input type="number" id="govtWaterWaitDry" name="govtWaterWaitDry" value="2">
            </div>

            <div class="form-group">
                <label for="govtWaterWaitMonsoon">Govt Water Wait - Monsoon (hours):</label>
                <input type="number" id="govtWaterWaitMonsoon" name="govtWaterWaitMonsoon" value="4">
            </div>

            <div class="form-group">
                <label for="mainsPower">Power Source:</label>
                <select id="mainsPower" name="mainsPower">
                    <option value="true">Mains Power</option>
                    <option value="false">Battery Power</option>
                </select>
            </div>

            <button type="submit">Save & Start System</button>
        </form>

        <div id="status" class="status" style="display:none;"></div>
    </div>

    <script>
        fetch('/api/config').then(r => r.json()).then(cfg => {
            document.getElementById('commMode').innerHTML = 'Communication: <strong>' + cfg.commMode + '</strong>';
            document.getElementById('season').value = cfg.season;
            document.getElementById('tankHeight').value = cfg.tankHeight;
            document.getElementById('tankDiameter').value = cfg.tankDiameter;
            document.getElementById('emergencyThresholdDry').value = cfg.emergencyThresholdDry;
            document.getElementById('emergencyThresholdMonsoon').value = cfg.emergencyThresholdMonsoon;
            document.getElementById('govtWaterWaitDry').value = cfg.govtWaterWaitHoursDry;
            document.getElementById('govtWaterWaitMonsoon').value = cfg.govtWaterWaitHoursMonsoon;
            document.getElementById('mainsPower').value = cfg.mainsPower.toString();
            
            document.querySelectorAll('input[name="duties"]').forEach(cb => {
                cb.checked = (cfg.duties & parseInt(cb.value)) !== 0;
            });
        });

        document.getElementById('setupForm').addEventListener('submit', function(e) {
            e.preventDefault();
            let duties = 0;
            document.querySelectorAll('input[name="duties"]:checked').forEach(cb => {
                duties |= parseInt(cb.value);
            });

            const params = new URLSearchParams();
            params.append('duties', duties);
            params.append('season', document.getElementById('season').value);
            params.append('tankHeight', document.getElementById('tankHeight').value);
            params.append('tankDiameter', document.getElementById('tankDiameter').value);
            params.append('emergencyThresholdDry', document.getElementById('emergencyThresholdDry').value);
            params.append('emergencyThresholdMonsoon', document.getElementById('emergencyThresholdMonsoon').value);
            params.append('govtWaterWaitDry', document.getElementById('govtWaterWaitDry').value);
            params.append('govtWaterWaitMonsoon', document.getElementById('govtWaterWaitMonsoon').value);
            params.append('mainsPower', document.getElementById('mainsPower').value);

            fetch('/configure', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: params.toString()
            }).then(r => r.json()).then(result => {
                const s = document.getElementById('status');
                s.className = 'status success';
                s.textContent = 'Configuration saved! Restarting...';
                s.style.display = 'block';
            }).catch(err => {
                const s = document.getElementById('status');
                s.className = 'status error';
                s.textContent = 'Error: ' + err.message;
                s.style.display = 'block';
            });
        });
    </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleSetupPost() {
  uint8_t selectedDuties = 0;

  if (server.hasArg("duties")) {
    selectedDuties = server.arg("duties").toInt();
  }

  config.duties = selectedDuties;
  config.season = (Season)server.arg("season").toInt();
  config.tankHeight = server.arg("tankHeight").toFloat();
  config.tankDiameter = server.arg("tankDiameter").toFloat();
  config.emergencyThresholdDry = server.arg("emergencyThresholdDry").toFloat();
  config.emergencyThresholdMonsoon = server.arg("emergencyThresholdMonsoon").toFloat();
  config.govtWaterWaitHoursDry = server.arg("govtWaterWaitDry").toInt();
  config.govtWaterWaitHoursMonsoon = server.arg("govtWaterWaitMonsoon").toInt();
  config.mainsPower = (server.arg("mainsPower") == "true");

  saveConfiguration();

  server.send(200, "application/json", "{\"success\":true}");

  delay(1000);
  ESP.restart();
}

void loadConfiguration() {
  config.duties = preferences.getInt("duties", DUTY_WATER_LEVEL | DUTY_DECISION_MAKING | DUTY_ALERT_SYSTEM);
  config.season = (Season)preferences.getInt("season", DRY_SEASON);
  config.tankHeight = preferences.getFloat("tankHeight", 200.0);
  config.tankDiameter = preferences.getFloat("tankDiameter", 150.0);
  config.emergencyThresholdDry = preferences.getFloat("emergThDry", 10.0);
  config.emergencyThresholdMonsoon = preferences.getFloat("emergThMon", 15.0);
  config.govtWaterWaitHoursDry = preferences.getInt("govtWaitDry", 2);
  config.govtWaterWaitHoursMonsoon = preferences.getInt("govtWaitMon", 4);
  config.mainsPower = preferences.getBool("mainsPower", true);

  currentDuties = config.duties;
  currentSeason = config.season;

  if (!preferences.getBool("configured", false)) {
    systemState = SETUP_MODE;
  } else {
    systemState = AUTONOMOUS_MODE;
  }
}

void saveConfiguration() {
  preferences.putInt("duties", config.duties);
  preferences.putInt("season", config.season);
  preferences.putFloat("tankHeight", config.tankHeight);
  preferences.putFloat("tankDiameter", config.tankDiameter);
  preferences.putFloat("emergThDry", config.emergencyThresholdDry);
  preferences.putFloat("emergThMon", config.emergencyThresholdMonsoon);
  preferences.putInt("govtWaitDry", config.govtWaterWaitHoursDry);
  preferences.putInt("govtWaitMon", config.govtWaterWaitHoursMonsoon);
  preferences.putBool("mainsPower", config.mainsPower);
  preferences.putBool("configured", true);
}

void enterAutonomousMode() {
  systemState = AUTONOMOUS_MODE;
  #if ZIGBEE_ENABLED
  WiFi.mode(WIFI_OFF);
  setupZigbeeNetwork();
  #endif
}

#if ZIGBEE_ENABLED
void setupZigbeeNetwork() {
  esp_zb_cfg_t zb_cfg = {};
  zb_cfg.esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER;
  zb_cfg.install_code_policy = false;

  esp_zb_init(&zb_cfg);
  createZigbeeEndpoint();
  esp_zb_core_action_handler_register(zigbee_action_handler);
  esp_zb_start(false);

  Serial.println("Zigbee network initialized");
}

void createZigbeeEndpoint() {
  esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
  if (!cluster_list) {
    Serial.println("Failed to create Zigbee cluster list");
    return;
  }

  esp_zb_attribute_list_t *water_attrs = create_water_management_attributes();
  if (water_attrs) {
    esp_zb_cluster_list_add_custom_cluster(cluster_list, water_attrs, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  }

  esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
  if (!ep_list) {
    Serial.println("Failed to create Zigbee endpoint list");
    return;
  }

  esp_zb_endpoint_config_t ep_config = {};
  ep_config.endpoint = WATER_SYSTEM_ENDPOINT;
  ep_config.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  ep_config.app_device_id = ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID;

  esp_zb_ep_list_add_ep(ep_list, cluster_list, ep_config);
  esp_zb_device_register(ep_list);
}

esp_err_t zigbee_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
      Serial.println("Zigbee attribute value set");
      break;
    default:
      break;
  }
  return ESP_OK;
}

extern "C" void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct) {
  uint32_t *p_sg_p = signal_struct->p_app_signal;
  esp_err_t err_status = signal_struct->esp_err_status;
  esp_zb_app_signal_type_t sig_type = (esp_zb_app_signal_type_t)*p_sg_p;

  switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
      Serial.println("Zigbee stack initialized");
      esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
      break;

    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
      if (err_status == ESP_OK) {
        Serial.println("Starting network steering...");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
      }
      break;

    case ESP_ZB_BDB_SIGNAL_STEERING:
      if (err_status == ESP_OK) {
        Serial.println("Joined Zigbee network successfully");
      } else {
        Serial.println("Network steering failed, retrying...");
        esp_zb_scheduler_alarm((esp_zb_callback_t)esp_zb_bdb_start_top_level_commissioning, 
                               ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
      }
      break;

    default:
      break;
  }
}
#endif // ZIGBEE_ENABLED

void performMeasurements() {
  static unsigned long lastMeasurementTime = 0;
  unsigned long currentTime = millis();
  int measurementInterval = (config.mainsPower ? 30000 : 60000);

  if (currentTime - lastMeasurementTime >= (unsigned long)measurementInterval) {
    lastMeasurementTime = currentTime;

    if (currentDuties & DUTY_WATER_LEVEL) {
      currentWaterLevel = measureWaterLevel();
    }

    if (currentDuties & DUTY_GOVT_WATER_DETECT) {
      govtWaterDetected = detectGovtWater();
      if (govtWaterDetected) {
        govtWaterLastDetected = currentTime;
      }
    }

    Serial.printf("Water: %.1f%% | Govt: %s\n", currentWaterLevel, govtWaterDetected ? "YES" : "NO");
  }
}

void executeControlLogic() {
  if (!(currentDuties & DUTY_DECISION_MAKING)) return;

  float threshold = (currentSeason == MONSOON) ? config.emergencyThresholdMonsoon : config.emergencyThresholdDry;
  int govtWaitHours = (currentSeason == MONSOON) ? config.govtWaterWaitHoursMonsoon : config.govtWaterWaitHoursDry;
  bool belowThreshold = currentWaterLevel < threshold;

  if (belowThreshold && !lowWaterConditionActive) {
    lowWaterConditionActive = true;
    alertAcknowledged = false;
  } else if (!belowThreshold && lowWaterConditionActive) {
    lowWaterConditionActive = false;
    alertAcknowledged = true;
  }

  if (belowThreshold) {
    if (govtWaterDetected && (currentDuties & DUTY_GOVT_PUMP_CONTROL)) {
      controlGovtPump(true);
      if (altPumpRunning) controlEmergencyPump(false);
    } else if (!govtWaterDetected && (currentDuties & DUTY_ALT_PUMP_CONTROL)) {
      unsigned long waitTimeMs = govtWaitHours * 3600000UL;
      if (millis() - govtWaterLastDetected > waitTimeMs && !altPumpRunning) {
        controlEmergencyPump(true);
        if (govtPumpRunning) controlGovtPump(false);
      }
    }
  }

  if (govtPumpRunning && currentWaterLevel >= 90.0f) {
    controlGovtPump(false);
  }
  if (altPumpRunning && currentWaterLevel >= 60.0f) {
    controlEmergencyPump(false);
  }

  // Safety timeout
  unsigned long safetyMs = PUMP_SAFETY_TIMEOUT_HOURS * 3600000UL;
  if (govtPumpRunning && millis() - govtPumpStartTime > safetyMs) {
    controlGovtPump(false);
  }
  if (altPumpRunning && millis() - altPumpStartTime > safetyMs) {
    controlEmergencyPump(false);
  }
}

void handleAlerts() {
  updateLEDs();

  float threshold = (currentSeason == MONSOON) ? config.emergencyThresholdMonsoon : config.emergencyThresholdDry;
  int pattern = ALERT_PATTERN_OFF;

  if (currentWaterLevel < threshold && !alertAcknowledged) {
    pattern = altPumpRunning ? ALERT_PATTERN_FAST_BEEP : ALERT_PATTERN_MEDIUM_BEEP;
  } else if (govtWaterDetected) {
    pattern = ALERT_PATTERN_SINGLE_BEEP;
  }

  activateBuzzer(pattern);
}

void updateLEDs() {
  auto setLed = [](int pin, int val) { if (pin != -1) digitalWrite(pin, val); };

  setLed(pins.ledGreen, LOW);
  setLed(pins.ledYellow, LOW);
  setLed(pins.ledRed, LOW);
  setLed(pins.ledBlue, LOW);

  float threshold = (currentSeason == MONSOON) ? config.emergencyThresholdMonsoon : config.emergencyThresholdDry;

  if (govtWaterDetected) {
    setLed(pins.ledBlue, HIGH);
  } else if (altPumpRunning) {
    setLed(pins.ledRed, HIGH);
  } else if (currentWaterLevel < threshold) {
    setLed(pins.ledYellow, HIGH);
  } else {
    setLed(pins.ledGreen, HIGH);
  }
}

void activateBuzzer(int pattern) {
  if (pins.buzzer == -1) return;

  static int activePattern = ALERT_PATTERN_OFF;
  static bool buzzerState = false;
  static unsigned long lastChange = 0;

  if (pattern == ALERT_PATTERN_OFF) {
    digitalWrite(pins.buzzer, LOW);
    activePattern = ALERT_PATTERN_OFF;
    return;
  }

  unsigned long onMs = 200, offMs = 800;
  if (pattern == ALERT_PATTERN_FAST_BEEP) { onMs = 150; offMs = 150; }
  else if (pattern == ALERT_PATTERN_SINGLE_BEEP) { onMs = 200; offMs = 1800; }

  if (pattern != activePattern) {
    activePattern = pattern;
    buzzerState = false;
    lastChange = millis();
  }

  unsigned long interval = buzzerState ? onMs : offMs;
  if (millis() - lastChange >= interval) {
    buzzerState = !buzzerState;
    digitalWrite(pins.buzzer, buzzerState ? HIGH : LOW);
    lastChange = millis();
  }
}

float measureWaterLevel() {
  if (pins.ultrasonicTrig == -1 || pins.ultrasonicEcho == -1) {
    return currentWaterLevel;
  }

  digitalWrite(pins.ultrasonicTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pins.ultrasonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pins.ultrasonicTrig, LOW);

  long duration = pulseIn(pins.ultrasonicEcho, HIGH, 30000);
  if (duration == 0) return currentWaterLevel;

  float distance = duration * 0.0343 / 2;
  float waterDepth = config.tankHeight - distance;
  waterDepth = constrain(waterDepth, 0, config.tankHeight);

  return (waterDepth / config.tankHeight) * 100.0;
}

bool detectGovtWater() {
  if (pins.govtWaterSensor == -1) return false;
  return digitalRead(pins.govtWaterSensor) == HIGH;
}

void controlPump(bool enable) {
  controlGovtPump(enable);
}

void controlGovtPump(bool enable) {
  if (pins.govtWaterPumpRelay == -1) return;

  if (enable && !govtPumpRunning) {
    digitalWrite(pins.govtWaterPumpRelay, HIGH);
    govtPumpRunning = true;
    govtPumpStartTime = millis();
    Serial.println("Govt pump ON");
  } else if (!enable && govtPumpRunning) {
    digitalWrite(pins.govtWaterPumpRelay, LOW);
    govtPumpRunning = false;
    Serial.println("Govt pump OFF");
  }
}

void controlEmergencyPump(bool enable) {
  if (pins.emergencyPumpRelay == -1) return;

  if (enable && !altPumpRunning) {
    digitalWrite(pins.emergencyPumpRelay, HIGH);
    altPumpRunning = true;
    altPumpStartTime = millis();
    Serial.println("Emergency pump ON");
  } else if (!enable && altPumpRunning) {
    digitalWrite(pins.emergencyPumpRelay, LOW);
    altPumpRunning = false;
    Serial.println("Emergency pump OFF");
  }
}

void updateSeasonalLogic() {
  // Future: automatic season detection
}
