# Water Management System - Wiring Diagram

## Overview
This diagram shows how to connect the ESP32, sensors, and actuators for the Zigbee-based water management system. The system supports dynamic node configuration (1-3 nodes) with configurable duties.

## Power Supply Requirements

### ESP32 Power (All Nodes)
- **Input Voltage**: 5V DC (USB) or 3.3V-5V regulated
- **Current Draw**: 500mA typical, 1A peak (when relays activate)
- **Power Source**: USB adapter, battery pack, or solar with regulator

### Relay Power (Pump Control Nodes)
- **Coil Voltage**: 5V DC (matches ESP32 output)
- **Current**: 50-100mA per relay coil
- **Pump Power**: Separate power supply based on pump requirements (typically 220V AC for water pumps)

---

## Wiring Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          ESP32 Zigbee Node                             │
│                    (Configurable for any duty)                         │
├─────────────────────────────────────────────────────────────────────────┤
│  USB Power 5V ──┬─── VIN (5V)                                         │
│  Battery ───────┼─── 3.3V (Output to sensors)                         │
│                 └─── GND (Common ground)                              │
│                                                                       │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐     │
│  │   Ultrasonic    │    │   Govt Water    │    │   LED Status     │     │
│  │   Sensor        │    │   Sensor        │    │   Indicators     │     │
│  │                 │    │                 │    │                 │     │
│  │ VCC ── 5V      │    │ VCC ── 5V      │    │                 │     │
│  │ GND ── GND     │    │ GND ── GND     │    │                 │     │
│  │ TRIG ─ GPIO12  │    │ SIG ─ GPIO14   │    │ Green ─ GPIO15   │     │
│  │ ECHO ─ GPIO13  │    │                 │    │ Yellow─ GPIO16  │     │
│  └─────────────────┘    └─────────────────┘    │ Blue ── GPIO17  │     │
│                                               └─────────────────┘     │
│                                                                       │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐     │
│  │ Govt Water Pump │    │ Emergency Pump  │    │     Buzzer      │     │
│  │     Relay       │    │     Relay       │    │                 │     │
│  │                 │    │                 │    │                 │     │
│  │ VCC ── 5V      │    │ VCC ── 5V      │    │                 │     │
│  │ GND ── GND     │    │ GND ── GND     │    │ GND ── GND      │     │
│  │ IN ── GPIO18   │    │ IN ── GPIO22   │    │ SIG ─ GPIO21     │     │
│  │                 │    │                 │    │                 │     │
│  │ NO ─ Pump+     │    │ NO ─ Pump+     │    │                 │     │
│  │ COM─ Pump Power│    │ COM─ Pump Power│    └─────────────────┘     │
│  │ NC ─ (unused)  │    │ NC ─ (unused)  │                           │
│  └─────────────────┘    └─────────────────┘                           │
│                                                                       │
│  ┌─────────────────┐    ┌─────────────────┐                          │
│  │ Multi-Node LEDs │    │   Alert Button  │                          │
│  │   (Optional)    │    │                 │                          │
│  │                 │    │                 │                          │
│  │ Node2─ GPIO33  │    │ SIG ─ GPIO23   │                          │
│  │ Node3─ GPIO34  │    │ GND─ GND       │                          │
│  │ GND ─ GND      │    │ 3.3V─ 3.3V     │                          │
│  └─────────────────┘    └─────────────────┘                          │
├─────────────────────────────────────────────────────────────────────────┤
│                         External Connections                           │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                       │
│ Government Water Supply Pump:                                        │
│ ┌─────────────────────────────────────────────────────────────────┐   │
│ │ 220V AC Mains ──┬─── Relay COM                                 │   │
│ │                 │                                               │   │
│ │                 ├─── Pump Power Input                          │   │
│ │                 │                                               │   │
│ │                 └─── Relay NO (Normally Open)                  │   │
│ └─────────────────────────────────────────────────────────────────┘   │
│                                                                       │
│ Emergency Water Pump:                                                │
│ ┌─────────────────────────────────────────────────────────────────┐   │
│ │ 220V AC Mains ──┬─── Relay COM                                 │   │
│ │                 │                                               │   │
│ │                 ├─── Pump Power Input                          │   │
│ │                 │                                               │   │
│ │                 └─── Relay NO (Normally Open)                  │   │
│ └─────────────────────────────────────────────────────────────────┘   │
│                                                                       │
│ Water Tank Sensors:                                                  │
│ ┌─────────────────────────────────────────────────────────────────┐   │
│ │ Ultrasonic Sensor positioned above water level                 │   │
│ │ - Distance measurement: 2cm to 400cm                           │   │
│ │ - Beam angle: 15 degrees                                       │   │
│ │ - Resolution: 1mm                                              │   │
│ └─────────────────────────────────────────────────────────────────┘   │
│                                                                       │
│ Government Water Detection:                                         │
│ ┌─────────────────────────────────────────────────────────────────┐   │
│ │ Flow Sensor or Pressure Sensor in govt water line              │   │
│ │ - Digital output: HIGH when water flowing                     │   │
│ │ - Power: 5V DC, 15mA                                           │   │
│ └─────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────┘
```

## Detailed Pin Assignments

### Primary Sensor Node Configuration
```cpp
#define ULTRASONIC_TRIG_PIN 12     // Ultrasonic trigger
#define ULTRASONIC_ECHO_PIN 13     // Ultrasonic echo
#define GOVT_WATER_SENSOR_PIN 14   // Government water detection
#define LED_GREEN_PIN 15           // Normal operation indicator
#define LED_YELLOW_PIN 16          // Low water warning
#define LED_BLUE_PIN 17            // Government water detected
```

### Government Pump Controller Node Configuration
```cpp
#define GOVT_WATER_PUMP_RELAY_PIN 18  // Government water pump relay
#define LED_RED_PIN 19               // Emergency pump active
#define BUZZER_PIN 21                // Alert buzzer
```

### Emergency Pump Controller Node Configuration
```cpp
#define EMERGENCY_PUMP_RELAY_PIN 22  // Emergency pump relay
#define ALERT_BUTTON_PIN 23          // Alert acknowledgment button
#define BACKUP_LED_RED_PIN 25        // Backup emergency indicator
#define BACKUP_LED_YELLOW_PIN 26     // Backup warning indicator
#define BACKUP_LED_GREEN_PIN 27      // Backup normal indicator
#define BACKUP_BUZZER_PIN 32         // Backup buzzer
```

### Multi-Node Status Display (Optional)
```cpp
#define NODE2_STATUS_LED 33         // Shows Node 2 status
#define NODE3_STATUS_LED 34         // Shows Node 3 status
```

## Relay Module Details

### Relay Specifications
- **Type**: SPDT (Single Pole Double Throw) or SPST (Single Pole Single Throw)
- **Coil Voltage**: 5V DC
- **Coil Current**: 70mA (typical)
- **Contact Rating**: 10A @ 250V AC (for pump control)
- **Response Time**: 10ms

### Relay Pin Connections
```
Relay Module:
├── VCC  → ESP32 5V or 3.3V
├── GND  → ESP32 GND
├── IN   → ESP32 GPIO pin (18 for govt pump, 22 for emergency pump)
├── NO   → Pump power live wire
├── COM  → AC mains live wire
└── NC   → (Unused)
```

## Sensor Details

### Ultrasonic Sensor (HC-SR04)
```
HC-SR04 Pinout:
├── VCC  → 5V
├── GND  → GND
├── TRIG → GPIO12
└── ECHO → GPIO13

