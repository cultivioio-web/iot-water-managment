# 🔧 Assembly Guide

## Cultivio AquaSense PCB Assembly Instructions

---

## 📋 Before You Start

### Required Tools

| Tool | Purpose | Price |
|------|---------|-------|
| Soldering Iron (temp controlled) | SMD soldering | ₹800-2000 |
| Fine tip (0.5-1mm) | Small components | Included |
| Solder wire (0.5mm, lead-free) | Connections | ₹150 |
| Flux pen or paste | Better flow | ₹200 |
| Tweezers (ESD safe) | Component handling | ₹100 |
| Magnifying glass/lamp | Inspection | ₹300 |
| Multimeter | Testing | ₹500 |
| Isopropyl alcohol (IPA) | Cleaning | ₹150 |
| Hot air station (optional) | Module soldering | ₹2000 |

### Required Materials

- Assembled PCBs (from JLCPCB)
- Through-hole components (relay, connectors)
- USB-C cable for power
- Test equipment

---

## 🔌 Sensor Node Assembly

### Step 1: Inspect PCB

```
Before soldering, check:
□ No scratches on copper traces
□ All pads visible and clean
□ Silkscreen readable
□ No shorts between pads (use multimeter)
```

### Step 2: SMD Components (if not pre-assembled)

**Solder in this order:**

1. **Smallest first** (resistors, capacitors)
2. **ICs** (AMS1117)
3. **ESP32-H2 module** (last - most expensive!)

```
Soldering SMD 0805:
1. Apply flux to pad
2. Tin ONE pad with small amount of solder
3. Hold component with tweezers
4. Reflow solder to attach one side
5. Solder other side
6. Reflow first side if needed
```

### Step 3: Through-Hole Components

| Component | Designator | Notes |
|-----------|------------|-------|
| JST-XH 4P Connector | J2 | Align notch correctly |

```
Soldering Through-Hole:
1. Insert component from TOP
2. Bend leads slightly to hold
3. Flip board
4. Solder from BOTTOM
5. Trim excess leads
```

### Step 4: USB-C Connector

```
⚠️ USB-C is tricky - many pins!

1. Align connector carefully
2. Tack ONE corner pin first
3. Check alignment
4. Solder remaining pins
5. Check for bridges with magnifier
6. Fix bridges with flux + solder wick
```

### Step 5: ESP32-H2 Module

```
Option A: Reflow with Hot Air
1. Apply solder paste to all pads
2. Place module carefully
3. Heat with hot air (280°C) until solder melts
4. Let cool, don't move!

Option B: Hand Solder
1. Tin all pads on PCB
2. Apply flux generously
3. Place module, align castellations
4. Heat each pad from the side
5. Solder should flow under module
```

### Step 6: Visual Inspection

```
Check each solder joint:
✓ Shiny and smooth
✓ Concave fillet shape
✓ No bridges between pins
✓ No cold joints (dull, grainy)
✓ No missing solder
```

### Step 7: Clean PCB

```
1. Apply IPA with brush
2. Scrub flux residue
3. Dry with lint-free cloth
4. Inspect under magnification
```

---

## 🎮 Controller Node Assembly

### Additional Components

| Component | Designator | Type |
|-----------|------------|------|
| Relay | K1 | Through-hole |
| Screw Terminals | J2, J3 | Through-hole |
| Transistor | Q1 | Through-hole (TO-92) |
| Diode | D1 | Through-hole (DO-41) |
| Buzzer | BZ1 | Through-hole |

### Step 1: SMD Components First

Same as Sensor Node:
1. Resistors & Capacitors
2. LEDs (note polarity!)
3. AMS1117
4. Button
5. USB-C
6. ESP32-H2 module

### Step 2: Through-Hole Components

**Solder in this order:**

```
1. Transistor (Q1 - BC547)
   - Flat side orientation per silkscreen
   - Don't overheat (3 sec max per pin)

2. Diode (D1 - 1N4007)
   - Band marking = cathode
   - Match silkscreen orientation!

3. Screw Terminals (J2, J3)
   - Wire entry facing board edge
   - Solder all pins

4. Buzzer (BZ1)
   - + marking on buzzer matches silkscreen
   - Press firmly while soldering

5. Relay (K1)
   - Last! It's the tallest
   - All 5 pins must be soldered
```

### Step 3: Relay Driver Circuit Check

```
Before powering on, verify:

Multimeter checks:
□ R2 (1K) between GPIO4 pad and Q1 base = ~1KΩ
□ D1 not shorted (diode mode: ~0.6V one way, OL other)
□ Q1 C-E not shorted (should be open)
□ No shorts between relay coil pins and 5V/GND
```

---

## 📡 Router Node Assembly

### Simplest Board!

Only SMD components:
1. Resistors (R1, R2, R3)
2. Capacitors (C1-C4)
3. LEDs (LED1, LED2)
4. AMS1117 (U2)
5. Button (SW1)
6. USB-C (J1)
7. ESP32-H2 (U1)

No through-hole components!

---

## ⚡ Power-On Testing

### First Power Test (All Nodes)

