# Zigbee Water Management – User Guide

This guide walks through everything an installer or field engineer needs to
prepare, flash, configure, and maintain the three-node Zigbee water management
system that ships in this repository.

---

## 1. System Overview

- **Architecture:** three ESP32 nodes (sensor, main controller, backup
  controller) connected over Zigbee mesh. Each node runs the shared Arduino
  sketch `zigbee_water_management.ino` through the ESP-IDF/Arduino
  integration.
- **Purpose:** monitor tall storage tanks, prioritize government water, and
  switch to an emergency pump when needed. All decisions are local; the system
  keeps running without the internet once provisioning is complete.
- **Key roles:**
  | Role | Responsibilities | Key I/O |
  | --- | --- | --- |
  | Primary Sensor | Water level, govt-water sensing, status LEDs | Ultrasonic sensor, GOVT input, RGB LEDs |
  | Main Controller | Decision logic, emergency pump, alerts | Relays, buzzer, Zigbee endpoint |
  | Backup Controller | Failover control, alert ack, status mirror | Relays, button, LEDs |

Hardware wiring details live in `docs/WIRING_DIAGRAM.md`, and deployment
tips (especially for tall tanks) are in `docs/TALL_TANK_DEPLOYMENT.md`.

---

## 2. Software Prerequisites

1. **ESP-IDF 5.1.x** (or newer within 5.1 minor range). Run `idf.py --version`
   to confirm the environment is exported.
2. **Python 3.11 (bundled with ESP-IDF tools)** for `idf.py`.
3. **Git** to clone both this repo and the shared firmware tree.
4. **USB drivers** for the ESP32 board you are flashing.
5. (Optional) **VS Code + ESP-IDF extension** if you prefer an IDE.

> **Why Arduino-as-a-component?** The existing Zigbee logic was authored as an
> Arduino sketch. ESP-IDF’s component manager automatically pulls
> `espressif/arduino-esp32@^3.0.0`, exposing the Arduino APIs to the IDF
> build without copying code.

---

## 3. Repository Layout & Required Checkouts

```
├─ zigbee_water_management/        # Shared firmware sources (.ino, config.h, etc.)
├─ firmware/zigbee_idf/ (this project)
│  ├─ main/                        # IDF component that wraps the Arduino sketch
│  ├─ docs/                        # Additional deployment docs
│  └─ ...
```

`main/CMakeLists.txt` looks for the firmware in three locations:

1. `../zigbee_water_management` **(recommended – sibling checkout)**
Make sure one of those paths exists before building.

---

## 4. Installation & Build Steps

1. **Clone both repositories**
   ```bash
   git clone https://github.com/<org>/zigbee-water-management.git
   git clone https://github.com/<org>/zigbee_water_management.git  # sibling
   ```

2. **Install ESP-IDF 5.1**  
   Follow the official instructions: <https://docs.espressif.com/projects/esp-idf/en/v5.1/esp32/get-started/>

3. **Export the environment**
   ```bash
   cd ~/esp/esp-idf
   ./install.sh esp32
   . ./export.sh
   ```

4. **Install project dependencies** (optional manual pin)
   ```bash
   cd /path/to/zigbee-water-management
   idf.py add-dependency "espressif/arduino-esp32@^3.0.0"
   ```
   The dependency is declared in `main/idf_component.yml`, so `idf.py build`
   will fetch it automatically if you skip this step.

5. **Select the target & generate configs**
   ```bash
   idf.py set-target esp32
   idf.py menuconfig    # optional tweaks (see below)
   ```
   `sdkconfig.defaults` pins `CONFIG_FREERTOS_HZ=1000` (Arduino requirement).

6. **Build & flash**
   ```bash
   idf.py build
   idf.py flash monitor
   ```
   The first build will download ~27 managed components; expect a longer run.

---

## 5. Configuration Details

### 5.1 Hardware Pins & Constants

- `zigbee_water_management/config.h` enumerates every GPIO, default tank
  dimensions, timing, Zigbee PAN ID, and alert patterns.
- Adjust the pin map if you redesign the hardware harness, then rebuild.

### 5.2 Runtime Provisioning (HTTP Wizard)

1. Power each node; it creates the `WaterSystem-Setup` Wi-Fi AP.
2. Connect from a phone/laptop and browse to `http://192.168.4.1/setup`.
3. Assign the **role**, **season profile**, **tank geometry**, and **alert
   thresholds**. Repeat for each node.
4. Save & reboot; nodes drop into Zigbee-only operation.

