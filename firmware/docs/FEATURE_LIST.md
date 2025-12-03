# Cultivio AquaSense Firmware Feature List

## Overview
The Cultivio AquaSense firmware is designed for ESP32-H2-based IoT devices in a water management system. It supports three roles (Sensor, Controller, Router) in a unified binary, selected during BLE provisioning. The system enables automated water tank monitoring and pump control via Zigbee mesh networking, with mobile app integration over BLE. Built on ESP-IDF v5.x, it emphasizes security, reliability, and low power for battery-operated sensors.

**Target Hardware:** ESP32-H2 (e.g., Mini module).
**Key Protocols:** BLE (provisioning/monitoring), Zigbee (HA profile for data exchange).
**Version:** 1.0.0 (post-bugfix).
**Status:** Production-ready after full bug resolution.

## Core Features
- **Unified Firmware Architecture:**
  - Single binary supports Sensor, Controller, and Router roles.
  - Role selected dynamically during BLE provisioning (no reflashing needed).
  - Shared hardware abstraction for GPIO, LEDs, and buttons across roles.

- **BLE Provisioning System:**
  - GATT-based configuration server for initial setup.
  - Custom characteristics for tank params, thresholds, network settings, and device name/location.
  - Secure connections: Requires pairing with MITM protection, 128-bit encryption, and bonding.
  - Unique default password generated from device MAC (e.g., "A1B2C3D4"); forces change on first setup.
  - Factory reset via command or 3-second button hold.
  - Thread-safe config access with mutex protection.

- **Zigbee Mesh Networking:**
  - ESP-Zigbee-SDK integration (HA profile).
  - Coordinator (Controller): Forms network, accepts joins (up to 10 children).
  - End Device (Sensor): Joins network, reports data every configurable interval (1-300s).
  - Router: Extends range, supports child devices.
  - Custom water level cluster (0xFC01): Attributes for % level, cm depth, sensor status, pump state.
  - Automatic channel/PAN ID configuration; retry limits (max 10) to prevent infinite loops.
  - Signal quality monitoring (RSSI-based).

- **Hardware Integration:**
  - GPIO configuration for ultrasonic sensor (trig/echo pins), relay (pump), LEDs (status/activity), and button.
  - LED feedback: Blinks for provisioning, errors, connections, pump state.
  - Button debounce (50ms) for reliable provisioning entry.
  - Watchdog timer (30s timeout) with task feeding to prevent hangs.

- **Persistent Storage:**
  - NVS for config (tank height/diameter, thresholds, network params, custom name, password).
  - Robust init with erase/reinit on corruption; error LED patterns on failure.
  - Provision timestamp and state tracking.

- **Status Monitoring:**
  - BLE broadcasting of real-time status (uptime, water level, pump state, manual override remaining, RSSI).
  - Mobile app can connect post-provisioning to view diagnostics without Zigbee access.

- **Manual Pump Override:**
  - BLE commands for timed pump start/stop (presets: 10-60 min).
  - 64-bit timer (esp_timer) for expiry (no 49-day overflow).
  - Clears on timeout or explicit stop; logs remaining time.

## Role-Specific Features

### Sensor Node (End Device)
- **Water Level Measurement:**
  - Ultrasonic sensor (HC-SR04 compatible) with 5-sample averaging for accuracy.
  - Calculates depth (cm) and percentage (%) based on configurable tank height/diameter/offset.
  - Handles invalid readings (timeouts, disconnected sensor) with status flags.
  - Division-by-zero protection for zero tank height.

- **Reporting:**
  - Periodic Zigbee reports (configurable 1-300s) of level, depth, and status.
  - Atomic attribute updates with locks to prevent race conditions during reads.

- **Power Management:**
  - Optimized polling with delays/task yields to avoid watchdog resets.
  - Light sleep compatible (future enhancement).

### Controller Node (Coordinator)
- **Pump Automation:**
  - Relay control based on water level thresholds (ON <20%, OFF >80%; configurable).
  - Auto-off on sensor timeout (30s) or max runtime (1-120 min, capped at 2h safety).
  - Logic validation: Ensures ON threshold < OFF threshold.