```
⚠️ IMPORTANT: Do this BEFORE connecting sensor/pump!

1. Connect USB-C cable to phone charger (5V/1A minimum)

2. Observe:
   □ No smoke or burning smell
   □ No components getting hot
   □ Power LED should light up

3. Measure voltages:
   □ 5V rail: 4.8-5.2V ✓
   □ 3.3V rail: 3.2-3.4V ✓
   
4. If anything wrong:
   - Disconnect immediately!
   - Check for shorts
   - Check component orientation
```

### LED Behavior (After Firmware Flash)

| Node | LED1 (Green) | LED2 |
|------|--------------|------|
| Sensor | Heartbeat (1Hz) | Red = Error |
| Controller | Heartbeat (1Hz) | Blue = Pump ON |
| Router | Heartbeat (1Hz) | Yellow = Activity |

---

## 🔗 Connector Assembly

### Sensor Node - Ultrasonic Sensor

```
JST-XH 4P Connector Pinout:
┌─────────────────────┐
│ 1   2   3   4       │
│ VCC TRIG ECHO GND   │
└─────────────────────┘

Wire Colors (JSN-SR04T):
- Red    → Pin 1 (VCC/5V)
- Yellow → Pin 2 (TRIG)
- Green  → Pin 3 (ECHO)
- Black  → Pin 4 (GND)
```

### Controller Node - Pump Connection

```
Screw Terminal Wiring:
┌───────────────────────────────────┐
│                                   │
│   J2 (OUTPUT)      J3 (INPUT)     │
│   ┌─────────┐      ┌─────────┐    │
│   │ COM  NO │      │ L    N  │    │
│   └─────────┘      └─────────┘    │
│                                   │
└───────────────────────────────────┘

Wiring:
- J3-L: Live wire from mains (through MCB)
- J3-N: Neutral (if using both)
- J2-COM: To pump motor (Live)
- J2-NO: From J3-L (switched)

⚠️ WARNING: 230V AC! Use proper safety!
```

---

## 🧪 Functional Testing

### Sensor Node Test

```
Test Sequence:
1. Power on via USB
2. Wait for BLE advertising (LED blinks)
3. Connect via mobile app
4. Check "Cultivio-XXXX" appears

5. Test ultrasonic sensor:
   - Point sensor at flat surface
   - Distance should read in app
   - Move hand closer/further
   - Reading should change

6. Test button:
   - Hold button 10 sec = Factory reset
   - LED should flash rapidly
```

### Controller Node Test

```
Test Sequence:
1. Power on via USB
2. Wait for BLE advertising
3. Connect via mobile app

4. Test relay (NO PUMP CONNECTED):
   - Trigger manual pump ON in app
   - You should hear relay CLICK
   - LED2 turns blue
   - After timeout, relay clicks OFF

5. Test buzzer:
   - Trigger alert condition
   - Buzzer should sound

6. Test button:
   - Press = Manual pump toggle
   - Hold 10 sec = Factory reset
```

### Router Node Test

```
Test Sequence:
1. Power on via USB
2. Green LED = Heartbeat (1Hz blink)
3. Yellow LED = Zigbee activity

4. Range test:
   - Place between sensor and controller
   - Verify mesh network forms
   - Signal strength should improve
```

---

## ❌ Troubleshooting

### No Power LED

| Check | Solution |
|-------|----------|
| USB cable | Try different cable |
| USB charger | Use 5V/1A+ charger |
| USB-C solder | Check for cold joints |
| AMS1117 | Check orientation, solder |
| Capacitor shorts | Check for bridges |

### ESP32 Not Booting

| Check | Solution |
|-------|----------|
| 3.3V rail | Should be 3.2-3.4V |
| EN pin | Should be pulled high |
| GPIO0 | Should be high (not held low) |
| Module solder | Reflow if needed |

### Relay Not Clicking

| Check | Solution |
|-------|----------|
| 5V at relay coil | Measure with multimeter |
| Transistor | Check orientation (flat side) |
| Base resistor | Should be ~1KΩ |
| Diode | Check orientation (band = cathode) |
| GPIO4 output | Measure voltage when triggered |

### Sensor Not Reading

| Check | Solution |
|-------|----------|
| 5V at sensor | Check JST connector |
| TRIG signal | Should pulse when measuring |
| ECHO signal | Should respond to TRIG |
| Wiring | Verify pin order |
| Sensor | Try different sensor |

---

## ✅ Final Quality Checklist

### Before Shipping/Installing

```
□ Visual inspection passed
□ No flux residue
□ All solder joints good
□ Power test passed (5V, 3.3V)
□ BLE advertising works
□ Firmware flashed and verified
□ Sensor reading correct (if sensor node)
□ Relay clicking (if controller node)
□ Button functions work
□ LEDs indicate correctly
□ Enclosure fits properly
□ Cables connected and tested
```

---

## 📦 Packaging

### Kit Contents Checklist

**Basic Kit (2 nodes):**
```
□ 1x Sensor Node (assembled, tested)
□ 1x Controller Node (assembled, tested)
□ 1x JSN-SR04T Ultrasonic Sensor
□ 1x Sensor cable (pre-wired JST-XH)
□ 2x USB-C cables (1m)
□ 1x Quick Start Guide
□ 1x Warranty Card
```

**Extended Kit (3 nodes):**
```
□ All Basic Kit items
□ 1x Router Node (assembled, tested)
□ 1x Extra USB-C cable
```

---

*See WIRING_DIAGRAMS.md for detailed connection diagrams*
*See TEST_PROCEDURES.md for complete QA checklist*