### 5.3 ESP-IDF Menuconfig Tweaks

- **mbedTLS PSK ciphers:** Arduino’s NetworkClientSecure enables PSK-based
  suites. Run `idf.py menuconfig` → `Component config > mbedTLS > TLS Key
  Exchange Methods > Enable pre-shared-key ciphersuites` and ensure “Enable
  PSK based ciphersuite modes” is checked if you plan to use PSK TLS.
- **FreeRTOS tick rate:** keep `CONFIG_FREERTOS_HZ=1000`. This project already
  enforces it, but avoid downgrading to 100 Hz when editing `sdkconfig`.

### 5.4 Zigbee Network Parameters

- PAN ID (`ZIGBEE_PAN_ID`), channel mask, and endpoint IDs live in
  `config.h`/`zigbee_config.h`.
- Update them only if you have other Zigbee meshes nearby and need to avoid
  collisions.

---

## 6. Flashing & First Boot Checklist

1. Connect the ESP32 via USB, select the correct serial port (e.g.
   `/dev/ttyUSB0` / `COMx`).
2. Run `idf.py flash monitor` and wait for the boot log.
3. Confirm:
   - Tick rate warning is absent (should log `CONFIG_FREERTOS_HZ=1000`).
   - Arduino init completes (`setup()` runs and prints “WaterSystem setup
     portal started…”).
4. Perform HTTP provisioning (Section 5.2) for each node.
5. After reboot, verify Zigbee join messages and status LED behavior.

---

## 7. Maintenance & Updates

- **Updating firmware:** pull latest changes, rebuild, and `idf.py flash`.
- **Managed components:** versions are locked in `dependencies.lock`. Delete
  `managed_components/` if you want a clean re-sync.
- **Resetting configuration:** run `idf.py erase-flash` to wipe NVS, then
  re-provision.
- **Logs in the field:** connect a USB-to-UART cable and run
  `idf.py monitor` to stream logs.

---

## 8. FAQ & Troubleshooting

| Question | Answer |
| --- | --- |
| **Build fails: “Unable to locate zigbee firmware sources”** | Make sure the `zigbee_water_management` folder exists in one of the three supported paths (sibling, `firmware/`, or parent). `main/CMakeLists.txt` scans those directories and stops if the `.ino` file is missing. |
| **`espressif/arduino-esp32` not found** | Run `idf.py add-dependency "espressif/arduino-esp32@^3.0.0"` or delete `managed_components/` and re-run `idf.py build` so the component manager can download it. |
| **CMake complains about `CONFIG_FREERTOS_HZ`** | Arduino requires a 1 kHz tick. Keep `CONFIG_FREERTOS_HZ=1000` (already set via `sdkconfig.defaults`). Re-run `idf.py menuconfig` if the value was changed. |
| **`ESP_PARTITION_SUBTYPE_DATA_LITTLEFS` errors** | Make sure you’re on the latest commit; `CMakeLists.txt` now force-includes `main/arduino_compat.h`, which defines the missing enum for ESP-IDF 5.1. |
| **mbedTLS PSK warning during build** | Enable PSK ciphers in `menuconfig` as described in Section 5.3. It’s optional unless you use TLS clients relying on PSK suites. |
| **How do I change tank height/diameter later?** | Either re-run the HTTP setup wizard (preferred) or edit `config.h` defaults and reflash. |
| **How do I update Wi-Fi SSID/password for setup mode?** | Change `SETUP_WIFI_SSID` / `SETUP_WIFI_PASSWORD` in `config.h`, rebuild, and flash all nodes. |
| **Where do I find wiring diagrams?** | See `docs/WIRING_DIAGRAM.md` for each role’s pinout and relay isolation notes. |
| **How do I capture logs without stopping the system?** | Use `idf.py monitor` with a USB cable; the system keeps running while streaming logs. Press `Ctrl+]` to exit. |
| **Can I integrate with existing Zigbee coordinators?** | The project uses a custom cluster defined in `zigbee_config.*`. Update the cluster IDs or attribute tables there if you need third-party interoperability. |

---

## 9. Need More Detail?

- **Architecture & design rationale:** `docs/architecture_discussion.md`
- **Deployment tips for tall tanks:** `docs/TALL_TANK_DEPLOYMENT.md`
- **Implementation summary & feature matrix:** `docs/IMPLEMENTATION_SUMMARY.md`

For code contributions or bug reports, follow the standard Git workflow:
fork, branch, make changes, run `idf.py build`, and open a pull request.

