# Cultivio AquaSense Firmware

## 🎯 Unified Firmware Architecture

The Cultivio AquaSense system uses a **single unified firmware** that can be configured as any device role during setup. This simplifies deployment and inventory management.

```
┌─────────────────────────────────────────────────────────────┐
│                   UNIFIED FIRMWARE                          │
│                                                             │
│   Flash same firmware to ALL ESP32-H2 devices               │
│                                                             │
│   ┌──────────────────────────────────────────────────────┐  │
│   │            BLE PROVISIONING (Setup)                  │  │
│   │                                                      │  │
│   │   ┌──────────┐ ┌────────────┐ ┌──────────┐           │  │
│   │   │  SENSOR  │ │ CONTROLLER │ │  ROUTER  │  User     │  │
│   │   │    📡    │ │     🎛️    │ │    📶    │  Selects │  │
│   │   └──────────┘ └────────────┘ └──────────┘           │  │
│   └──────────────────────────────────────────────────────┘  │
│                          ↓                                  │
│   Role saved to NVS → Device runs as selected role          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 📁 Firmware Structure

```
firmware/
├── unified/              # ← USE THIS (Single firmware for all roles)
│   ├── main/
│   │   ├── unified_main.c
│   │   ├── CMakeLists.txt
│   │   └── idf_component.yml
│   ├── CMakeLists.txt
│   ├── sdkconfig.defaults
│   ├── sdkconfig.defaults.esp32h2
│   └── partitions.csv
│
├── shared/               # Shared components
│   ├── ble_provision.h   # BLE provisioning + status monitoring
│   ├── ble_provision.c
│   └── cultivio_brand.h  # Company branding
│
├── sensor_node/          # (Legacy - use unified instead)
├── controller_node/      # (Legacy - use unified instead)
└── router_node/          # (Legacy - use unified instead)
```

## 🚀 Quick Start

### 1. Build the Unified Firmware

```powershell
# Navigate to unified firmware
cd firmware/unified

# Set target (ESP32-H2)
idf.py set-target esp32h2

# Build
idf.py build
```

### 2. Flash to All Devices

```powershell
# Flash the same firmware to every ESP32-H2
idf.py -p COMx flash
```

### 3. Configure Each Device via Mobile App

1. Power on device → Automatically enters provisioning mode
2. Open provisioning app (or web app)
3. Connect to "Cultivio-XXXX" via Bluetooth
4. **Select device role:**
   - 📡 **Sensor Node** - Place at tank
   - 🎛️ **Controller Node** - Place near pump
   - 📶 **Router Node** - Place in between (for tall buildings)
5. **Set device identification (for multi-device buildings):**
   - Custom name (e.g., "Flat301" → shows as "Cultivio-Flat301")
   - Password (default: `1234`, change it!)
   - Location (e.g., "Building A, 3rd Floor")
6. Configure role-specific settings
7. Save → Device restarts in selected role

## 🔐 Device Identification & Security

### Custom Device Names
For buildings with multiple Cultivio devices:
```
Default:    Cultivio-A3B2     (MAC-based)
Custom:     Cultivio-Flat301  (User-friendly!)
```

### Password Protection
- **Default Password:** `1234`
- Enable "Require password" in shared buildings
- Prevents neighbors from controlling your device

### BLE Commands for Identification
| Command | Description |
|---------|-------------|
| `0x05` | Set custom name |
| `0x06` | Set password |
| `0x07` | Set location |

## 📋 Device Roles

### 📡 Sensor Node (Water Level Measurement)
- Measures water level using HC-SR04 ultrasonic sensor
- Reports to Controller via Zigbee
- Broadcasts status via BLE for mobile monitoring
- **Zigbee Role:** End Device

**Configuration:**
- Tank height (cm)
- Tank diameter (cm)
- Sensor offset (cm)
- Report interval (seconds)

### 🎛️ Controller Node (Pump Control)
- Receives water level from Sensor via Zigbee
- Controls pump relay based on thresholds
- Supports manual pump override via mobile app
- **Zigbee Role:** Coordinator

**Configuration:**
- Pump ON threshold (%)
- Pump OFF threshold (%)
- Pump timeout (minutes)

### 📶 Router Node (Range Extender)
- Extends Zigbee network range
- No sensors or actuators
- Just forwards Zigbee messages
- **Zigbee Role:** Router

**Use when:**
- Building has 4+ floors
- Distance between Sensor and Controller > 15m
- Signal strength < 40%

## 🔧 Hardware Connections

### All Nodes (Common)
| Pin | Function |
|-----|----------|
| GPIO8 | Status LED |
| GPIO9 | Activity LED / Pump LED |
| GPIO10 | Provisioning Button |

### Sensor Node (Additional)
| Pin | Function |
|-----|----------|
| GPIO2 | Ultrasonic TRIG |
| GPIO3 | Ultrasonic ECHO |

### Controller Node (Additional)
| Pin | Function |
|-----|----------|
| GPIO2 | Relay Control |

## 📱 Mobile App Features

After provisioning, the mobile app can:

1. **View Live Status**
   - Water level (%)
   - Pump status (ON/OFF/MANUAL)
   - Zigbee connection
   - Signal strength
   - Uptime

2. **Manual Pump Control** (Controller only)
   - Start pump for 10/15/20/30/45/60 minutes
   - Stop pump immediately
   - View countdown timer

3. **Re-provision** (Hold button for 3 seconds)
   - Change device role
   - Update configuration

## 🏗️ Building for Different Scenarios

### For 1-3 Story Buildings (2 Nodes)
```
[Sensor at Tank] ←──Zigbee──→ [Controller at Pump]
```
Flash unified firmware to 2x ESP32-H2, configure as Sensor and Controller.

### For 4-5 Story Buildings (3 Nodes)
```
[Sensor] ←──Zigbee──→ [Router] ←──Zigbee──→ [Controller]
  Tank                 Middle                  Pump
```
Flash unified firmware to 3x ESP32-H2, configure as Sensor, Router, and Controller.

## 📦 Dependencies

The firmware uses these ESP-IDF components:
- `espressif/esp-zigbee-lib` - Zigbee stack
- `espressif/esp-zboss-lib` - Zigbee low-level library
- `nvs_flash` - Non-volatile storage
- `bt` - Bluetooth (BLE) stack

## 🔍 Troubleshooting

### Device won't enter provisioning mode
- Hold button for 3+ seconds during boot
- LED should blink rapidly

### Can't connect via Bluetooth
- Ensure phone's Bluetooth is on
- Look for "Cultivio-XXXX" in device list
- Try power cycling the device

### Zigbee not connecting
- Ensure Controller is powered on first (it's the coordinator)
- Check that devices are within range
- Consider adding a Router node

### Build errors
```powershell
# Clean and rebuild
idf.py fullclean
idf.py set-target esp32h2
idf.py build
```

## 📄 License

Copyright (c) 2024 Cultivio. All rights reserved.
