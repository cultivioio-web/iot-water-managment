# Bill of Materials (BOM)

## Cultivio AquaSense - Phase 1

This document lists all components required to build the water management system.

---

## Quick Summary by Building Height

| Building | Nodes | Components | Est. Cost (INR) | Est. Cost (USD) |
|----------|-------|------------|-----------------|-----------------|
| **1-3 Story** | 2 | Sensor + Controller | ₹1,700-2,000 | $20-25 |
| **4-5 Story** | 3 | + Router Node | ₹2,200-2,500 | $26-30 |

### Configuration Guide

```
1-2 Story:  [Sensor] ←──────→ [Controller]     (2 nodes)
3 Story:    [Sensor] ←──────→ [Controller]     (2 nodes, test signal)
4-5 Story:  [Sensor] ←→ [Router] ←→ [Controller]  (3 nodes)
```

---

## Cost Summary by Configuration

### Basic Setup (2 Nodes) - For 1-3 Story Buildings

| Category | Items | INR | USD |
|----------|-------|-----|-----|
| Microcontrollers | 2x ESP32-H2 | ₹700 | $8-10 |
| Sensors | 1x HC-SR04 | ₹80 | $1-2 |
| Actuators | 1x Relay Module | ₹100 | $2-3 |
| Passive Components | LEDs, Resistors, Buttons | ₹100 | $1-2 |
| Connectors & Wire | Various | ₹200 | $2-3 |
| Power | 2x USB Adapters | ₹300 | $4-5 |
| Enclosures | 2x Junction Box | ₹300 | $4-5 |
| **Total** | | **₹1,780** | **$22-30** |

### Extended Setup (3 Nodes) - For 4-5 Story Buildings

| Additional Items | INR | USD |
|------------------|-----|-----|
| 1x ESP32-H2 Mini (Router) | ₹350 | $4-5 |
| 1x USB Adapter | ₹150 | $2 |
| 1x Junction Box (optional) | ₹150 | $2 |
| **Router Total** | **₹500-650** | **$6-9** |
| **Grand Total (3 nodes)** | **₹2,280-2,430** | **$28-39** |

---

## Detailed Bill of Materials

### 1. Microcontrollers

| # | Component | Description | Qty | INR | USD | Source |
|---|-----------|-------------|-----|-----|-----|--------|
| 1.1 | ESP32-H2 Mini | Zigbee/BLE SoC (Sensor) | 1 | ₹350 | $4-5 | AliExpress, Robu.in |
| 1.2 | ESP32-H2 Mini | Zigbee/BLE SoC (Controller) | 1 | ₹350 | $4-5 | AliExpress, Robu.in |
| 1.3 | ESP32-H2 Mini | Zigbee Router (4-5 story only) | 0-1 | ₹350 | $4-5 | AliExpress, Robu.in |

**Specifications:**
- IEEE 802.15.4 radio (Zigbee 3.0)
- Bluetooth 5 (LE)
- RISC-V single-core @ 96MHz
- 320KB SRAM, 4MB Flash
- USB-C connector
- Size: 25.4 x 18mm

**How Many Do You Need?**
| Building Height | ESP32-H2 Boards |
|-----------------|-----------------|
| 1-3 Story | 2 boards |
| 4-5 Story | 3 boards (includes router) |

**Alternatives:**
- ESP32-C6 Mini (adds WiFi support)
- Seeed XIAO ESP32-C6

---

### 2. Sensors

| # | Component | Description | Qty | Unit Price | Source |
|---|-----------|-------------|-----|------------|--------|
| 2.1 | HC-SR04 | Ultrasonic Distance Sensor | 1 | $2-4 | AliExpress, Amazon |

**Specifications:**
- Operating Voltage: 5V DC
- Operating Current: 15mA
- Range: 2cm - 400cm
- Resolution: 3mm
- Measuring Angle: 15°
- Trigger Input: 10μs TTL pulse
- Dimensions: 45 x 20 x 15mm

**Mounting Notes:**
- Waterproof the sensor housing
- Mount perpendicular to water surface
- Keep 5cm clearance from tank top

