/*
 * Dynamic Zigbee Water Management System - ESP32 Firmware
 *
 * Architecture: Dynamic Duty Assignment (1-3 nodes)
 * Communication: Zigbee mesh network (50ft range)
 * Operation: Self-sufficient after HTTP setup
 * Features: Network-aware, status sharing, seamless failover
 *
 * Duty System (configurable bitmask):
 * - DUTY_WATER_LEVEL: Water level measurement
 * - DUTY_GOVT_WATER: Government water detection
 * - DUTY_DECISION_MAKING: Control logic and decisions
 * - DUTY_PUMP_CONTROL: Emergency pump activation
 * - DUTY_ALERT_SYSTEM: LED/buzzer alerts
 * - DUTY_STATUS_DISPLAY: Multi-node status display
 *
 * Network Modes:
 * - SINGLE_NODE: 1 node performs all duties
 * - DUAL_NODE: 2 nodes coordinate duties
 * - TRIPLE_NODE: 3 nodes with full redundancy
 */

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "zigbee_config.h"
#include "esp_err.h"

// Zigbee includes (using ESP-Zigbee-SDK)
#include "esp_zigbee_core.h"
#include "esp_zigbee_cluster.h"
#include "esp_zigbee_endpoint.h"

// Duty bitmask system
#define DUTY_WATER_LEVEL        0x01  // Water level measurement
#define DUTY_GOVT_WATER_DETECT  0x02  // Government water detection
#define DUTY_DECISION_MAKING    0x04  // Control logic and decisions
#define DUTY_GOVT_PUMP_CONTROL  0x08  // Government water pump activation
#define DUTY_ALT_PUMP_CONTROL   0x10  // Alternative water pump activation
#define DUTY_ALERT_SYSTEM       0x20  // LED/buzzer alerts
#define DUTY_STATUS_DISPLAY     0x40  // Multi-node status display

// Network status structure
struct NodeStatus {
  uint16_t nodeId;
  uint32_t lastSeen;
  uint8_t duties;           // Bitmask of assigned duties
  float waterLevel;         // Current water level reading
  bool govtWaterDetected;   // Government water status
  bool emergencyPumpActive; // Pump status
  uint8_t systemStatus;     // Overall status
  bool isOnline;           // Node connectivity status
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

uint8_t currentDuties = 0;  // Bitmask of assigned duties
NetworkMode networkMode = SINGLE_NODE;
Season currentSeason = DRY_SEASON;
SystemState systemState = SETUP_MODE;

// Hardware pins (configurable based on duties)
struct PinConfig {
  int ultrasonicTrig = -1;
  int ultrasonicEcho = -1;
  int govtWaterSensor = -1;
  int govtWaterPumpRelay = -1;     // Government water supply pump
  int emergencyPumpRelay = -1;     // Emergency/alternative water pump
  int ledRed = -1;
  int ledYellow = -1;
  int ledGreen = -1;
  int ledBlue = -1;
  int buzzer = -1;
  int alertButton = -1;
  int node2StatusLed = -1;         // Shows Node 2 status
  int node3StatusLed = -1;         // Shows Node 3 status
} pins;

// System configuration (stored in NVS)
struct SystemConfig {
  uint8_t duties;                  // Bitmask of assigned duties
  Season season;
  float tankHeight;
  float tankDiameter;
  float emergencyThresholdDry;     // 10%
  float emergencyThresholdMonsoon; // 15%
  int govtWaterWaitHoursDry;       // 1-2 hours
  int govtWaterWaitHoursMonsoon;   // up to 4 hours
  bool mainsPower;                 // true = mains, false = battery
} config;

// Global network status
NetworkStatus networkStatus = {0};

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

// Zigbee variables
esp_zb_ep_list_t *zigbeeEndpointList = nullptr;

// Web server for setup (HTTP)
WebServer server(80);

// Preferences for configuration storage
Preferences preferences;

// Function prototypes
void setupZigbeeNetwork();
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
void updateLEDs();
void activateBuzzer(int pattern);
esp_err_t zigbee_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message);

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== 3-Node Zigbee Water Management System ===");

  // Initialize preferences
  preferences.begin("water_system", false);

  // Load saved configuration
  loadConfiguration();

  // Initialize hardware based on role
  initializeHardware();

  if (systemState == SETUP_MODE) {
    // Setup mode: HTTP configuration interface
    setupWiFiAndWebServer();
    Serial.println("System in SETUP MODE - Access web interface at http://<IP>/setup");
  } else {
    // Autonomous mode: Zigbee mesh operation
    setupZigbeeNetwork();
    Serial.println("System in AUTONOMOUS MODE - Zigbee mesh active");
  }

  Serial.printf("Node Duties: 0x%02X | Season: %s\n",
                currentDuties,
                currentSeason == MONSOON ? "MONSOON" : "DRY_SEASON");
}

