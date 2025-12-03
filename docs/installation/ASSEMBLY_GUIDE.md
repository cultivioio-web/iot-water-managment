# Assembly Guide

## Water Management System - Phase 1

Step-by-step instructions to build and assemble your IoT water management system.

---

## 📦 Before You Begin

### Verify Components
Check that you have all items from the [BOM](./BOM.md):

**Sensor Node:**
- [ ] 1x ESP32-H2 Mini
- [ ] 1x HC-SR04 Ultrasonic Sensor
- [ ] 1x Blue LED (Status)
- [ ] 1x White LED (Provisioning)
- [ ] 2x 220Ω Resistors
- [ ] 1x Tactile Button
- [ ] Dupont wires
- [ ] Enclosure

**Controller Node:**
- [ ] 1x ESP32-H2 Mini
- [ ] 1x 5V Relay Module
- [ ] 1x Blue LED (Status)
- [ ] 1x Red LED (Pump)
- [ ] 2x 220Ω Resistors
- [ ] 1x Tactile Button
- [ ] Dupont wires
- [ ] Enclosure

### Tools Needed
- [ ] Soldering iron + solder
- [ ] Wire strippers
- [ ] Multimeter
- [ ] Small screwdriver
- [ ] Hot glue gun (optional)
- [ ] Drill with small bits (for enclosure)

---

## 🔧 Sensor Node Assembly

### Step 1: Prepare the ESP32-H2 Mini

1. **Inspect the board** for any visible damage
2. **Test the board** by connecting USB-C and checking power LED
3. **Solder pin headers** if not pre-soldered (optional for breadboard testing)

```
Tip: Before soldering, test your setup on a breadboard first!
```

### Step 2: Wire the HC-SR04 Sensor

**Connection Table:**
| HC-SR04 Pin | ESP32-H2 Pin | Wire Color |
|-------------|--------------|------------|
| VCC | 5V | Red |
| GND | GND | Black |
| TRIG | GPIO2 | Yellow |
| ECHO | GPIO3 | Green |

**Assembly Steps:**

```
1. Cut 4 Dupont wires to length (~15-20cm)

2. Connect wires to HC-SR04:
   ┌─────────────┐
   │   HC-SR04   │
   │  ┌─┬─┬─┬─┐  │
   │  │V│T│E│G│  │
   │  │C│R│C│N│  │
   │  │C│I│H│D│  │
   │  └─┴─┴─┴─┘  │
   │   │ │ │ │   │
   └───┼─┼─┼─┼───┘
       │ │ │ │
      Red│ │Black
       Yellow│
          Green

3. Connect other ends to ESP32-H2:
   - Red → 5V pin
   - Black → GND pin
   - Yellow → GPIO2
   - Green → GPIO3

4. Test connection with multimeter (continuity check)
```

### Step 3: Add Status LEDs

**LED Wiring Diagram:**
```
                220Ω
GPIO8 ────────/\/\/\/\────────┤►├──────── GND
                             Blue LED
                             (Status)

                220Ω
GPIO9 ────────/\/\/\/\────────┤►├──────── GND
                             White LED
                             (Provisioning)
```

**Assembly Steps:**

```
1. Identify LED polarity:
   - Longer leg = Anode (+) = connects to resistor
   - Shorter leg = Cathode (-) = connects to GND
   - Flat side of LED casing = Cathode

2. Solder 220Ω resistor to GPIO8
   ┌───────┐
   │ESP32  │
   │GPIO8 ─┼─/\/\/\─┐
   │       │  220Ω  │
   │       │        ▼
   │       │      ┌─┴─┐
   │       │      │LED│ Blue
   │       │      └─┬─┘
   │  GND ─┼────────┘
   └───────┘

3. Repeat for GPIO9 with White LED

4. Test: Apply 3.3V to GPIO, LED should light up
```

### Step 4: Add Provisioning Button