---

### 3. Actuators

| # | Component | Description | Qty | Unit Price | Source |
|---|-----------|-------------|-----|------------|--------|
| 3.1 | 5V Relay Module | 1-Channel Relay | 1 | $2-4 | AliExpress, Amazon |

**Specifications:**
- Coil Voltage: 5V DC
- Coil Current: 70-80mA
- Contact Rating: 10A @ 250VAC, 10A @ 30VDC
- Relay Type: SPDT (NO/NC/COM)
- Optocoupler isolated (recommended)
- Trigger: Active LOW or Active HIGH (check model)

**Safety Features to Look For:**
- Optocoupler isolation
- LED indicator
- Flyback diode included

---

### 4. LEDs & Indicators

| # | Component | Description | Qty | Unit Price | Source |
|---|-----------|-------------|-----|------------|--------|
| 4.1 | Blue LED | 5mm, Water Clear | 2 | $0.10 | Local electronics |
| 4.2 | White LED | 5mm, Water Clear | 1 | $0.10 | Local electronics |
| 4.3 | Red LED | 5mm, Water Clear | 1 | $0.10 | Local electronics |

**LED Specifications:**
- Forward Voltage: 2.0V (Red), 3.0-3.2V (Blue/White)
- Forward Current: 20mA max
- Luminous Intensity: 3000-6000mcd
- Viewing Angle: 30°

---

### 5. Passive Components

| # | Component | Description | Qty | Unit Price | Source |
|---|-----------|-------------|-----|------------|--------|
| 5.1 | 220Ω Resistor | 1/4W, 5% | 4 | $0.02 | Local electronics |
| 5.2 | Tactile Button | 6x6mm, Through-hole | 2 | $0.10 | Local electronics |
| 5.3 | Capacitor 100nF | Ceramic, Decoupling | 2 | $0.05 | Local electronics |
| 5.4 | Capacitor 10μF | Electrolytic, Power filter | 2 | $0.10 | Local electronics |

**Resistor Color Code (220Ω):**
```
Red - Red - Brown - Gold
 2  -  2  -  x10  - 5%
```

---

### 6. Connectors & Wiring

| # | Component | Description | Qty | Unit Price | Source |
|---|-----------|-------------|-----|------------|--------|
| 6.1 | USB-C Cable | For programming/power | 2 | $2-3 | Amazon |
| 6.2 | Dupont Wires | Male-Female, 20cm | 20 | $2/set | AliExpress |
| 6.3 | 22 AWG Wire | Solid core, assorted | 1m each | $3/set | Local electronics |
| 6.4 | Screw Terminal | 2-pin, 5.08mm pitch | 4 | $0.20 | Local electronics |
| 6.5 | Pin Header | 2.54mm, Male | 2 strips | $0.30 | Local electronics |

**Wire Color Convention:**
| Color | Purpose |
|-------|---------|
| Red | VCC/5V/3.3V |
| Black | GND |
| Yellow | Signal (TRIG) |
| Green | Signal (ECHO) |
| Blue | LED Status |
| White | LED Provisioning |
| Orange | Relay Control |
| Gray | Button |

---

### 7. Power Supply

| # | Component | Description | Qty | INR | USD | Source |
|---|-----------|-------------|-----|-----|-----|--------|
| 7.1 | USB Power Adapter | 5V 2A, USB-C | 2-3 | ₹150 ea | $2 | Amazon |
| 7.2 | USB-C Cable | 1-2m length | 2-3 | ₹100 ea | $1 | Amazon |
| 7.3 | *OR* 18650 Battery | 3.7V 3000mAh | 2-3 | ₹200 ea | $3 | Amazon |
| 7.4 | *OR* TP4056 Module | USB Charger + 5V Boost | 2-3 | ₹100 ea | $1.50 | AliExpress |

**Quantity by Building:**
| Building | USB Adapters | USB Cables |
|----------|--------------|------------|
| 1-3 Story | 2 | 2 |
| 4-5 Story | 3 | 3 |