Mounting: Point downwards towards water surface
Distance: 2-400cm range
Beam: 15° cone angle
```

### Government Water Sensor
```
Flow/Pressure Sensor:
├── VCC  → 5V
├── GND  → GND
├── OUT  → GPIO14 (Digital signal)
└── Optional: Analog output if available

Mounting: In government water supply line
Trigger: HIGH when water is flowing
```

## LED Status Indicators

### LED Specifications
- **Type**: Standard 5mm LEDs
- **Current**: 20mA max (use 220Ω resistor)
- **Voltage**: 2-3V forward voltage
- **Color Coding**:
  - Green: Normal operation
  - Yellow: Low water warning
  - Red: Emergency pump active
  - Blue: Government water detected

### LED Circuit
```
ESP32 GPIO ── Resistor (220Ω) ── LED Anode
                                      │
                                      └── LED Cathode ── GND
```

## Power Distribution

### Main Power Rail
```
5V DC Power Supply → ESP32 VIN
                    ├── Relay VCC (both relays)
                    ├── Ultrasonic VCC
                    └── Government Sensor VCC

3.3V from ESP32 → LED current limiting resistors
                  └── Button pull-up (internal)
```

### Pump Power (Separate Circuit)
```
220V AC Mains → Circuit Breaker/Fuse
                ├── Government Pump Relay COM
                └── Emergency Pump Relay COM

Relays NO contacts → Individual pump power inputs
```

## Safety Considerations

### Electrical Safety
- Use appropriate wire gauge for pump current (14 AWG minimum)
- Install circuit breakers on pump circuits
- Keep high voltage (220V) and low voltage (5V) circuits separate
- Use proper insulation and cable management

### Water Safety
- Mount ultrasonic sensor above maximum water level
- Waterproof all electronics near water
- Use appropriate pipe materials and fittings
- Install check valves to prevent backflow

### System Safety
- Test all connections before applying power
- Verify relay operation with multimeter
- Calibrate ultrasonic sensor distance readings
- Test emergency shutdown procedures

## Testing Checklist

### Power Connections
- [ ] ESP32 powers on (LEDs indicate setup mode)
- [ ] Relay coils energize when commanded
- [ ] Ultrasonic sensor receives power (no error messages)

### Sensor Connections
- [ ] Ultrasonic readings change with hand movement
- [ ] Government water sensor triggers when activated
- [ ] Button press acknowledged (if equipped)

### Pump Control
- [ ] Government pump relay clicks when activated
- [ ] Emergency pump relay clicks when activated
- [ ] Both relays never activate simultaneously

### Indicator Lights
- [ ] Green LED lights during normal operation
- [ ] Yellow LED lights when water low
- [ ] Red LED lights when emergency pump active
- [ ] Blue LED lights when government water detected

## Node Configuration Examples

### Single Node (All Duties)
Connect all sensors and actuators to one ESP32 board.

### Two Node Network
```
Node 1 (Sensor): Ultrasonic + Govt Water Sensor + LEDs
Node 2 (Control): Both Pump Relays + Buzzer + Alert Button
```

### Three Node Network
```
Node 1 (Primary Sensor): Ultrasonic + Govt Water + LEDs
Node 2 (Govt Pump): Govt Pump Relay + Red LED + Buzzer
Node 3 (Emergency): Emergency Pump Relay + Alert System + Status LEDs
```

---

**Note**: Always double-check connections against this diagram before applying power. Use a multimeter to verify continuity and proper voltage levels. Safety first! ⚡