**Button Connection:**
```
GPIO10 ───────────┬─────────── Button ─────────── GND
                  │
           (Internal pull-up)

Button is "Normally Open" - connects GPIO10 to GND when pressed
```

**Assembly Steps:**

```
1. Tactile button has 4 pins (2 pairs connected internally)
   
   ┌─────────┐
   │  ┌───┐  │
   │  │BTN│  │
   │  └───┘  │
   │ A     B │
   │ │     │ │
   │ A     B │
   └─────────┘
   
   Pins A-A are connected
   Pins B-B are connected
   Press button to connect A to B

2. Connect one side (A) to GPIO10
3. Connect other side (B) to GND
4. Test: Press button, GPIO10 should read LOW
```

### Step 5: Sensor Node Final Assembly

```
┌────────────────────────────────────────────────────────────┐
│                    SENSOR NODE COMPLETE                    │
├────────────────────────────────────────────────────────────┤
│                                                            │
│     ┌─────────────────────────────────────────────┐        │
│     │              ESP32-H2 Mini                  │        │
│     │    ┌───────────────────────────┐            │        │
│     │    │        USB-C              │            │        │
│     │    └───────────────────────────┘            │        │
│     │                                             │        │
│     │  3V3  5V  GND  GPIO2  GPIO3  GPIO8  GPIO9  GPIO10    │
│     │   │    │   │     │      │      │      │      │       │
│     └───┼────┼───┼─────┼──────┼──────┼──────┼──────┼───────┘
│         │    │   │     │      │      │      │      │       │
│         │    │   ├─────┼──────┼──────┴──────┴──────┤       │
│         │    │   │     │      │                    │       │
│         │   ┌┴───┴┐    │      │      ┌─────────────┴┐      │
│         │   │HC-SR│    │      │      │  LEDs +      │      │
│         │   │ 04  │    │      │      │  Button      │      │
│         │   └─────┘    │      │      └──────────────┘      │
│         │    Sensor    └──────┘                            │
│         │                                                  │
└─────────┴──────────────────────────────────────────────────┘
```

---

## 🔧 Controller Node Assembly

### Step 1: Prepare the ESP32-H2 Mini

Same as Sensor Node - inspect, test, and optionally solder headers.

### Step 2: Wire the Relay Module

**Connection Table:**
| Relay Pin | ESP32-H2 Pin | Wire Color |
|-----------|--------------|------------|
| VCC | 5V | Red |
| GND | GND | Black |
| IN | GPIO2 | Orange |

**Assembly Steps:**

```
1. Identify relay module pins:
   
   ┌─────────────────────────┐
   │       RELAY MODULE      │
   │  ┌───┐                  │
   │  │   │  ┌──┬──┬──┐      │
   │  │ R │  │NO│CM│NC│      │  High Voltage Side
   │  │ E │  └──┴──┴──┘      │  (Pump Connection)
   │  │ L │                  │
   │  │ A │  ┌──┬──┬──┐      │
   │  │ Y │  │VC│GN│IN│      │  Low Voltage Side
   │  └───┘  │C │D │  │      │  (ESP32 Connection)
   │         └──┴──┴──┘      │
   └─────────────────────────┘

2. Connect Low Voltage Side:
   - VCC → ESP32 5V (Red wire)
   - GND → ESP32 GND (Black wire)
   - IN → ESP32 GPIO2 (Orange wire)

3. DO NOT connect high voltage side yet!
   (We'll do that during installation)

4. Test: Send HIGH to GPIO2, relay should click
```

### Step 3: Add Status LEDs

**Same process as Sensor Node:**
```
GPIO8 → 220Ω → Blue LED → GND   (Status)
GPIO9 → 220Ω → Red LED → GND    (Pump Running)
```

### Step 4: Add Provisioning Button

```
GPIO10 → Button → GND
```

### Step 5: Controller Node Final Assembly