**Option A: USB Power (Recommended)**
- Simple USB-C adapter
- Always-on power
- Most reliable for permanent installation

**Option B: Battery Power (For remote locations)**
- 18650 Li-ion cell
- TP4056 charging module
- 5V boost converter (MT3608)

---

### 8. Enclosures

| # | Component | Description | Qty | INR | USD | Source |
|---|-----------|-------------|-----|-----|-----|--------|
| 8.1 | Project Box | Weatherproof, Sensor | 1 | ₹150 | $2 | AliExpress, Robu.in |
| 8.2 | Project Box | ABS, Controller | 1 | ₹100 | $1.50 | AliExpress, Robu.in |
| 8.3 | Project Box | Small, Router (optional) | 0-1 | ₹100 | $1.50 | AliExpress |
| 8.4 | Cable Gland | PG7, Waterproof | 4 | ₹50 | $0.50 | Local electronics |
| 8.5 | Mounting Bracket | HC-SR04 holder | 1 | ₹50-100 | $1 | 3D Print / Amazon |

**Enclosure by Node:**
| Node | Location | Type | Weather Rating |
|------|----------|------|----------------|
| Sensor | Terrace/Tank top | IP65 Weatherproof | Required |
| Controller | Ground floor (indoor) | Basic ABS box | Optional |
| Router | Middle floor (indoor) | Basic ABS box | Optional |

**Enclosure Requirements:**
- Sensor Node: IP65 rated (splash proof, outdoor use)
- Controller/Router: Basic protection (indoor use)
- Room for ESP32 + components
- Ventilation holes (if sealed)
- Cable entry points

---

### 9. Pump System (User Supplied)

| # | Component | Description | Qty | Notes |
|---|-----------|-------------|-----|-------|
| 9.1 | Water Pump | 220V AC Submersible | 1 | Based on tank size |
| 9.2 | Circuit Breaker | 10A MCB | 1 | Required for safety |
| 9.3 | Electrical Wire | 14 AWG, 2-core | 5m | For pump connection |
| 9.4 | Pipe Fittings | As per installation | - | For water flow |

**Pump Selection Guide:**
| Tank Size | Recommended Pump | Flow Rate |
|-----------|------------------|-----------|
| < 500L | 0.25 HP | 30 L/min |
| 500-1000L | 0.5 HP | 50 L/min |
| > 1000L | 1 HP | 80+ L/min |

---

## Shopping Lists by Source

### For 1-3 Story Building (2 Nodes)

**AliExpress/Robu.in (~₹1,000 / $12)**
```
□ ESP32-H2 Mini x2
□ HC-SR04 Ultrasonic Sensor x1
□ 5V Relay Module x1
□ Dupont Wire Set x1
□ Project Box x2
```

**Local Electronics Store (~₹200 / $2)**
```
□ 5mm LEDs (Blue x2, White x1, Red x1)
□ 220Ω Resistors x4
□ Tactile Buttons 6x6mm x2
□ 100nF Capacitors x2
□ 10μF Capacitors x2
□ 22 AWG Wire (assorted colors)
□ Screw Terminals x4
□ Pin Headers
```

**Amazon/Local (~₹500 / $6)**
```
□ USB-C Cables x2
□ USB Power Adapters 5V 2A x2
□ Cable Glands PG7 x4
```

---

### For 4-5 Story Building (3 Nodes) - Add These

**Additional for Router Node (~₹500 / $6)**
```
□ ESP32-H2 Mini x1 (additional)
□ USB Power Adapter 5V 2A x1 (additional)
□ Small Project Box x1 (optional, indoor use)
□ 5mm LEDs x2 (Green, Blue)
□ 220Ω Resistors x2
```

### Complete Shopping List (4-5 Story)

**Online Order (~₹1,400 / $17)**
```
□ ESP32-H2 Mini x3 (Sensor + Controller + Router)
□ HC-SR04 Ultrasonic Sensor x1
□ 5V Relay Module x1
□ Dupont Wire Set x1
□ Project Box x3
```

