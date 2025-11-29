# Zigbee Firmware (Source Files)

This folder now only holds the shared source files (`zigbee_water_management.ino`, `zigbee_config.*`, `config.h`) that are consumed by both:

- The legacy Arduino build (no longer recommended for Zigbee, but retained for reference)
- The ESP-IDF project at `firmware/zigbee_idf/` (preferred build path)

To build and flash the Zigbee firmware, use the ESP-IDF project. The `.ino` file remains in place so the same logic can be imported by both environments without duplication.