- **Data Reception:**
  - Listens for Zigbee reports; updates globals with locks.
  - Tracks sensor online/offline status and last update time.

- **Network Management:**
  - Forms Zigbee PAN; accepts device joins with announcements.
  - Steers end devices; monitors children.

### Router Node
- **Network Extension:**
  - Acts as Zigbee router to improve range/reliability.
  - Supports up to 10 children; keeps network alive.
  - Basic status LED for connectivity.

## Security Features
- **BLE Security (SEC #2):**
  - Mandatory pairing with Secure Connections (SC), MITM protection, and bonding.
  - 128-bit encryption keys; IO capability set to display-only for PIN.
  - No unauthenticated access; rejects unauthorized connections.

- **Password Management (SEC #1):**
  - No hardcoded defaults; unique per-device from MAC.
  - Flag forces password change on first provisioning.
  - Length validation (4-16 chars); optional disable.

- **Input Validation (SEC #3):**
  - All config params range-checked (e.g., tank 50-1000cm, thresholds 0-100%, channels 11-26).
  - Rejects illogical settings (e.g., ON >= OFF threshold).
  - Buffer overflows prevented with snprintf, null termination, and temp buffers.

- **Other:**
  - Stack buffer clearing in status responses (SEC #4).
  - Mutexes for shared config access to prevent races.

## Reliability & Error Handling
- **Error Recovery:**
  - NVS corruption: Auto-erase/reinit; fallback to defaults on fatal failure (error LED blink).
  - Zigbee join/formation: Retry up to 10x with 1s delays; error mode on max (infinite blink).
  - Sensor failures: Status flags, safe defaults (e.g., 0% on invalid height).

- **Thread Safety:**
  - Mutex for all `g_device_config` access (BLE callbacks, tasks, saves).
  - Zigbee locks for attribute updates.

- **Watchdog & Stability (BUG #10):**
  - 30s timeout; adds/feeds all tasks (sensor, control, Zigbee).
  - Delays/yields in tight loops (ultrasonic polling).

- **Overflow Protections:**
  - uint32_t for timeouts (BUG #3); esp_timer for manual overrides (BUG #8, 64-bit no wrap).
  - Bounds checks in name/password handling (BUG #6).

- **Button & Input Safety (BUG #11):**
  - 50ms debounce; 3s hold for provisioning.

- **Attribute Safety (BUG #9, #13):**
  - Atomic sets via `esp_zb_zcl_set_attribute_val`; proper status updates from Zigbee.

- **Testing Hooks:**
  - Comprehensive logging (levels: I/W/E); uptime/runtime tracking.
  - LED patterns for states (connected, error, provisioning).

## Future Enhancements
- **Power Optimization:** Deep sleep for sensors; PM config for H2.
- **OTA Updates:** BLE/Zigbee-based firmware upgrades with rollback.
- **Data Logging:** NVS history (24h water levels); app retrieval.
- **Advanced Controls:** Schedules, multiple thresholds, health monitoring (sensor reliability).
- **Diagnostics:** UART dumps, error codes via Morse LED.
- **Multilingual:** App-localized logs (English default).
- **Binding:** Direct sensor-controller Zigbee binding for faster response.

## Hardware Dependencies
- **Pins:** Trig/Relay (GPIO2), Echo (GPIO3), Status LED (GPIO8), Activity/Pump LED (GPIO9), Button (GPIO10).
- **Peripherals:** Ultrasonic (HC-SR04), Relay (5V pump), optional buzzer.
- **Power:** 3.3V; low-power modes for battery sensors.

## Build & Deployment
- **ESP-IDF:** v5.x with esp-zigbee-lib and esp-zboss-lib components.
- **Partitions:** Custom CSV for NVS/OTA.
- **Flash:** `idf.py -p PORT flash monitor`.
- **Roles:** Provision via app to set role; unified binary simplifies manufacturing.

**Generated:** December 3, 2025  
**Complete:** Yes (post all fixes). For updates, edit this file.
