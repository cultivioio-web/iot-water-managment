# Cultivio AquaSense - Phase 1 Deployment Checklist

## Pre-Deployment Checklist

### ✅ Hardware Verification

| Item | Check | Notes |
|------|-------|-------|
| □ | ESP32-H2 Mini (Sensor Node) | Verify board works, LED lights up |
| □ | ESP32-H2 Mini (Controller Node) | Verify board works, LED lights up |
| □ | HC-SR04 Ultrasonic Sensor | Test with hand at various distances |
| □ | 5V Relay Module | Click test - listen for relay sound |
| □ | Water Pump | Test separately with direct power |
| □ | LEDs (4x) + Resistors | Test each LED individually |
| □ | Push Buttons (2x) | Test button press detection |
| □ | Power Supplies (2x USB 5V) | Stable 5V output, ≥1A |
| □ | Wiring/Jumper Cables | No frayed wires, good connections |
| □ | Enclosures (optional) | Weather-resistant if outdoor |

### ✅ Firmware Verification

```bash
# Flash Sensor Node
cd firmware/sensor_node
idf.py -p COM3 flash monitor

# Flash Controller Node  
cd firmware/controller_node
idf.py -p COM4 flash monitor
```

| Item | Check | Notes |
|------|-------|-------|
| □ | Sensor Node flashed | See Cultivio banner on serial |
| □ | Controller Node flashed | See Cultivio banner on serial |
| □ | Both enter provisioning mode | LED blinking on first boot |
| □ | BLE advertising works | Device appears in app scan |

### ✅ Provisioning Test

| Item | Check | Notes |
|------|-------|-------|
| □ | App loads in Chrome/Edge | Open `provisioning-app/index.html` |
| □ | Can scan for devices | Both nodes appear as `Cultivio-XXXX` |
| □ | Can connect to Sensor Node | Status shows "Connected" |
| □ | Can connect to Controller Node | Status shows "Connected" |
| □ | Can configure tank settings | Height, diameter, offset |
| □ | Can configure pump thresholds | ON/OFF percentages |
| □ | Settings saved successfully | "Configuration saved" toast |
| □ | Devices restart after provisioning | Auto-restart after save |

### ✅ Zigbee Network Test

| Item | Check | Notes |
|------|-------|-------|
| □ | Controller forms network | "Network formed" in serial log |
| □ | Sensor joins network | "Successfully joined" in serial log |
| □ | Controller receives water level | Data appears in serial log |
| □ | Status LED solid on Controller | Indicates sensor connected |

### ✅ Functional Test (Bench Test)

| Item | Check | Notes |
|------|-------|-------|
| □ | Ultrasonic reads distances | Move hand, see % change in app |
| □ | Pump turns ON at low level | Simulate low water (cover sensor) |
| □ | Pump turns OFF at high level | Simulate high water (hand close) |
| □ | Manual override works | Start pump for 10min via app |
| □ | Manual stop works | Stop pump via app button |
| □ | Pump timeout works | Let run for configured timeout |
| □ | Status visible in app | Water %, pump status, uptime |

---

## Field Installation Checklist

### ✅ Site Preparation

| Item | Check | Notes |
|------|-------|-------|
| □ | Power outlet near controller | Within USB cable reach |
| □ | Power outlet near sensor (or long USB cable) | |
| □ | Tank access for sensor mounting | Top of tank accessible |
| □ | Pump electrical connection accessible | For relay wiring |
| □ | Clear line-of-sight for Zigbee | <30m between nodes |

### ✅ Sensor Node Installation

| Item | Check | Notes |
|------|-------|-------|
| □ | Measure actual tank height | Record: _____ cm |
| □ | Measure tank diameter | Record: _____ cm |
| □ | Mount sensor above water | Pointing straight down |
| □ | Measure sensor offset from top | Record: _____ cm |
| □ | Secure wiring | No loose connections |
| □ | Protect from direct water spray | Cover if needed |
| □ | Connect USB power | LED should blink |

### ✅ Controller Node Installation

| Item | Check | Notes |
|------|-------|-------|
| □ | Mount near pump | Within relay wire reach |
| □ | Wire relay to pump circuit | **POWER OFF FIRST!** |
| □ | Double-check relay wiring | COM/NO connections |
| □ | Secure all connections | Use terminal blocks |
| □ | Connect USB power | LED should blink |
| □ | Test pump manually | Via app before full auto |

### ✅ Configuration (On-Site)

| Item | Check | Notes |
|------|-------|-------|
| □ | Provision Sensor Node | Enter actual tank dimensions |
| □ | Provision Controller Node | Set appropriate thresholds |
| □ | **Set custom device name** | E.g., "Flat301" for easy identification |
| □ | **Change default password** | Default is "1234" - change it! |
| □ | **Enable password protection** | Required for multi-tenant buildings |
| □ | Set location info | E.g., "Building A, 3rd Floor" |
| □ | Verify water level reading | Compare with actual level |
| □ | Adjust sensor offset if needed | Re-provision if required |
| □ | Test pump ON threshold | Drain to trigger pump |
| □ | Test pump OFF threshold | Fill to stop pump |

