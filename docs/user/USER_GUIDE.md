# Water Management System - User Guide

## Phase 1: ESP32-H2 Zigbee System

This guide covers everything you need to set up, configure, and operate the IoT water management system.

---

## 📖 Table of Contents

1. [System Overview](#1-system-overview)
2. [Hardware Requirements](#2-hardware-requirements)
3. [Software Setup](#3-software-setup)
4. [Assembly](#4-assembly)
5. [Firmware Installation](#5-firmware-installation)
6. [Device Provisioning](#6-device-provisioning)
7. [Installation](#7-installation)
8. [Operation](#8-operation)
9. [Maintenance](#9-maintenance)
10. [Troubleshooting](#10-troubleshooting)

---

## 1. System Overview

### Architecture

The Phase 1 system consists of two ESP32-H2 nodes communicating via Zigbee:

```
┌─────────────────────┐              ┌─────────────────────────┐
│   SENSOR NODE       │   Zigbee    │   CONTROLLER NODE       │
│   (End Device)      │◄═══════════►│   (Coordinator)         │
│                     │  802.15.4   │                         │
│  • Water level      │             │  • Pump control         │
│    measurement      │             │  • Decision logic       │
│  • Status LEDs      │             │  • Status LEDs          │
└─────────────────────┘             └─────────────────────────┘
```

### Features

| Feature | Description |
|---------|-------------|
| **Zigbee Communication** | Low-power IEEE 802.15.4 mesh |
| **BLE Provisioning** | Configure via smartphone app |
| **Automatic Pump Control** | ON/OFF based on water level |
| **Safety Timeout** | Pump auto-off after configured time |
| **Visual Indicators** | LED status for all operations |

### How It Works

1. **Sensor Node** measures water level with ultrasonic sensor
2. **Data sent** to Controller via Zigbee every 5 seconds
3. **Controller** decides when to turn pump ON/OFF
4. **Pump activates** when water falls below threshold
5. **Pump stops** when water rises above threshold

---

## 2. Hardware Requirements

### Bill of Materials

See [BOM.md](./BOM.md) for complete parts list.

**Quick Summary:**
| Item | Qty | Est. Cost |
|------|-----|-----------|
| ESP32-H2 Mini | 2 | $12-16 |
| HC-SR04 Sensor | 1 | $2-4 |
| 5V Relay Module | 1 | $2-4 |
| LEDs, Resistors, Buttons | - | $2-3 |
| Wires, Enclosures | - | $8-15 |
| **Total** | | **~$35** |

### Pin Assignments

**Sensor Node:**
| GPIO | Function |
|------|----------|
| GPIO2 | HC-SR04 Trigger |
| GPIO3 | HC-SR04 Echo |
| GPIO8 | Status LED |
| GPIO9 | Provisioning LED |
| GPIO10 | Provisioning Button |

**Controller Node:**
| GPIO | Function |
|------|----------|
| GPIO2 | Pump Relay |
| GPIO8 | Status LED |
| GPIO9 | Pump LED |
| GPIO10 | Provisioning Button |

See [WIRING_DIAGRAM.md](./WIRING_DIAGRAM.md) for detailed diagrams.

---

## 3. Software Setup

### Prerequisites

1. **ESP-IDF v5.1+** installed
   - Windows: Use [ESP-IDF Tools Installer](https://dl.espressif.com/dl/esp-idf/)
   - Linux/Mac: Follow [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/get-started/)

2. **Git** for cloning repository

3. **Chrome or Edge browser** for provisioning app

### Verify Installation

```bash
# Check ESP-IDF is installed
idf.py --version

# Should show: ESP-IDF v5.x.x
```

---

## 4. Assembly

See [ASSEMBLY_GUIDE.md](./ASSEMBLY_GUIDE.md) for step-by-step instructions.

### Quick Assembly Overview

**Sensor Node:**
1. Connect HC-SR04 to ESP32-H2 (5V, GND, GPIO2, GPIO3)
2. Add LEDs with 220Ω resistors (GPIO8, GPIO9)
3. Add button between GPIO10 and GND
4. Mount in weatherproof enclosure

**Controller Node:**
1. Connect Relay Module to ESP32-H2 (5V, GND, GPIO2)
2. Add LEDs with 220Ω resistors (GPIO8, GPIO9)
3. Add button between GPIO10 and GND
4. Wire relay to pump (CAUTION: High voltage!)

---

## 5. Firmware Installation

### Build Sensor Node

```bash
# Navigate to sensor node directory
cd firmware/sensor_node

# Set target to ESP32-H2
idf.py set-target esp32h2

# Build the firmware
idf.py build

# Flash to device (replace COM3 with your port)
idf.py -p COM3 flash monitor
```

### Build Controller Node

```bash
cd firmware/controller_node

idf.py set-target esp32h2
idf.py build
idf.py -p COM4 flash monitor
```

### Expected Output

```
I (xxx) SENSOR_NODE: ========================================
I (xxx) SENSOR_NODE:   Water Level Sensor Node - Phase 1
I (xxx) SENSOR_NODE: ========================================
I (xxx) BLE_PROV: Device not provisioned - entering setup mode
I (xxx) BLE_PROV: BLE Provisioning started
```

---

## 6. Device Provisioning

### Overview

Both devices are configured via Bluetooth using the web provisioning app. No coding required!

### Step 1: Open Provisioning App

Open in **Chrome** or **Edge** browser:
```
provisioning-app/index.html
```

Or host on a web server and access via URL.

### Step 2: Enter Provisioning Mode

**Automatic:** First boot enters provisioning mode
**Manual:** Hold GPIO10 button for 3 seconds

LED will blink rapidly when in provisioning mode.

### Step 3: Connect to Device

1. Click **"Scan for Devices"**
2. Select your device (WaterNode-XXXX)
3. Wait for connection (LED blinks slower)

### Step 4: Configure Sensor Node

| Setting | Default | Description |
|---------|---------|-------------|
| Tank Height | 200 cm | Height of your water tank |
| Tank Diameter | 100 cm | Diameter of tank |
| Sensor Offset | 5 cm | Distance from sensor to max water |
| Report Interval | 5 sec | How often to send readings |

### Step 5: Configure Controller Node

| Setting | Default | Description |
|---------|---------|-------------|
| Pump ON Threshold | 20% | Turn pump ON below this |
| Pump OFF Threshold | 80% | Turn pump OFF above this |
| Pump Timeout | 60 min | Max pump runtime |

### Step 6: Save & Activate

1. Click **"Save & Activate"**
2. Device saves configuration
3. LED blinks 5 times (success)
4. Device restarts automatically

### Step 7: Repeat for Second Node

---

## 7. Installation

### Sensor Node Placement

```
         5cm offset
           │
     ┌─────▼─────┐
     │  ┌─────┐  │
     │  │SR04 │  │  ← Mount sensor pointing down
     │  └──▼──┘  │
     │     │     │
     │  ~~~│~~~  │  ← Water surface
     │     │     │
     │  Tank     │
     └───────────┘
```

**Installation Tips:**
- Mount sensor perpendicular to water surface
- Keep 5cm clearance from tank top
- Use waterproof enclosure for electronics
- Run cables through cable glands

### Controller Node Placement

- Mount in **dry location** near pump
- Keep away from water and humidity
- Ensure proper ventilation
- Easy access for maintenance

### Pump Wiring

⚠️ **HIGH VOLTAGE - HIRE A QUALIFIED ELECTRICIAN!**

See [WIRING_DIAGRAM.md](./WIRING_DIAGRAM.md) for details.

---

## 8. Operation

### Startup Sequence

1. **Power on Controller Node first**
   - Forms Zigbee network
   - Status LED blinks while waiting

2. **Power on Sensor Node**
   - Joins Zigbee network
   - LED blinks 3 times on success
   - Starts sending water level data

3. **Normal Operation**
   - Controller LED solid when receiving data
   - Pump LED indicates pump status

### LED Indicators

**Sensor Node:**
| LED State | Meaning |
|-----------|---------|
| Fast blink | Provisioning mode |
| Single blink | Normal operation |
| Triple blink | Sensor error |

**Controller Node:**
| LED State | Meaning |
|-----------|---------|
| Fast blink | Provisioning mode |
| Solid | Sensor connected |
| Blinking | Waiting for sensor |
| Pump LED ON | Pump is running |

### Automatic Operation

The system operates automatically after setup:

```
Water Level Check
       │
       ▼
   Below 20%? ──Yes──► Pump ON
       │
      No
       │
       ▼
   Above 80%? ──Yes──► Pump OFF
       │
      No
       │
       ▼
   Keep current state
```

---

## 9. Maintenance

### Regular Checks

| Task | Frequency |
|------|-----------|
| Check water level readings | Weekly |
| Clean ultrasonic sensor | Monthly |
| Check relay operation | Monthly |
| Verify LED indicators | Weekly |
| Check enclosure seals | Monthly |

### Re-Provisioning

To change settings:
1. Hold button for 3 seconds
2. Connect with provisioning app
3. Update settings
4. Save & Activate

### Factory Reset

```bash
idf.py -p COMx erase-flash
idf.py -p COMx flash
```

Then re-provision the device.

### Firmware Updates

```bash
cd firmware/sensor_node  # or controller_node
git pull
idf.py build
idf.py -p COMx flash
```

---

## 10. Troubleshooting

### Common Issues

| Problem | Possible Cause | Solution |
|---------|----------------|----------|
| Device not in provisioning mode | Button not held long enough | Hold 3+ seconds |
| Bluetooth not finding device | Wrong browser | Use Chrome or Edge |
| Sensor readings wrong | Sensor mounted incorrectly | Mount straight down |
| Pump not activating | Relay wiring wrong | Check NO/COM connections |
| Zigbee not connecting | Nodes too far apart | Move closer (< 10m) |
| LEDs not working | Wrong polarity | Check LED orientation |

### Serial Monitor Debug

```bash
idf.py -p COMx monitor
```

Press `Ctrl+]` to exit.

### Reset Options

| Reset Type | Method |
|------------|--------|
| Soft reset | Press EN button or power cycle |
| Re-provision | Hold GPIO10 for 3 seconds |
| Full factory reset | `idf.py erase-flash` |

---

## 📚 Additional Documentation

| Document | Description |
|----------|-------------|
| [WIRING_DIAGRAM.md](./WIRING_DIAGRAM.md) | Detailed pin connections |
| [BOM.md](./BOM.md) | Complete parts list |
| [ASSEMBLY_GUIDE.md](./ASSEMBLY_GUIDE.md) | Step-by-step assembly |
| [architecture_discussion.md](./architecture_discussion.md) | System design details |

---

## 🆘 Getting Help

1. Check [Troubleshooting](#10-troubleshooting) section
2. Review serial monitor output
3. Check LED indicators
4. Verify wiring against diagrams
5. Open an issue on GitHub

---

**Document Version:** 2.0 (Phase 1 - ESP32-H2)  
**Last Updated:** December 2024
