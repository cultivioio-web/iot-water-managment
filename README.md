# Zigbee Water Management – ESP-IDF Project

This directory contains an ESP-IDF project that reuses the Zigbee firmware (`zigbee_water_management.ino`) via the *Arduino-as-a-component* workflow. Build it with `idf.py` instead of the Arduino IDE to access the full ESP-Zigbee SDK.

## Prerequisites

1. Install ESP-IDF v5.1+ and export the environment (`idf.py --version` should work).
2. Enable the Arduino component:  
   ```bash
   idf.py add-dependency "arduino=^3.0.0"
   ```
3. Install Espressif’s Zigbee SDK (comes bundled with ESP-IDF).

## Build & Flash

```bash
cd firmware/zigbee_idf
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

The project’s `main/CMakeLists.txt` pulls in:

- `../zigbee_water_management/zigbee_water_management.ino`
- `../zigbee_water_management/zigbee_config.cpp`

Those files still contain the application logic, HTTP setup server, and Zigbee cluster definitions.

## Notes

- The old Arduino-targeted sketch is now treated as shared code. Do not edit it inside the IDF build tree; keep edits in `firmware/zigbee_water_management/`.
- ESP-IDF handles Wi-Fi, HTTP, and Zigbee stacks; make sure your environment provides the required components.
- For tall-tank multi-node deployments, follow `docs/TALL_TANK_DEPLOYMENT.md`.

