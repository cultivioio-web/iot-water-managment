# Zigbee Water Management – ESP-IDF Project

This directory contains an ESP-IDF project that reuses the Zigbee firmware (`zigbee_water_management.ino`) via the *Arduino-as-a-component* workflow. Build it with `idf.py` instead of the Arduino IDE to access the full ESP-Zigbee SDK.

For a complete install + provisioning walkthrough, see `docs/USER_GUIDE.md`.

## Prerequisites

1. Install ESP-IDF v5.1+ and export the environment (`idf.py --version` should work).
2. Arduino-as-a-component is declared in `main/idf_component.yml`. `idf.py`
   will pull it automatically, but you can pin it manually if needed:
   ```bash
   idf.py add-dependency "espressif/arduino-esp32@^3.0.0"
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

If you mirror the shared firmware inside this repo instead (e.g.
`firmware/zigbee_water_management`) or keep it one directory higher
(`../../zigbee_water_management`), the build will auto-detect those layouts as
well. The recommended setup is to keep a sibling checkout at
`../zigbee_water_management/`.

## Notes

- The old Arduino-targeted sketch is now treated as shared code. Do not edit it inside the IDF build tree; keep edits in `firmware/zigbee_water_management/`.
- ESP-IDF handles Wi-Fi, HTTP, and Zigbee stacks; make sure your environment provides the required components.
- For tall-tank multi-node deployments, follow `docs/TALL_TANK_DEPLOYMENT.md`.
- ESP32-class targets do not ship a native 802.15.4 MAC. The build includes a
  minimal `esp_radio_spinel` shim so the Zigbee stack can compile on ESP-IDF
  v5.1, but a real Zigbee radio co-processor (and Espressif’s official
  `esp_radio` component available in IDF v5.2+) is still required for Zigbee
  traffic to flow at runtime.