```
┌────────────────────────────────────────────────────────────┐
│                  CONTROLLER NODE COMPLETE                  │
├────────────────────────────────────────────────────────────┤
│                                                            │
│     ┌─────────────────────────────────────────────┐        │
│     │              ESP32-H2 Mini                  │        │
│     │    ┌───────────────────────────┐            │        │
│     │    │        USB-C              │            │        │
│     │    └───────────────────────────┘            │        │
│     │                                             │        │
│     │  3V3  5V  GND  GPIO2  GPIO8  GPIO9  GPIO10 │         │
│     │   │    │   │     │      │      │      │    │         │
│     └───┼────┼───┼─────┼──────┼──────┼──────┼────┘         │
│         │    │   │     │      │      │      │              │
│         │    │   ├─────┼──────┴──────┴──────┤              │
│         │    │   │     │                    │              │
│         │   ┌┴───┴┐    │    ┌───────────────┴┐             │
│         │   │RELAY│    │    │  LEDs +        │             │
│         │   │     │◄───┘    │  Button        │             │
│         │   │ VCC │         └────────────────┘             │
│         │   │ GND │                                        │
│         │   │ IN  │                                        │
│         │   │     │                                        │
│         │   │ NO──┼────────► To Pump (+)                   │
│         │   │ COM─┼────────► From AC Power                 │
│         │   │ NC  │ (unused)                               │
│         │   └─────┘                                        │
│         │                                                  │
└─────────┴──────────────────────────────────────────────────┘
```

---

## 📱 Firmware Installation

### Step 1: Install ESP-IDF

```bash
# Follow Espressif's guide:
# https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/get-started/

# Windows: Use ESP-IDF Tools Installer
# Linux/Mac: Use install.sh script
```

### Step 2: Build & Flash Sensor Node

```bash
# Open ESP-IDF terminal
cd C:\test\iot-water-managment\firmware\sensor_node

# Clean and set target
idf.py fullclean
idf.py set-target esp32h2

# Build
idf.py build

# Flash (replace COM3 with your port)
idf.py -p COM3 flash monitor
```

### Step 3: Build & Flash Controller Node

```bash
cd C:\test\iot-water-managment\firmware\controller_node

idf.py fullclean
idf.py set-target esp32h2
idf.py build
idf.py -p COM4 flash monitor
```

### Step 4: Provisioning

1. **Open provisioning app** in Chrome/Edge:
   ```
   File: provisioning-app/index.html
   ```

2. **Power on device** - LED will blink rapidly

3. **Click "Scan for Devices"** - Select WaterNode-XXXX

4. **Configure settings**:
   - Sensor: Tank height, diameter, offset
   - Controller: Pump thresholds, timeout

5. **Click "Save & Activate"**

6. **Repeat for second node**

---

## 🏠 Installation

### Sensor Node Placement

```
                    ┌─── ESP32 + LEDs + Button
                    │    (in weatherproof box)
                    │
              ┌─────┴─────┐
              │           │
    ══════════╪═══════════╪══════════  ← Tank Top
              │           │
              │  ┌─────┐  │
              │  │HC-SR│  │ ← Mount sensor 5cm below top
              │  │ 04  │  │   Point straight down
              │  └──▼──┘  │
              │     │     │
              │   Sound   │
              │   Waves   │
              │     │     │
              │     ▼     │
         ~~~~~│~~~~~~~~~~~│~~~~~  ← Water Level
              │           │
              │           │
              └───────────┘
```

**Installation Steps:**

1. **Mount sensor bracket** inside tank, near top
2. **Position HC-SR04** pointing straight down
3. **Run wires** through waterproof cable gland
4. **Mount enclosure** outside tank (dry area)
5. **Seal all openings** with silicone

### Controller Node Placement

