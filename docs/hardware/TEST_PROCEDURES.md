# 🧪 Test Procedures

## Cultivio AquaSense Quality Assurance

---

## 📋 Test Overview

| Test Stage | When | Duration |
|------------|------|----------|
| Visual Inspection | After assembly | 2 min |
| Power Test | Before firmware | 1 min |
| Firmware Flash | After power test | 3 min |
| Functional Test | After firmware | 5 min |
| Burn-in Test | Before packaging | 30 min |
| Final Inspection | Before shipping | 2 min |

---

## 1️⃣ Visual Inspection

### Checklist

```
PCB Quality:
□ No visible damage or scratches
□ Silkscreen clearly printed
□ All pads clean and visible
□ Board edges smooth (no burrs)

Solder Quality:
□ All joints shiny and smooth
□ No solder bridges
□ No cold joints (dull/grainy)
□ No missing solder
□ No excess solder blobs

Component Placement:
□ All components present
□ Correct orientation (ICs, LEDs, diodes)
□ Components flat against PCB
□ No lifted pins

Physical:
□ No bent pins on connectors
□ USB-C port straight
□ Button clicks properly
□ Relay not damaged (controller)
```

### Pass/Fail Criteria

| Defect | Action |
|--------|--------|
| Missing component | Rework - add component |
| Solder bridge | Rework - remove with wick |
| Cold joint | Rework - reflow |
| Wrong orientation | Rework - replace component |
| PCB damage | REJECT - scrap board |

---

## 2️⃣ Power Test

### Equipment Needed

- USB-C cable
- 5V/2A USB power source
- Multimeter (DC voltage mode)
- Current-limited power supply (optional)

### Test Procedure

```
Step 1: Current Limit (if available)
- Set current limit to 100mA
- Connect power supply
- If current exceeds 100mA → FAIL (short circuit)

Step 2: Power On
- Connect USB-C to 5V source
- Wait 3 seconds
- Check for:
  □ No smoke
  □ No burning smell
  □ No hot components

Step 3: Voltage Measurements
```

| Test Point | Expected | Min | Max | Result |
|------------|----------|-----|-----|--------|
| VBUS (5V) | 5.0V | 4.75V | 5.25V | □ Pass |
| 3.3V Rail | 3.3V | 3.2V | 3.4V | □ Pass |
| GND | 0V | - | - | □ Pass |

```
Step 4: Current Draw (idle, no firmware)
```

| Node | Expected | Max |
|------|----------|-----|
| Sensor | 30-50mA | 100mA |
| Controller | 35-55mA | 120mA |
| Router | 25-45mA | 80mA |

### Power Test Form

```
╔═══════════════════════════════════════════════════════╗
║           POWER TEST RECORD                           ║
╠═══════════════════════════════════════════════════════╣
║ Serial Number: ________________                       ║
║ Node Type:     □ Sensor  □ Controller  □ Router       ║
║ Date:          ________________                       ║
║ Tester:        ________________                       ║
╠═══════════════════════════════════════════════════════╣
║ 5V Rail:       _______ V    □ Pass  □ Fail            ║
║ 3.3V Rail:     _______ V    □ Pass  □ Fail            ║
║ Current Draw:  _______ mA   □ Pass  □ Fail            ║
║ Visual Check:               □ Pass  □ Fail            ║
╠═══════════════════════════════════════════════════════╣
║ OVERALL:                    □ PASS  □ FAIL            ║
║ Notes: _________________________________________      ║
╚═══════════════════════════════════════════════════════╝
```

---

## 3️⃣ Firmware Flash Test

### Equipment Needed

- Computer with ESP-IDF installed
- USB-C cable (data capable)
- Firmware binary files

### Procedure

```
Step 1: Connect to Computer
- Connect USB-C
- Check COM port appears
- Note port number (e.g., COM3)

Step 2: Enter Flash Mode
- Hold BOOT button (GPIO0)
- Press and release RESET (or reconnect USB)
- Release BOOT button

Step 3: Flash Firmware
```

```powershell
# Flash command
idf.py -p COM3 flash

# Expected output:
# Connecting...
# Chip is ESP32-H2
# Writing at 0x00000000... (100 %)
# Hash of data verified.
```

```
Step 4: Verify Boot
- Press RESET or reconnect USB
- Monitor serial output:
```

```
Expected serial output:
------------------------------------
Cultivio AquaSense v1.0
Node Type: [SENSOR/CONTROLLER/ROUTER]
MAC: XX:XX:XX:XX:XX:XX
Starting BLE...
BLE advertising started
------------------------------------
```

### Flash Test Pass Criteria

| Check | Pass |
|-------|------|
| Firmware downloads successfully | □ |
| No flash errors | □ |
| Device reboots after flash | □ |
| Serial output correct | □ |
| BLE advertising starts | □ |

---

## 4️⃣ Functional Test

### 4.1 All Nodes - BLE Test

```
Step 1: BLE Advertising
- Power on device
- Open nRF Connect app (or Cultivio app)
- Scan for BLE devices
- "Cultivio-XXXX" should appear

Step 2: BLE Connection
- Connect to device
- Read services/characteristics
- Verify device responds

Step 3: Provisioning Test
- Open Cultivio provisioning app
- Connect to device
- Set device role
- Set custom name
- Save configuration
- Verify settings saved (reconnect and check)
```

### 4.2 Sensor Node - Specific Tests

