# Cultivio AquaSense

<p align="center">
  <img src="docs/assets/cultivio-logo.svg" alt="Cultivio Logo" width="200">
</p>

<p align="center">
  <strong>Smart IoT Water Management System</strong><br>
  Automated water level monitoring and pump control using ESP32-H2 & Zigbee
</p>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#quick-start">Quick Start</a> •
  <a href="#documentation">Documentation</a> •
  <a href="#hardware">Hardware</a>
</p>

---

## Features

- 🌊 **Automatic Water Level Monitoring** - Ultrasonic sensor measures tank water level
- 🔄 **Smart Pump Control** - Automatically turns pump ON/OFF based on thresholds
- 📡 **Zigbee Communication** - Low-power wireless between sensor and controller
- 📱 **BLE Provisioning** - Configure devices via smartphone/browser app
- ⚡ **ESP32-H2 Powered** - Modern, efficient microcontroller with native Zigbee
- 🛡️ **Safety Features** - Pump timeout, sensor failure detection

## System Architecture

```
┌─────────────────────┐              ┌─────────────────────────┐
│   SENSOR NODE       │   Zigbee    │   CONTROLLER NODE       │
│   (ESP32-H2)        │◄═══════════►│   (ESP32-H2)            │
│                     │  802.15.4   │                         │
│  📊 Water Level     │             │  ⚙️ Pump Control        │
│  📡 Sends Data      │             │  📥 Receives Data       │
│  💡 Status LEDs     │             │  💡 Status LEDs         │
└─────────────────────┘             └─────────────────────────┘
         │                                    │
         ▼                                    ▼
    ┌─────────┐                          ┌─────────┐
    │  WATER  │                          │  WATER  │
    │  TANK   │◄─────────────────────────│  PUMP   │
    └─────────┘         Water Flow       └─────────┘
```

## Quick Start

### 1. Hardware Required

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32-H2 Mini | 2 | Microcontrollers |
| HC-SR04 Sensor | 1 | Water level measurement |
| 5V Relay Module | 1 | Pump control |
| LEDs + Resistors | 4 | Status indicators |

See [BOM.md](docs/technical/BOM.md) for complete parts list.

### 2. Build & Flash (Unified Firmware)

```bash
# Clone repository
git clone https://github.com/cultivio/aquasense.git
cd aquasense

# Build unified firmware (works for all device roles)
cd firmware/unified
idf.py set-target esp32h2
idf.py build

# Flash to each ESP32-H2 device
idf.py -p COM3 flash  # Device 1
idf.py -p COM4 flash  # Device 2
idf.py -p COM5 flash  # Device 3 (optional router)
```

**Note:** Same firmware for all devices! Role (Sensor/Controller/Router) is selected during provisioning.

### 3. Provision Devices

1. Open `provisioning-app/index.html` in Chrome/Edge
2. Power on device (LED blinks rapidly)
3. Click "Scan for Devices"
4. Configure settings
5. Save & Activate

## Documentation

📁 All documentation is organized in the [`docs/`](docs/) folder.

### 👤 For End Users
| Document | Description |
|----------|-------------|
| [END_USER_MANUAL.md](docs/user/END_USER_MANUAL.md) | Simple guide for daily use |
| [END_USER_MANUAL_HINGLISH.md](docs/user/END_USER_MANUAL_HINGLISH.md) | User manual in Hinglish |
| [USER_GUIDE.md](docs/user/USER_GUIDE.md) | Complete technical guide |

### 🔧 For Installers
| Document | Description |
|----------|-------------|
| [ASSEMBLY_GUIDE.md](docs/installation/ASSEMBLY_GUIDE.md) | Hardware assembly instructions |
| [WIRING_DIAGRAM.md](docs/installation/WIRING_DIAGRAM.md) | Pin connections and diagrams |
| [CALIBRATION_GUIDE.md](docs/installation/CALIBRATION_GUIDE.md) | Sensor calibration procedures |
| [DEPLOYMENT_CHECKLIST.md](docs/installation/DEPLOYMENT_CHECKLIST.md) | Field deployment checklist |
| [OVERHEAD_TANK_SETUP.md](docs/installation/OVERHEAD_TANK_SETUP.md) | Multi-floor building setup |

### 💻 For Developers
| Document | Description |
|----------|-------------|
| [BOM.md](docs/technical/BOM.md) | Bill of Materials with pricing |
| [architecture_discussion.md](docs/technical/architecture_discussion.md) | System architecture |

### 📈 For Sales/Business
| Document | Description |
|----------|-------------|
| [MARKETING_BROCHURE.md](docs/marketing/MARKETING_BROCHURE.md) | Product brochure |
| [COMPETITIVE_ANALYSIS.md](docs/marketing/COMPETITIVE_ANALYSIS.md) | Market analysis |
| [MARKETING_DEALER_KIT.md](docs/marketing/MARKETING_DEALER_KIT.md) | Dealer info |

## Project Structure

```
cultivio-aquasense/
├── firmware/
│   ├── unified/                  # 🎯 Single firmware for all devices
│   │   ├── main/
│   │   │   └── unified_main.c    # Role-based main logic
│   │   └── CMakeLists.txt
│   └── shared/                   # Shared components
│       └── ble_provision/        # BLE provisioning & status
│
├── provisioning-app/
│   └── index.html                # 📱 Web-based device setup & monitoring
│
├── docs/
│   ├── 📁 user/                  # End user documentation
│   │   ├── END_USER_MANUAL.md
│   │   ├── END_USER_MANUAL_HINGLISH.md
│   │   └── USER_GUIDE.md
│   ├── 📁 installation/          # Installation guides
│   │   ├── ASSEMBLY_GUIDE.md
│   │   ├── WIRING_DIAGRAM.md
│   │   ├── CALIBRATION_GUIDE.md
│   │   └── DEPLOYMENT_CHECKLIST.md
│   ├── 📁 technical/             # Technical docs
│   │   ├── BOM.md
│   │   └── architecture_discussion.md
│   ├── 📁 marketing/             # Sales materials
│   │   ├── MARKETING_BROCHURE.md
│   │   └── COMPETITIVE_ANALYSIS.md
│   └── 📁 assets/                # Images & logos
│       └── cultivio-logo.svg
│
└── README.md
```

## Hardware Connections

### Sensor Node (ESP32-H2 Mini)
| GPIO | Function |
|------|----------|
| GPIO2 | Ultrasonic TRIG |
| GPIO3 | Ultrasonic ECHO |
| GPIO8 | Status LED |
| GPIO9 | Provisioning LED |
| GPIO10 | Provisioning Button |

### Controller Node (ESP32-H2 Mini)
| GPIO | Function |
|------|----------|
| GPIO2 | Pump Relay |
| GPIO8 | Status LED |
| GPIO9 | Pump LED |
| GPIO10 | Provisioning Button |

## Configuration Options

### Sensor Node
- Tank Height (cm)
- Tank Diameter (cm)
- Sensor Offset (cm)
- Report Interval (sec)

### Controller Node
- Pump ON Threshold (%)
- Pump OFF Threshold (%)
- Pump Timeout (min)
- Zigbee Channel

## Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

## License

Copyright © 2024 Cultivio. All rights reserved.

---

<p align="center">
  Made with 💚 by <strong>Cultivio</strong>
</p>