```
     ┌────────────────────┐
     │   CONTROLLER NODE  │
     │  ┌──────────────┐  │
     │  │ ESP32-H2     │  │
     │  │ + Relay      │  │    ← Mount in dry location
     │  │ + LEDs       │  │       near pump
     │  └──────────────┘  │
     └────────┬───────────┘
              │
              │  Low Voltage (5V)
    ══════════╪══════════════════
              │  DANGER ZONE
              │  High Voltage (220V AC)
    ══════════╪══════════════════
              │
     ┌────────┴────────┐
     │   PUMP MOTOR    │
     │   220V AC       │
     └─────────────────┘
```

**Installation Steps:**

1. **Mount controller box** in dry, ventilated area
2. **Keep away from water** and humidity
3. **Connect relay to pump** (have electrician verify)
4. **Label all wires** clearly
5. **Install circuit breaker** before pump

---

## ⚡ Pump Wiring (CAUTION: HIGH VOLTAGE!)

⚠️ **WARNING: 220V AC can cause serious injury or death!**
⚠️ **Have a licensed electrician verify this installation!**

### Wiring Diagram

```
                    CIRCUIT BREAKER
   AC MAINS            (10A)              RELAY            PUMP
  ┌───────┐         ┌───────┐         ┌─────────┐      ┌───────┐
  │       │         │       │         │         │      │       │
  │   L ──┼────────►│  MCB  │────────►│   COM   │      │   M   │
  │       │         │       │         │         │      │       │
  │   N ──┼─────────┼───────┼─────────┼─────────┼─────►│       │
  │       │         │       │         │   NO ───┼─────►│       │
  │  PE ──┼─────────┼───────┼─────────┼─────────┼─────►│  PE   │
  │       │         │       │         │         │      │       │
  └───────┘         └───────┘         └─────────┘      └───────┘
  
  L = Live (Hot)
  N = Neutral
  PE = Protective Earth (Ground)
  MCB = Miniature Circuit Breaker
```

### Safety Checklist

Before powering on:
- [ ] All connections tight and insulated
- [ ] Circuit breaker installed and tested
- [ ] Proper wire gauge used (14 AWG minimum)
- [ ] Ground wire connected to pump housing
- [ ] No exposed wires or connections
- [ ] Enclosure properly sealed
- [ ] Licensed electrician has verified installation

---

## ✅ Testing Checklist

### Power On Test
- [ ] Sensor node powers on (USB connected)
- [ ] Controller node powers on
- [ ] LEDs indicate provisioning mode (fast blink)

### Provisioning Test
- [ ] Web app finds both devices via Bluetooth
- [ ] Settings save successfully
- [ ] Devices restart after provisioning

### Zigbee Communication Test
- [ ] Controller forms network (LED solid)
- [ ] Sensor joins network (3 quick blinks)
- [ ] Controller receives sensor data

### Sensor Test
- [ ] Water level readings are accurate
- [ ] Move hand over sensor - reading changes
- [ ] Serial monitor shows values

### Pump Control Test
- [ ] Lower water level below threshold
- [ ] Pump turns ON (LED on, relay clicks)
- [ ] Raise water level above threshold
- [ ] Pump turns OFF

### Safety Test
- [ ] Disconnect sensor → Pump turns OFF (safety)
- [ ] Timeout works → Pump turns OFF after limit

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| LED not lighting | Check polarity, verify resistor connection |
| Sensor no readings | Check TRIG/ECHO wiring, verify 5V power |
| Relay not clicking | Check IN pin, verify 5V power |
| No Bluetooth | Use Chrome/Edge, enable Bluetooth |
| Devices not pairing | Move closer, reset provisioning |
| Pump not starting | Check relay wiring, verify AC power |

---

## 🎉 Congratulations!

You've successfully assembled your water management system!

**Next Steps:**
1. Monitor the system for a few days
2. Adjust thresholds as needed via provisioning app
3. Add waterproofing for outdoor installation
4. Consider solar power for remote locations

---

**Document Version:** 1.0  
**Last Updated:** December 2024