void loop() {
  if (systemState == SETUP_MODE) {
    // Handle web server requests during setup
    server.handleClient();

    // Blink LED to indicate setup mode
    if (pins.ledBlue != -1) {
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 500) {
        bool state = digitalRead(pins.ledBlue);
        digitalWrite(pins.ledBlue, !state);
        lastBlink = millis();
      }
    }
  } else {
    // Autonomous Zigbee operation
    static unsigned long lastUpdate = 0;

    // Periodic system updates
    if (millis() - lastUpdate > 1000) {  // 1 second intervals
      performMeasurements();
      executeControlLogic();
      handleAlerts();
      updateSeasonalLogic();
      lastUpdate = millis();
    }

    // Check for alert acknowledgment button
    if (pins.alertButton != -1 && digitalRead(pins.alertButton) == LOW) {
      alertAcknowledged = true;
      Serial.println("Alert acknowledged by user");
      delay(200);  // Debounce
    }
  }
}

void initializeHardware() {
  // Initialize pins based on assigned duties (bitmask)

  // Always available pins for basic operation
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
  updateLEDs();  // All LEDs off initially
}

void setupWiFiAndWebServer() {
  // Start WiFi in AP+STA mode for setup
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("WaterSystem-Setup", "watersystem123");
  Serial.println("WiFi AP started: WaterSystem-Setup");

  IPAddress IP = WiFi.softAPIP();
  Serial.printf("Setup AP IP: %s\n", IP.toString());

  // Setup web routes
  setupWebInterface();
  server.begin();
  Serial.println("Web server started on port 80");
}

