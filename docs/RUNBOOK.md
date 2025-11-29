# Water Management System - Runbook

## Table of Contents
1. [Quick Start](#quick-start)
2. [Prerequisites](#prerequisites)
3. [Hardware Setup](#hardware-setup)
4. [Software Build & Flash](#software-build--flash)
5. [Initial Configuration](#initial-configuration)
6. [Operation Guide](#operation-guide)
7. [Monitoring & Status](#monitoring--status)
8. [Troubleshooting](#troubleshooting)
9. [Maintenance](#maintenance)

---

## Quick Start

```bash
# 1. Clone and enter project
cd C:\Users\praghav\IOT\iot-water-managment

# 2. Set target (ESP32 for testing, ESP32-C6/H2 for Zigbee)
idf.py set-target esp32

# 3. Build
idf.py build

# 4. Flash (replace COM3 with your port)
idf.py -p COM3 flash monitor
```

---

## Prerequisites

### Software Requirements

| Component | Version | Installation |
|-----------|---------|--------------|
| ESP-IDF | v5.3+ | [ESP-IDF Installer](https://dl.espressif.com/dl/esp-idf/) |
| Python | 3.8+ | Included with ESP-IDF |
| Git | 2.x | [git-scm.com](https://git-scm.com) |

### Hardware Requirements

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32 / ESP32-C6 / ESP32-H2 | 1-3 | Main controller |
| HC-SR04 Ultrasonic Sensor | 1 | Water level measurement |
| Flow/Pressure Sensor | 1 | Government water detection |
| 5V Relay Module (2-channel) | 1 | Pump control |
| LEDs (Green, Yellow, Red, Blue) | 4 | Status indicators |
| Buzzer | 1 | Audio alerts |
| Push Button | 1 | Alert acknowledgment |
| 220Ω Resistors | 4 | LED current limiting |
| 5V Power Supply | 1 | System power |
| Jumper Wires | As needed | Connections |

---

## Hardware Setup

### Pin Connections Summary

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 Pin Mapping                    │
├──────────────────┬──────────┬───────────────────────────┤
│ Component        │ GPIO Pin │ Notes                     │
├──────────────────┼──────────┼───────────────────────────┤
│ Ultrasonic TRIG  │ GPIO 12  │ Output - 5V tolerant      │
│ Ultrasonic ECHO  │ GPIO 13  │ Input - 5V tolerant       │
│ Govt Water Sensor│ GPIO 14  │ Input - Digital HIGH/LOW  │
│ LED Green        │ GPIO 15  │ Output - 220Ω resistor    │
│ LED Yellow       │ GPIO 16  │ Output - 220Ω resistor    │
│ LED Blue         │ GPIO 17  │ Output - 220Ω resistor    │
│ Govt Pump Relay  │ GPIO 18  │ Output - Active HIGH      │
│ LED Red          │ GPIO 19  │ Output - 220Ω resistor    │
│ Buzzer           │ GPIO 21  │ Output - Active HIGH      │
│ Emergency Relay  │ GPIO 22  │ Output - Active HIGH      │
│ Alert Button     │ GPIO 23  │ Input - Internal pullup   │
│ Node 2 Status LED│ GPIO 33  │ Output - Optional         │
│ Node 3 Status LED│ GPIO 34  │ Output - Optional         │
└──────────────────┴──────────┴───────────────────────────┘
```

### Wiring Steps

1. **Power Connections**
   ```
   5V Power Supply → ESP32 VIN
   GND             → ESP32 GND (common ground for all)
   ```

2. **Ultrasonic Sensor (HC-SR04)**
   ```
   VCC  → 5V
   GND  → GND
   TRIG → GPIO 12
   ECHO → GPIO 13
   ```

3. **Government Water Sensor**
   ```
   VCC → 5V
   GND → GND
   OUT → GPIO 14
   ```

4. **LEDs (each with 220Ω resistor)**
   ```
   GPIO → 220Ω Resistor → LED Anode
   LED Cathode → GND
   ```

5. **Relay Module**
   ```
   VCC → 5V
   GND → GND
   IN1 → GPIO 18 (Govt Pump)
   IN2 → GPIO 22 (Emergency Pump)
   ```

6. **Buzzer**
   ```
   + → GPIO 21
   - → GND
   ```

7. **Alert Button**
   ```
   One terminal → GPIO 23
   Other terminal → GND
   ```

### Safety Checklist Before Power-On

- [ ] All connections double-checked
- [ ] No short circuits (check with multimeter)
- [ ] High voltage (220V) pump wiring isolated
- [ ] Relay rated for pump load current
- [ ] Common ground established

---

## Software Build & Flash

### Step 1: Open ESP-IDF Terminal

```powershell
# Windows: Use ESP-IDF PowerShell/CMD Prompt
# Or run the export script
C:\ESP\esp-idf\export.ps1
```

### Step 2: Navigate to Project

```powershell
cd C:\Users\praghav\IOT\iot-water-managment
```

### Step 3: Set Target

```powershell
# For ESP32 (WiFi-only testing)
idf.py set-target esp32

# For ESP32-C6 (Zigbee production)
idf.py set-target esp32c6

# For ESP32-H2 (Zigbee production)
idf.py set-target esp32h2
```

### Step 4: Build

```powershell
idf.py build
```

**Expected Output:**
```
Project build complete. To flash, run:
 idf.py flash
```

### Step 5: Find COM Port

```powershell
# Windows
mode

# Or check Device Manager → Ports (COM & LPT)
```

### Step 6: Flash & Monitor

```powershell
idf.py -p COM3 flash monitor
```

**Expected Serial Output:**
```
=== 3-Node Zigbee Water Management System ===
System in SETUP MODE - Access web interface at http://<IP>/setup
WiFi AP started: WaterSystem-Setup
Setup AP IP: 192.168.4.1
```

### Step 7: Exit Monitor

Press `Ctrl+]` to exit the serial monitor.

---

## Initial Configuration

### Step 1: Connect to WiFi AP

1. On your phone/laptop, connect to WiFi:
   - **SSID**: `WaterSystem-Setup`
   - **Password**: `watersystem123`

### Step 2: Open Web Interface

1. Open browser and navigate to: `http://192.168.4.1`

### Step 3: Configure Node Duties

Select the duties this node should perform:

| Duty | Description | Required Hardware |
|------|-------------|-------------------|
| Water Level Measurement | Reads ultrasonic sensor | HC-SR04 |
| Government Water Detection | Detects govt supply | Flow sensor |
| Decision Making | Controls pump logic | None |
| Government Pump Control | Activates govt pump | Relay + pump |
| Emergency Pump Control | Activates backup pump | Relay + pump |
| Alert System | LED and buzzer alerts | LEDs + buzzer |
| Status Display | Multi-node status LEDs | Additional LEDs |

### Step 4: Configure Tank & Thresholds

| Parameter | Description | Recommended |
|-----------|-------------|-------------|
| Tank Height | Height in cm | Measure your tank |
| Tank Diameter | Diameter in cm | Measure your tank |
| Emergency Threshold (Dry) | Trigger level | 10% |
| Emergency Threshold (Monsoon) | Trigger level | 15% |
| Govt Water Wait (Dry) | Hours before backup | 2 hours |
| Govt Water Wait (Monsoon) | Hours before backup | 4 hours |
| Power Source | Mains or Battery | Select actual |

### Step 5: Save Configuration

Click **"Save Configuration & Start System"**

The device will restart in **Autonomous Mode**.

---

## Operation Guide

### LED Status Indicators

| LED Color | Status |
|-----------|--------|
| 🟢 Green | Normal operation (water level OK) |
| 🟡 Yellow | Low water warning |
| 🔴 Red | Emergency pump active |
| 🔵 Blue | Government water detected |
| 🔵 Blinking Blue | Setup mode |

### Buzzer Patterns

| Pattern | Meaning |
|---------|---------|
| Single beep (every 2s) | Government water available |
| Medium beeps (every 1s) | Low water warning |
| Fast beeps (continuous) | Emergency pump running |
| No sound | Normal operation |

### Automatic Pump Control Logic

```
IF water_level < emergency_threshold:
    IF govt_water_detected:
        → Activate GOVT PUMP
        → Stop when level reaches 90%
    ELSE IF time_since_govt_water > wait_hours:
        → Activate EMERGENCY PUMP
        → Stop when level reaches 60%

Safety: Both pumps auto-stop after 2 hours
```

### Alert Acknowledgment

Press the **Alert Button** to silence the buzzer when alerted.

---

## Monitoring & Status

### Serial Monitor

```powershell
idf.py -p COM3 monitor
```

**Sample Output:**
```
Water Level: 45.2% | Govt Water: NO
Water Level: 44.8% | Govt Water: NO
Government water detected!
Water Level: 44.5% | Govt Water: YES
Government water supply pump STARTED
Water Level: 52.3% | Govt Water: YES
...
Government pump stopped at 90.1% (full tank)
```

### Web Status API (WiFi Mode Only)

```bash
curl http://192.168.4.1/api/status
```

```json
{
  "waterLevel": 45.2,
  "govtWaterDetected": false,
  "govtPumpRunning": false,
  "altPumpRunning": false,
  "lowWater": false,
  "season": "dry",
  "mode": "autonomous"
}
```

---

## Troubleshooting

### Build Errors

| Error | Solution |
|-------|----------|
| `Failed to resolve component 'arduino'` | Run `idf.py set-target esp32` again |
| `CONFIG_FREERTOS_HZ` mismatch | Already fixed in sdkconfig.defaults |
| `esp_zigbee_core.h not found` | Use ESP32-C6/H2 for Zigbee |

### Hardware Issues

| Symptom | Check |
|---------|-------|
| No serial output | USB cable, COM port, baud rate (115200) |
| Ultrasonic reads 0 | Wiring, 5V power, sensor facing water |
| Pump doesn't activate | Relay wiring, GPIO pin, relay power |
| LEDs don't light | Resistor value, polarity, GPIO |
| Button doesn't work | Wiring, internal pullup enabled |

### WiFi Issues

| Symptom | Solution |
|---------|----------|
| Can't see AP | Restart ESP32, check if in setup mode |
| Can't connect | Use correct password: `watersystem123` |
| Web page doesn't load | Check IP: `192.168.4.1` |

### Sensor Calibration

**Ultrasonic Sensor:**
```cpp
// In config.h, adjust if readings are off:
#define SOUND_SPEED_CM_PER_US 0.0343  // Adjust for temperature

// Test command (in serial monitor):
// Hold object at known distance and compare readings
```

**Government Water Sensor:**
- Verify sensor outputs HIGH (3.3V) when water is flowing
- Some sensors output LOW when active - invert logic if needed

---

## Maintenance

### Daily Checks
- [ ] Verify LED status matches water level
- [ ] Check serial output for errors
- [ ] Ensure pumps respond to manual tests

### Weekly Checks
- [ ] Clean ultrasonic sensor face
- [ ] Check relay contacts for wear
- [ ] Verify WiFi connectivity (if in setup mode)
- [ ] Test alert button functionality

### Monthly Checks
- [ ] Inspect wiring for corrosion
- [ ] Test pump safety timeout
- [ ] Verify tank height calibration
- [ ] Check power supply voltage

### Seasonal Adjustments
- [ ] Update season in configuration (Dry/Monsoon)
- [ ] Adjust emergency thresholds if needed
- [ ] Modify govt water wait times

### Firmware Updates

```powershell
# Pull latest code
git pull

# Rebuild and flash
idf.py build
idf.py -p COM3 flash
```

---

## Reset to Factory Defaults

To clear all configuration and return to setup mode:

```powershell
idf.py -p COM3 erase-flash
idf.py -p COM3 flash
```

This will:
- Erase NVS (stored configuration)
- Reflash firmware
- Device boots into Setup Mode

---

## Support & Documentation

| Document | Description |
|----------|-------------|
| `docs/END_USER_MANUAL.md` | **Non-technical guide** for daily users |
| `docs/USER_GUIDE.md` | Installer & developer guide |
| `docs/WIRING_DIAGRAM.md` | Detailed wiring instructions |
| `docs/IMPLEMENTATION_SUMMARY.md` | Technical overview |
| `README.md` | Project overview |

---

**Last Updated:** November 2024  
**Firmware Version:** Built with ESP-IDF 5.3.4