**Local + Amazon (~₹800 / $10)**
```
□ 5mm LEDs (Blue x3, White x1, Red x1, Green x1)
□ 220Ω Resistors x6
□ Tactile Buttons 6x6mm x2
□ Capacitors (100nF x2, 10μF x2)
□ 22 AWG Wire (assorted)
□ Screw Terminals x4
□ USB-C Cables x3
□ USB Power Adapters 5V 2A x3
□ Cable Glands PG7 x4
```

---

## Tools Required

| Tool | Purpose | Essential? |
|------|---------|------------|
| Soldering Iron | Component assembly | Yes |
| Solder Wire | 60/40 or lead-free | Yes |
| Wire Strippers | Preparing cables | Yes |
| Multimeter | Testing connections | Yes |
| Screwdriver Set | Enclosure assembly | Yes |
| Hot Glue Gun | Waterproofing | Recommended |
| Drill | Enclosure holes | Recommended |
| 3D Printer | Custom brackets | Optional |

---

## Cost Breakdown by Node

### Sensor Node (Terrace)
| Item | INR | USD |
|------|-----|-----|
| ESP32-H2 Mini | ₹350 | $4-5 |
| HC-SR04 | ₹80 | $1-2 |
| LEDs + Resistors | ₹50 | $0.50 |
| Button | ₹10 | $0.10 |
| Wires + Connectors | ₹100 | $1 |
| USB Adapter + Cable | ₹200 | $3 |
| Enclosure (Weatherproof) | ₹150 | $2 |
| **Subtotal** | **₹940** | **~$12** |

### Controller Node (Ground Floor)
| Item | INR | USD |
|------|-----|-----|
| ESP32-H2 Mini | ₹350 | $4-5 |
| Relay Module | ₹100 | $2 |
| LEDs + Resistors | ₹50 | $0.50 |
| Button | ₹10 | $0.10 |
| Wires + Connectors | ₹100 | $1 |
| USB Adapter | ₹150 | $2 |
| Enclosure | ₹100 | $1.50 |
| **Subtotal** | **₹860** | **~$11** |

### Router Node (Middle Floor - Optional)
| Item | INR | USD |
|------|-----|-----|
| ESP32-H2 Mini | ₹350 | $4-5 |
| LEDs (status only) | ₹20 | $0.20 |
| USB Adapter | ₹150 | $2 |
| **Subtotal** | **₹520** | **~$6** |

---

## Total System Cost by Configuration

| Configuration | Nodes | INR | USD |
|---------------|-------|-----|-----|
| **1-3 Story** (Basic) | 2 | ₹1,800 | $23 |
| **4-5 Story** (Extended) | 3 | ₹2,320 | $29 |

*(Excluding pump, existing power outlets, and tools)*

---

## Supplier Links

### India 🇮🇳

**Robu.in (Recommended)**
- ESP32-H2 Mini: ~₹350
- HC-SR04: ~₹70-100
- Relay Module: ~₹80-120
- Free shipping above ₹500

**Amazon.in**
- ESP32-H2-DevKitM-1
- HC-SR04 Ultrasonic Sensor
- Relay Module 5V
- Fast delivery

**Other Indian Stores:**
- Evelta.com
- Electronicspices.com
- Probots.co.in
- Thingbits.net

### International

**AliExpress (Budget, 2-4 weeks shipping)**
- Search: "ESP32-H2 Mini"
- Search: "HC-SR04 Waterproof"
- Search: "5V Relay Module Optocoupler"

**Amazon (US/Global)**
- ESP32-H2-DevKitM-1
- HC-SR04 Ultrasonic Sensor
- SunFounder Relay Module

**Electronics Distributors:**
- Mouser/DigiKey (US)
- RS Components (Global)
- Element14 (India)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | Dec 2024 | Initial Phase 1 BOM |
| 1.1 | Dec 2024 | Added Router Node for 4-5 story buildings, INR pricing |

---

**Note:** Prices are approximate and may vary by region and supplier. Always buy a few extra components (especially LEDs and resistors) as spares.