void setupWebInterface() {
  // Main setup page
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getSetupPageHTML());
  });

  server.on("/setup", HTTP_GET, []() {
    server.send(200, "text/html", getSetupPageHTML());
  });

  // Handle configuration POST
  server.on("/configure", HTTP_POST, handleSetupPost);

  // API endpoint for configuration
  server.on("/api/config", HTTP_GET, []() {
    StaticJsonDocument<512> doc;
    doc["duties"] = currentDuties;
    doc["season"] = currentSeason;
    doc["tankHeight"] = config.tankHeight;
    doc["tankDiameter"] = config.tankDiameter;
    doc["emergencyThresholdDry"] = config.emergencyThresholdDry;
    doc["emergencyThresholdMonsoon"] = config.emergencyThresholdMonsoon;
    doc["govtWaterWaitHoursDry"] = config.govtWaterWaitHoursDry;
    doc["govtWaterWaitHoursMonsoon"] = config.govtWaterWaitHoursMonsoon;
    doc["mainsPower"] = config.mainsPower;

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
        .form-group { margin: 15px 0; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        select, input { width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px; }
        button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }
        button:hover { background: #0056b3; }
        .status { margin-top: 20px; padding: 10px; border-radius: 4px; }
        .success { background: #d4edda; color: #155724; }
        .error { background: #f8d7da; color: #721c24; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Water Management System Setup</h1>
        <form id="setupForm">
            <div class="form-group">
                <label>Node Duties (select all that apply):</label>
                <div class="checkbox-group">
                    <label><input type="checkbox" name="duties" value="1"> Water Level Measurement</label><br>
                    <label><input type="checkbox" name="duties" value="2"> Government Water Detection</label><br>
                    <label><input type="checkbox" name="duties" value="4"> Decision Making</label><br>
                    <label><input type="checkbox" name="duties" value="8"> Government Water Pump Control</label><br>
                    <label><input type="checkbox" name="duties" value="16"> Emergency Pump Control</label><br>
                    <label><input type="checkbox" name="duties" value="32"> Alert System</label><br>
                    <label><input type="checkbox" name="duties" value="64"> Status Display</label>
                </div>
            </div>

            <div class="form-group">
                <label for="season">Current Season:</label>
                <select id="season" name="season">
                    <option value="0">Dry Season</option>
                    <option value="1">Monsoon Season</option>
                </select>
            </div>

            <div class="form-group">
                <label for="tankHeight">Tank Height (cm):</label>
                <input type="number" id="tankHeight" name="tankHeight" value="200" step="1">
            </div>

            <div class="form-group">
                <label for="tankDiameter">Tank Diameter (cm):</label>
                <input type="number" id="tankDiameter" name="tankDiameter" value="150" step="1">
            </div>

            <div class="form-group">
                <label for="emergencyThresholdDry">Emergency Threshold Dry Season (%):</label>
                <input type="number" id="emergencyThresholdDry" name="emergencyThresholdDry" value="10" step="0.5">
            </div>

            <div class="form-group">
                <label for="emergencyThresholdMonsoon">Emergency Threshold Monsoon (%):</label>
                <input type="number" id="emergencyThresholdMonsoon" name="emergencyThresholdMonsoon" value="15" step="0.5">
            </div>

            <div class="form-group">
                <label for="govtWaterWaitDry">Govt Water Wait Time Dry Season (hours):</label>
                <input type="number" id="govtWaterWaitDry" name="govtWaterWaitDry" value="2" step="0.5">
            </div>

            <div class="form-group">
                <label for="govtWaterWaitMonsoon">Govt Water Wait Time Monsoon (hours):</label>
                <input type="number" id="govtWaterWaitMonsoon" name="govtWaterWaitMonsoon" value="4" step="0.5">
            </div>

            <div class="form-group">
                <label for="powerSource">Power Source:</label>
                <select id="powerSource" name="mainsPower">
                    <option value="true">Mains Power</option>
                    <option value="false">Battery Power</option>
                </select>
            </div>

            <button type="submit">Save Configuration & Start System</button>
        </form>

        <div id="status" class="status" style="display:none;"></div>
    </div>

    <script>
        // Load current configuration
        fetch('/api/config').then(r => r.json()).then(config => {
            document.getElementById('season').value = config.season;
            document.getElementById('tankHeight').value = config.tankHeight;
            document.getElementById('tankDiameter').value = config.tankDiameter;
            document.getElementById('emergencyThresholdDry').value = config.emergencyThresholdDry;
            document.getElementById('emergencyThresholdMonsoon').value = config.emergencyThresholdMonsoon;
            document.getElementById('govtWaterWaitDry').value = config.govtWaterWaitHoursDry;
            document.getElementById('govtWaterWaitMonsoon').value = config.govtWaterWaitHoursMonsoon;
            document.getElementById('powerSource').value = config.mainsPower.toString();

            // Set duty checkboxes
            const duties = config.duties || 0;
            const checkboxes = document.querySelectorAll('input[name="duties"]');
            checkboxes.forEach(cb => {
                const bit = parseInt(cb.value);
                cb.checked = (duties & bit) !== 0;
            });
        });

        document.getElementById('setupForm').addEventListener('submit', async function(e) {
            e.preventDefault();

            const formData = new FormData(e.target);

            // Convert checkbox array to bitwise OR
            let dutiesValue = 0;
            const dutyCheckboxes = document.querySelectorAll('input[name="duties"]:checked');
            dutyCheckboxes.forEach(cb => {
                dutiesValue |= parseInt(cb.value);
            });

            // Create data object
            const data = {
                duties: dutiesValue,
                season: parseInt(formData.get('season')),
                tankHeight: parseFloat(formData.get('tankHeight')),
                tankDiameter: parseFloat(formData.get('tankDiameter')),
                emergencyThresholdDry: parseFloat(formData.get('emergencyThresholdDry')),
                emergencyThresholdMonsoon: parseFloat(formData.get('emergencyThresholdMonsoon')),
                govtWaterWaitDry: parseInt(formData.get('govtWaterWaitDry')),
                govtWaterWaitMonsoon: parseInt(formData.get('govtWaterWaitMonsoon')),
                mainsPower: formData.get('mainsPower') === 'true'
            };

            try {
                const response = await fetch('/configure', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(data)
                });

                const result = await response.json();
                const statusDiv = document.getElementById('status');

                if (response.ok) {
                    statusDiv.className = 'status success';
                    statusDiv.textContent = 'Configuration saved! System will restart in autonomous mode.';
                    statusDiv.style.display = 'block';
                } else {
                    statusDiv.className = 'status error';
                    statusDiv.textContent = 'Error: ' + result.error;
                    statusDiv.style.display = 'block';
                }
            } catch (error) {
                document.getElementById('status').className = 'status error';
                document.getElementById('status').textContent = 'Network error: ' + error.message;
                document.getElementById('status').style.display = 'block';
            }
        });
    </script>
</body>
</html>
)rawliteral";

  return html;
}

void handleSetupPost() {
  // Handle form data from checkboxes
  uint8_t selectedDuties = 0;

  // Parse checkbox values
  if (server.hasArg("duties")) {
    int numArgs = server.args();
    for (int i = 0; i < numArgs; i++) {
      if (server.argName(i) == "duties") {
        selectedDuties |= static_cast<uint8_t>(server.arg(i).toInt());
      }
    }
  }

  // Update configuration
  config.duties = selectedDuties;
  config.season = (Season)server.arg("season").toInt();
  config.tankHeight = server.arg("tankHeight").toFloat();
  config.tankDiameter = server.arg("tankDiameter").toFloat();
  config.emergencyThresholdDry = server.arg("emergencyThresholdDry").toFloat();
  config.emergencyThresholdMonsoon = server.arg("emergencyThresholdMonsoon").toFloat();
  config.govtWaterWaitHoursDry = server.arg("govtWaterWaitDry").toInt();
  config.govtWaterWaitHoursMonsoon = server.arg("govtWaterWaitMonsoon").toInt();
  config.mainsPower = (server.arg("mainsPower") == "true");

  // Save to preferences
  saveConfiguration();

  // Send success response
  server.send(200, "application/json", "{\"success\":true,\"message\":\"Configuration saved. System restarting...\"}");

  // Restart after short delay
  delay(2000);
  ESP.restart();
}

void loadConfiguration() {
  // Load from preferences, use defaults if not set
  config.duties = preferences.getInt("duties", DUTY_WATER_LEVEL | DUTY_DECISION_MAKING | DUTY_ALERT_SYSTEM); // Default: all duties for single node
  config.season = (Season)preferences.getInt("season", DRY_SEASON);
  config.tankHeight = preferences.getFloat("tankHeight", 200.0);
  config.tankDiameter = preferences.getFloat("tankDiameter", 150.0);
  config.emergencyThresholdDry = preferences.getFloat("emergencyThresholdDry", 10.0);
  config.emergencyThresholdMonsoon = preferences.getFloat("emergencyThresholdMonsoon", 15.0);
  config.govtWaterWaitHoursDry = preferences.getInt("govtWaterWaitDry", 2);
  config.govtWaterWaitHoursMonsoon = preferences.getInt("govtWaterWaitMonsoon", 4);
  config.mainsPower = preferences.getBool("mainsPower", true);

  currentDuties = config.duties;
  currentSeason = config.season;
  govtWaterLastDetected = millis();
  lowWaterConditionActive = false;
  alertAcknowledged = true;

  // Check if this is first run (no configuration saved)
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
  preferences.putFloat("emergencyThresholdDry", config.emergencyThresholdDry);
  preferences.putFloat("emergencyThresholdMonsoon", config.emergencyThresholdMonsoon);
  preferences.putInt("govtWaterWaitDry", config.govtWaterWaitDry);
  preferences.putInt("govtWaterWaitMonsoon", config.govtWaterWaitHoursMonsoon);
  preferences.putBool("mainsPower", config.mainsPower);
  preferences.putBool("configured", true);
}

void enterAutonomousMode() {
  systemState = AUTONOMOUS_MODE;
  // Disable WiFi and enable Zigbee
  WiFi.mode(WIFI_OFF);
  setupZigbeeNetwork();
}

void setupZigbeeNetwork() {
  // Initialize Zigbee stack
  esp_zb_cfg_t zb_cfg = {
    .esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER,
    .install_code_policy = false,
  };

  esp_zb_init(&zb_cfg);

  // Create endpoint
  createZigbeeEndpoint();

  // Register callbacks
  esp_zb_core_action_handler_register(zigbee_action_handler);

  // Start Zigbee stack
  esp_zb_start(false);

  Serial.println("Zigbee network initialized");
}

void createZigbeeEndpoint() {
  esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
  if (!cluster_list) {
    Serial.println("Failed to create Zigbee cluster list");
    return;
  }

  esp_zb_cluster_t *water_cluster = create_water_management_cluster();
  if (water_cluster) {
    esp_zb_cluster_list_add_custom_cluster(cluster_list, water_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  }

  esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
  if (!ep_list) {
    Serial.println("Failed to create Zigbee endpoint list");
    return;
  }

  esp_zb_ep_list_add_ep(ep_list, cluster_list, WATER_SYSTEM_ENDPOINT, ESP_ZB_AF_HA_PROFILE_ID, ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID);
  esp_zb_device_register(ep_list);
}

void performMeasurements() {
  static unsigned long lastMeasurementTime = 0;
  unsigned long currentTime = millis();

  // Measurement interval based on season and power source
  int measurementInterval = (config.mainsPower ? 30000 : 60000);  // 30s mains, 60s battery

  if (currentTime - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentTime;

    bool canMeasure = currentDuties & DUTY_WATER_LEVEL;
    bool canDetectGovt = currentDuties & DUTY_GOVT_WATER_DETECT;

    if (canMeasure) {
      currentWaterLevel = measureWaterLevel();
    }

    if (canDetectGovt) {
      govtWaterDetected = detectGovtWater();

      if (govtWaterDetected) {
        govtWaterLastDetected = currentTime;
        Serial.println("Government water detected!");
      }
    }

    if (canMeasure || canDetectGovt) {
      Serial.printf("Water Level: %.1f%% | Govt Water: %s\n",
                    currentWaterLevel,
                    govtWaterDetected ? "YES" : "NO");
    }
  }
}

void executeControlLogic() {
  if (!(currentDuties & DUTY_DECISION_MAKING)) {
    return;  // Only nodes with decision making duty execute logic
  }

  float emergencyThreshold = (currentSeason == MONSOON) ?
                            config.emergencyThresholdMonsoon :
                            config.emergencyThresholdDry;

  int govtWaitHours = (currentSeason == MONSOON) ?
                     config.govtWaterWaitHoursMonsoon :
                     config.govtWaterWaitHoursDry;
  bool belowThreshold = currentWaterLevel < emergencyThreshold;

  if (belowThreshold && !lowWaterConditionActive) {
    lowWaterConditionActive = true;
    alertAcknowledged = false;
  } else if (!belowThreshold && lowWaterConditionActive) {
    lowWaterConditionActive = false;
    alertAcknowledged = true;
  }

  // Water filling logic based on dual sources
  if (belowThreshold) {
    bool canUseGovtPump = currentDuties & DUTY_GOVT_PUMP_CONTROL;
    bool canUseAltPump = currentDuties & DUTY_ALT_PUMP_CONTROL;

    if (govtWaterDetected && canUseGovtPump) {
      controlGovtPump(true);
      if (altPumpRunning) controlEmergencyPump(false);
      Serial.println("Using government water supply pump - govt water detected");

    } else if (!govtWaterDetected && canUseAltPump) {
      unsigned long timeSinceGovtWater = millis() - govtWaterLastDetected;
      unsigned long waitTimeMs = govtWaitHours * 60UL * 60UL * 1000UL;

      if (timeSinceGovtWater > waitTimeMs && !altPumpRunning) {
        controlEmergencyPump(true);
        if (govtPumpRunning) controlGovtPump(false);
        Serial.printf("Using emergency water pump - waited %d hours for govt water\n", govtWaitHours);
      }
    }
  }

  // Stop pumps when tank is sufficiently filled (90% for govt water, 60% for alternative)
  if (govtPumpRunning && currentWaterLevel >= 90.0f) {
    controlGovtPump(false);
    Serial.printf("Government pump stopped at %.1f%% (full tank)\n", currentWaterLevel);
  }

  if (altPumpRunning && currentWaterLevel >= 60.0f) {
    controlEmergencyPump(false);
    Serial.printf("Emergency pump stopped at %.1f%% (emergency fill)\n", currentWaterLevel);
  }

  // Safety timeouts (2 hours max for each pump)
  unsigned long safetyTimeoutMs = PUMP_SAFETY_TIMEOUT_HOURS * 60UL * 60UL * 1000UL;
  if (govtPumpRunning && millis() - govtPumpStartTime > safetyTimeoutMs) {
    controlGovtPump(false);
    Serial.println("Government pump stopped - safety timeout");
  }

  if (altPumpRunning && millis() - altPumpStartTime > safetyTimeoutMs) {
    controlEmergencyPump(false);
    Serial.println("Emergency pump stopped - safety timeout");
  }

  // Stop emergency pump if government water becomes available
  if (govtWaterDetected && altPumpRunning && (currentDuties & DUTY_GOVT_PUMP_CONTROL)) {
    controlEmergencyPump(false);
    controlGovtPump(true);
    Serial.println("Switching to government water supply pump - govt water now available");
  }
}

void handleAlerts() {
  // Update LED status
  updateLEDs();

  // Handle buzzer alerts
  float threshold = (currentSeason == MONSOON) ? config.emergencyThresholdMonsoon : config.emergencyThresholdDry;
  int pattern = ALERT_PATTERN_OFF;

  if (currentWaterLevel < threshold) {
    if (!alertAcknowledged) {
      pattern = altPumpRunning ? ALERT_PATTERN_FAST_BEEP : ALERT_PATTERN_MEDIUM_BEEP;
    }
  } else if (govtWaterDetected) {
    pattern = ALERT_PATTERN_SINGLE_BEEP;
  }

  activateBuzzer(pattern);
}

void updateLEDs() {
  auto setLed = [](int pin, int value) {
    if (pin != -1) {
      digitalWrite(pin, value);
    }
  };

  setLed(pins.ledGreen, LOW);
  setLed(pins.ledYellow, LOW);
  setLed(pins.ledRed, LOW);
  setLed(pins.ledBlue, LOW);

  // Status indication
  if (govtWaterDetected) {
    setLed(pins.ledBlue, HIGH);  // Blue for govt water
  } else if (altPumpRunning) {
    setLed(pins.ledRed, HIGH);  // Red for emergency pump
  } else if (currentWaterLevel < (currentSeason == MONSOON ? config.emergencyThresholdMonsoon : config.emergencyThresholdDry)) {
    setLed(pins.ledYellow, HIGH);  // Yellow for low water warning
  } else {
    setLed(pins.ledGreen, HIGH);  // Green for normal operation
  }
}

void activateBuzzer(int pattern) {
  if (pins.buzzer == -1) return;

  static int activePattern = ALERT_PATTERN_OFF;
  static bool buzzerState = false;
  static unsigned long lastChange = 0;

  if (pattern == ALERT_PATTERN_OFF) {
    activePattern = ALERT_PATTERN_OFF;
    buzzerState = false;
    digitalWrite(pins.buzzer, LOW);
    return;
  }

  unsigned long onDuration = 0;
  unsigned long offDuration = 0;

  switch (pattern) {
    case ALERT_PATTERN_SINGLE_BEEP:
      onDuration = 200;
      offDuration = 1800;
      break;
    case ALERT_PATTERN_MEDIUM_BEEP:
      onDuration = 200;
      offDuration = 800;
      break;
    case ALERT_PATTERN_FAST_BEEP:
      onDuration = 150;
      offDuration = 150;
      break;
    default:
      return;
  }

  unsigned long now = millis();

  if (pattern != activePattern) {
    activePattern = pattern;
    buzzerState = false;
    digitalWrite(pins.buzzer, LOW);
    lastChange = now;
  }

  unsigned long interval = buzzerState ? onDuration : offDuration;
  if (now - lastChange >= interval) {
    buzzerState = !buzzerState;
    digitalWrite(pins.buzzer, buzzerState ? HIGH : LOW);
    lastChange = now;
  }
}

float measureWaterLevel() {
  if (pins.ultrasonicTrig == -1 || pins.ultrasonicEcho == -1) {
    return currentWaterLevel;  // Return last known value if no sensor
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

  if (waterDepth < 0) waterDepth = 0;
  if (waterDepth > config.tankHeight) waterDepth = config.tankHeight;

  return (waterDepth / config.tankHeight) * 100.0;
}

bool detectGovtWater() {
  if (pins.govtWaterSensor == -1) return false;

  int sensorValue = digitalRead(pins.govtWaterSensor);
  return sensorValue == HIGH;  // Adjust based on sensor type
}

void controlGovtPump(bool enable) {
  if (pins.govtWaterPumpRelay == -1) return;

  if (enable && !govtPumpRunning) {
    digitalWrite(pins.govtWaterPumpRelay, HIGH);
    govtPumpRunning = true;
    govtPumpStartTime = millis();
    Serial.println("Government water supply pump STARTED");
  } else if (!enable && govtPumpRunning) {
    digitalWrite(pins.govtWaterPumpRelay, LOW);
    govtPumpRunning = false;
    Serial.println("Government water supply pump STOPPED");
  }
}

void controlEmergencyPump(bool enable) {
  if (pins.emergencyPumpRelay == -1) return;

  if (enable && !altPumpRunning) {
    digitalWrite(pins.emergencyPumpRelay, HIGH);
    altPumpRunning = true;
    altPumpStartTime = millis();
    Serial.println("Emergency water pump STARTED");
  } else if (!enable && altPumpRunning) {
    digitalWrite(pins.emergencyPumpRelay, LOW);
    altPumpRunning = false;
    Serial.println("Emergency water pump STOPPED");
  }
}

void updateSeasonalLogic() {
  // Check if season should change (simplified - could be based on date/calendar)
  // For now, manual season change via button or external trigger
  // Future enhancement: calendar-based or environmental sensing
}

esp_err_t zigbee_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  // Handle Zigbee stack events
  switch (callback_id) {
    case ESP_ZB_CORE_NETWORK_FORMED_ID:
      Serial.println("Zigbee network formed");
      break;
    case ESP_ZB_CORE_NETWORK_JOINED_ID:
      Serial.println("Joined Zigbee network");
      break;
    default:
      break;
  }
  return ESP_OK;
}