```
Test A: Ultrasonic Sensor Reading
1. Connect JSN-SR04T sensor
2. Point sensor at flat surface (30cm away)
3. Read distance via BLE app
4. Expected: 28-32cm (±2cm accuracy)

5. Move hand to 15cm
6. Read distance
7. Expected: 13-17cm

Test B: Sensor at Various Distances

| Actual Distance | Reading | Pass (±5cm) |
|-----------------|---------|-------------|
| 10 cm | _____ cm | □ |
| 30 cm | _____ cm | □ |
| 50 cm | _____ cm | □ |
| 100 cm | _____ cm | □ |
| 150 cm | _____ cm | □ |

Test C: LED Indicators
- Green LED: Blinks at ~1Hz (heartbeat) □
- Red LED: Off in normal operation □
- Red LED: On when sensor error □

Test D: Button Function
- Short press: No action (or status refresh)
- Long press (10s): Factory reset, rapid LED flash
```

### 4.3 Controller Node - Specific Tests

```
Test A: Relay Operation
1. DO NOT connect pump yet!
2. Trigger manual pump ON via app
3. Listen for relay CLICK
4. Measure across NO-COM: should be closed (0Ω)
5. Trigger pump OFF
6. Relay clicks, NO-COM open (OL)

Test B: Relay Timing
| Duration Set | Actual Duration | Pass (±5%) |
|--------------|-----------------|------------|
| 1 minute | _____ sec | □ |
| 5 minutes | _____ sec | □ |
| 10 minutes | _____ sec | □ |

Test C: Buzzer Test
1. Trigger alert condition (low water simulation)
2. Buzzer should sound
3. Buzzer should stop when condition clears

Test D: LED Indicators
- Green LED: Blinks at ~1Hz □
- Blue LED: ON when pump running □
- Blue LED: OFF when pump stopped □

Test E: Button Function
- Short press: Toggle pump ON/OFF □
- Long press (10s): Factory reset □
```

### 4.4 Router Node - Specific Tests

```
Test A: Zigbee Router Function
1. Power on controller (coordinator)
2. Power on sensor (end device)
3. Verify they connect directly

4. Move sensor out of range
5. Power on router between them
6. Verify connection re-establishes through router

Test B: LED Indicators
- Green LED: Blinks at ~1Hz (heartbeat) □
- Yellow LED: Blinks on Zigbee traffic □

Test C: Range Extension
| Setup | Connection | Pass |
|-------|------------|------|
| Sensor-Controller direct (5m) | □ Yes □ No | □ |
| Sensor-Controller (15m, no router) | □ Yes □ No | □ |
| Sensor-Router-Controller (15m) | □ Yes □ No | □ |
```

---

## 5️⃣ Burn-In Test

### Purpose
Catch early failures (infant mortality)

### Procedure

```
Setup:
1. Power on all tested units
2. Connect sensors/leave relay cycling
3. Run for 30 minutes minimum

Monitor:
□ No units stopped working
□ No overheating (touch test <50°C)
□ LEDs still functioning
□ BLE still advertising
□ Readings still accurate (sensor)
□ Relay still clicking (controller)

After Burn-In:
□ Re-run quick functional test
□ Check for any degradation
```

### Burn-In Test Form

```
╔═══════════════════════════════════════════════════════╗
║           BURN-IN TEST RECORD                         ║
╠═══════════════════════════════════════════════════════╣
║ Batch Number: ________________                        ║
║ Units Tested: ________________                        ║
║ Start Time:   ________________                        ║
║ End Time:     ________________                        ║
║ Duration:     _______ minutes                         ║
╠═══════════════════════════════════════════════════════╣
║ Units Passed: _______                                 ║
║ Units Failed: _______                                 ║
║                                                       ║
║ Failed Unit Serial Numbers:                           ║
║ 1. ________________  Reason: ___________________      ║
║ 2. ________________  Reason: ___________________      ║
║ 3. ________________  Reason: ___________________      ║
╠═══════════════════════════════════════════════════════╣
║ Tester: ________________  Date: ________________      ║
╚═══════════════════════════════════════════════════════╝
```

---

## 6️⃣ Final Inspection

### Before Packaging Checklist

```
Physical:
□ PCB clean (no flux residue)
□ No visible damage
□ Enclosure fits properly
□ All screws tight
□ Labels applied (serial number)

Functional:
□ Powers on correctly
□ BLE advertising
□ Button works
□ LEDs work

Documentation:
□ Serial number recorded
□ Test records filed
□ Warranty activated
```

---

## 📊 Test Summary Report

### Daily Production Summary

```
╔═══════════════════════════════════════════════════════╗
║         DAILY PRODUCTION TEST SUMMARY                 ║
╠═══════════════════════════════════════════════════════╣
║ Date: ________________                                ║
╠═══════════════════════════════════════════════════════╣
║                    Sensor   Controller   Router       ║
║ Units Tested:      _____    _____        _____        ║
║ Units Passed:      _____    _____        _____        ║
║ Units Failed:      _____    _____        _____        ║
║ Pass Rate:         _____%   _____%       _____%       ║
╠═══════════════════════════════════════════════════════╣
║ Common Failures:                                      ║
║ 1. _______________________________________________    ║
║ 2. _______________________________________________    ║
║ 3. _______________________________________________    ║
╠═══════════════════════════════════════════════════════╣
║ Quality Manager: ________________                     ║
║ Signature: ________________                           ║
╚═══════════════════════════════════════════════════════╝
```

---

## 🔧 Test Equipment Calibration

### Required Calibration

| Equipment | Calibration Interval | Last Cal | Next Cal |
|-----------|---------------------|----------|----------|
| Multimeter | 12 months | ________ | ________ |
| Power Supply | 12 months | ________ | ________ |
| Distance Reference | 6 months | ________ | ________ |

---

*See ASSEMBLY_GUIDE.md for assembly instructions*
*See WIRING_DIAGRAMS.md for connection details*