### 🔐 Device Identification (For Multi-Device Buildings)

| Device | Custom Name | Password | Location |
|--------|-------------|----------|----------|
| Sensor | Cultivio-________ | ________ | ____________ |
| Controller | Cultivio-________ | ________ | ____________ |
| Router (if used) | Cultivio-________ | ________ | ____________ |

---

## Post-Installation Verification

### ✅ 24-Hour Monitoring

| Time | Water Level | Pump Status | Notes |
|------|-------------|-------------|-------|
| Install time | ___% | □ ON □ OFF | |
| +1 hour | ___% | □ ON □ OFF | |
| +4 hours | ___% | □ ON □ OFF | |
| +8 hours | ___% | □ ON □ OFF | |
| +24 hours | ___% | □ ON □ OFF | |

### ✅ Verify Automatic Operation

| Item | Check | Notes |
|------|-------|-------|
| □ | Pump activated automatically | When water dropped below threshold |
| □ | Pump stopped automatically | When water reached upper threshold |
| □ | No false triggers | Stable readings, no erratic behavior |
| □ | Network stayed connected | Check via app |

---

## Troubleshooting Quick Reference

### Sensor Node Issues

| Problem | Solution |
|---------|----------|
| No LED activity | Check USB power connection |
| Erratic readings | Check sensor mounting, remove obstructions |
| Always reads 0% or 100% | Verify sensor wiring (TRIG/ECHO pins) |
| Can't provision | Hold button 3 sec to force provisioning mode |
| Won't join network | Restart Controller first, then Sensor |

### Controller Node Issues

| Problem | Solution |
|---------|----------|
| No LED activity | Check USB power connection |
| Pump won't turn on | Check relay wiring, test relay manually |
| Pump runs continuously | Check OFF threshold, verify sensor data |
| Can't provision | Hold button 3 sec to force provisioning mode |
| Not receiving sensor data | Check Zigbee connection, restart both nodes |

### Network Issues

| Problem | Solution |
|---------|----------|
| Nodes won't connect | Restart Controller first, wait 30 sec, then Sensor |
| Frequent disconnections | Move nodes closer, reduce interference |
| Slow response | Normal - updates every 5 seconds |

### App Issues

| Problem | Solution |
|---------|----------|
| Can't scan for devices | Enable Bluetooth on phone, use Chrome/Edge |
| Device not found | Check device is powered, in BLE range (<10m) |
| Connection fails | Restart device, try scanning again |
| Status not updating | Check BLE connection, tap Refresh button |

---

## Factory Reset Procedure

If you need to completely reset a device:

```bash
# Connect device via USB and run:
cd firmware/sensor_node  # or controller_node
idf.py -p COMx erase-flash
idf.py -p COMx flash
```

Or simpler method:
1. Hold the **Provisioning Button** for **10 seconds**
2. All LEDs will flash rapidly
3. Device restarts in provisioning mode
4. Re-configure via mobile app

---

## Recommended Spare Parts

| Item | Quantity | Reason |
|------|----------|--------|
| ESP32-H2 Mini | 1 | Backup for either node |
| HC-SR04 Sensor | 1 | Most likely to fail outdoors |
| Relay Module | 1 | Backup for pump control |
| USB Cables | 2 | Wear and tear |
| Jumper Wires | 10 | For repairs |
| 220Ω Resistors | 5 | For LED replacements |

---

## Field Test Log

### Installation Details

| Field | Value |
|-------|-------|
| Installation Date | |
| Location | |
| Tank Type | |
| Tank Capacity (L) | |
| Pump Model | |
| Installer Name | |

### Configuration Values

| Parameter | Value |
|-----------|-------|
| Tank Height (cm) | |
| Tank Diameter (cm) | |
| Sensor Offset (cm) | |
| Pump ON Threshold (%) | |
| Pump OFF Threshold (%) | |
| Pump Timeout (min) | |
| Report Interval (sec) | |

### Test Results

| Test | Pass/Fail | Date | Notes |
|------|-----------|------|-------|
| Initial power-on | | | |
| BLE provisioning | | | |
| Zigbee pairing | | | |
| Water level accuracy | | | |
| Pump ON trigger | | | |
| Pump OFF trigger | | | |
| Manual override | | | |
| 24-hour stability | | | |

---

## Sign-Off

| Role | Name | Signature | Date |
|------|------|-----------|------|
| Installer | | | |
| Tester | | | |
| Customer | | | |

---

**Version:** 1.0  
**Document:** DEPLOYMENT_CHECKLIST.md  
**Product:** Cultivio AquaSense Phase 1

