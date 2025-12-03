# 💰 BOM Optimization Guide

## Reducing AquaSense Production Costs

**Goal**: Reduce per-unit cost from ₹1,500 to ₹800-1,000

---

## 📊 Current vs Optimized BOM

### Current BOM (Using Dev Boards)

| Component | Current | Price | Total |
|-----------|---------|-------|-------|
| ESP32-H2 DevKit | 2 | ₹800 | ₹1,600 |
| HC-SR04 Ultrasonic | 1 | ₹80 | ₹80 |
| 5V Relay Module | 1 | ₹50 | ₹50 |
| Power Supply 5V | 2 | ₹100 | ₹200 |
| Enclosure | 2 | ₹80 | ₹160 |
| Wires, Connectors | 1 | ₹100 | ₹100 |
| Packaging | 1 | ₹50 | ₹50 |
| **Total** | | | **₹2,240** |

### Optimized BOM (Custom PCB)

| Component | Optimized | Price | Total |
|-----------|-----------|-------|-------|
| ESP32-H2-MINI-1 Module | 2 | ₹300 | ₹600 |
| Custom PCB (Sensor) | 1 | ₹50 | ₹50 |
| Custom PCB (Controller) | 1 | ₹60 | ₹60 |
| JSN-SR04T Waterproof Ultrasonic | 1 | ₹150 | ₹150 |
| 5V 1-Ch Relay (SRD-05VDC) | 1 | ₹25 | ₹25 |
| AMS1117-3.3V LDO | 2 | ₹5 | ₹10 |
| USB-C Connector | 2 | ₹10 | ₹20 |
| Capacitors, Resistors | Set | ₹30 | ₹30 |
| LEDs (Status) | 4 | ₹2 | ₹8 |
| Push Button | 2 | ₹3 | ₹6 |
| Enclosure (3D Printed) | 2 | ₹40 | ₹80 |
| Wires, Connectors | 1 | ₹50 | ₹50 |
| Packaging | 1 | ₹30 | ₹30 |
| **Total** | | | **₹1,119** |

### 💰 Savings: ₹1,121 per unit (50% reduction!)

---

## 🔧 Component-by-Component Optimization

### 1. ESP32-H2 Module Selection

| Option | Type | Price | Recommendation |
|--------|------|-------|----------------|
| ESP32-H2-DevKitM | Dev Board | ₹800 | ❌ Too expensive |
| ESP32-H2-MINI-1 | Module | ₹300 | ✅ Best for production |
| ESP32-H2-WROOM | Module | ₹350 | ✅ More GPIO pins |

**Winner**: ESP32-H2-MINI-1 (₹300)
- 4MB Flash
- Integrated antenna
- Small footprint (13.2 x 16.6 mm)

### 2. Ultrasonic Sensor

| Option | Type | Price | Waterproof | Range |
|--------|------|-------|------------|-------|
| HC-SR04 | Basic | ₹80 | ❌ | 2-400cm |
| JSN-SR04T | Waterproof | ₹150 | ✅ | 25-450cm |
| A02YYUW | Waterproof Serial | ₹300 | ✅ | 3-450cm |

**Winner for Tank**: JSN-SR04T (₹150)
- Waterproof probe (IP67)
- Handles humidity in tank
- Good range for 500L-2000L tanks

### 3. Relay Selection

| Option | Channels | Price | Current |
|--------|----------|-------|---------|
| SRD-05VDC-SL-C | 1 | ₹25 | 10A |
| HLS8L-DC5V-S-C | 1 | ₹35 | 15A |
| Module (blue board) | 1 | ₹50 | 10A |

**Winner**: SRD-05VDC-SL-C bare relay (₹25)
- Needs transistor driver circuit on PCB
- Saves ₹25 vs module

### 4. Power Supply

| Option | Price | Notes |
|--------|-------|-------|
| Hi-Link HLK-PM01 (5V/3W) | ₹120 | AC-DC, safe |
| USB-C + Phone Charger | ₹0 | User provides charger |
| 5V/2A Adapter included | ₹80 | Bulk price |

**Winner**: USB-C (user provides charger)
- Saves ₹80-120 per unit
- Every home has USB chargers
- Safer (no AC-DC on PCB)

### 5. Enclosure

| Option | Price | Pros | Cons |
|--------|-------|------|------|
| Injection Molded | ₹30 | Cheap at scale | ₹50K mold cost |
| 3D Printed | ₹40 | No mold cost | Slower |
| Off-shelf ABS box | ₹50 | Available now | Generic look |

**Winner (Initial)**: 3D Printed (₹40)
**Winner (Scale)**: Injection Molded (₹30 at 1000+ units)

---

## 📦 Optimized BOM by Volume

### 10 Units (Prototype)

| Component | Qty | Unit Price | Total |
|-----------|-----|------------|-------|
| ESP32-H2-MINI-1 | 20 | ₹350 | ₹7,000 |
| Custom PCB (JLCPCB) | 20 | ₹30 | ₹600 |
| JSN-SR04T | 10 | ₹180 | ₹1,800 |
| Relay + Components | 10 | ₹100 | ₹1,000 |
| Enclosure (3D Print) | 20 | ₹50 | ₹1,000 |
| Misc | 10 | ₹50 | ₹500 |
| **Total** | | | **₹11,900** |
| **Per Kit** | | | **₹1,190** |

### 100 Units (Small Batch)

| Component | Qty | Unit Price | Total |
|-----------|-----|------------|-------|
| ESP32-H2-MINI-1 (LCSC) | 200 | ₹280 | ₹56,000 |
| Custom PCB (JLCPCB) | 200 | ₹20 | ₹4,000 |
| JSN-SR04T (Alibaba) | 100 | ₹120 | ₹12,000 |
| Relay + Components | 100 | ₹60 | ₹6,000 |
| Enclosure (3D Print) | 200 | ₹35 | ₹7,000 |
| Misc | 100 | ₹40 | ₹4,000 |
| **Total** | | | **₹89,000** |
| **Per Kit** | | | **₹890** |

### 1000 Units (Production)

| Component | Qty | Unit Price | Total |
|-----------|-----|------------|-------|
| ESP32-H2-MINI-1 (Direct) | 2000 | ₹220 | ₹4,40,000 |
| Custom PCB (China) | 2000 | ₹12 | ₹24,000 |
| JSN-SR04T (Factory) | 1000 | ₹80 | ₹80,000 |
| Relay + Components | 1000 | ₹40 | ₹40,000 |
| Injection Mold Enclosure | 2000 | ₹25 | ₹50,000 |
| Misc | 1000 | ₹30 | ₹30,000 |
| **Total** | | | **₹6,64,000** |
| **Per Kit** | | | **₹664** |

---

## 🏭 Supplier Recommendations

### ESP32-H2 Modules

| Supplier | MOQ | Price/Unit | Lead Time |
|----------|-----|------------|-----------|
| **LCSC** | 1 | ₹300 | 10-15 days |
| **AliExpress** | 1 | ₹350 | 15-25 days |
| **Mouser India** | 1 | ₹450 | 3-5 days |
| **Espressif Direct** | 1000 | ₹200 | 30-45 days |

### PCB Manufacturing

| Supplier | MOQ | 10 PCBs | 100 PCBs |
|----------|-----|---------|----------|
| **JLCPCB** | 5 | ₹150 | ₹600 |
| **PCBWay** | 5 | ₹200 | ₹800 |
| **Lioncircuits (India)** | 1 | ₹500 | ₹2,000 |

**Winner**: JLCPCB (cheapest, 5-7 day delivery)

### Ultrasonic Sensors

| Supplier | MOQ | Price | Notes |
|----------|-----|-------|-------|
| **AliExpress** | 1 | ₹180 | 15-25 days |
| **Alibaba** | 50 | ₹100 | Bulk only |
| **Robu.in** | 1 | ₹250 | Fast delivery |

### Components (Resistors, Caps, LEDs)

| Supplier | Best For |
|----------|----------|
| **LCSC** | Bulk orders, SMD |
| **Robu.in** | Quick prototypes |
| **UTSource** | Hard-to-find parts |

---

## 📋 Final Optimized BOM (Per Kit)

### Sensor Node

| Component | Part Number | Qty | Price |
|-----------|-------------|-----|-------|
| ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | 1 | ₹280 |
| Sensor PCB | Custom | 1 | ₹20 |
| JSN-SR04T Ultrasonic | JSN-SR04T | 1 | ₹120 |
| AMS1117-3.3 LDO | AMS1117-3.3 | 1 | ₹3 |
| 10µF Capacitor | 0805 | 2 | ₹2 |
| 100nF Capacitor | 0805 | 3 | ₹1 |
| 10K Resistor | 0805 | 2 | ₹1 |
| Status LED (Green) | 0805 | 1 | ₹1 |
| Status LED (Red) | 0805 | 1 | ₹1 |
| Push Button | 6x6mm | 1 | ₹2 |
| USB-C Connector | 16-pin | 1 | ₹8 |
| Antenna (PCB) | - | 1 | ₹0 |
| Enclosure | 3D Print | 1 | ₹30 |
| **Subtotal** | | | **₹469** |

### Controller Node

| Component | Part Number | Qty | Price |
|-----------|-------------|-----|-------|
| ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | 1 | ₹280 |
| Controller PCB | Custom | 1 | ₹25 |
| Relay 5V/10A | SRD-05VDC-SL-C | 1 | ₹25 |
| BC547 Transistor | BC547 | 1 | ₹2 |
| 1N4007 Diode | 1N4007 | 1 | ₹1 |
| AMS1117-3.3 LDO | AMS1117-3.3 | 1 | ₹3 |
| 10µF Capacitor | 0805 | 2 | ₹2 |
| 100nF Capacitor | 0805 | 3 | ₹1 |
| 1K Resistor | 0805 | 2 | ₹1 |
| 10K Resistor | 0805 | 2 | ₹1 |
| Status LED (Green) | 0805 | 1 | ₹1 |
| Status LED (Blue) | 0805 | 1 | ₹1 |
| Buzzer (Optional) | 5V Active | 1 | ₹10 |
| Push Button | 6x6mm | 1 | ₹2 |
| USB-C Connector | 16-pin | 1 | ₹8 |
| Screw Terminal | 2-pin | 2 | ₹6 |
| Enclosure | 3D Print | 1 | ₹35 |
| **Subtotal** | | | **₹404** |

### Router Node (Simplest!)

| Component | Part Number | Qty | Price |
|-----------|-------------|-----|-------|
| ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | 1 | ₹280 |
| Router PCB | Custom (30x25mm) | 1 | ₹15 |
| AMS1117-3.3 LDO | AMS1117-3.3 | 1 | ₹3 |
| 10µF Capacitor | 0805 | 2 | ₹2 |
| 100nF Capacitor | 0805 | 2 | ₹1 |
| 10K Resistor | 0805 | 1 | ₹0.5 |
| 330Ω Resistor | 0805 | 2 | ₹1 |
| Status LED (Green) | 0805 | 1 | ₹1 |
| Activity LED (Yellow) | 0805 | 1 | ₹1 |
| Push Button | 6x6mm | 1 | ₹2 |
| USB-C Connector | 16-pin | 1 | ₹8 |
| Enclosure | 3D Print (tiny) | 1 | ₹20 |
| **Subtotal** | | | **₹335** |

### Cables & Accessories

| Component | Part | Qty | Price |
|-----------|------|-----|-------|
| JSN-SR04T Sensor | Waterproof ultrasonic | 1 | ₹150 |
| JST-XH 4P Cable | Pre-crimped 30cm | 1 | ₹15 |
| USB-C Cable 1m | Power cable | 2-3 | ₹50 each |
| Pump Wire 1.5mm² | 1 meter | 1 | ₹30 |
| Cable Gland PG7 | Sensor entry | 1 | ₹10 |
| Heat Shrink | Assorted | 1 | ₹20 |
| **Accessories Subtotal** | | | **₹325** |

### Screws & Mounting Hardware

| Component | Size | Qty | Price |
|-----------|------|-----|-------|
| PCB Mount Screws | M2x5mm self-tapping | 8 | ₹4 |
| Enclosure Screws | M2x8mm Phillips | 8 | ₹4 |
| Wall Mount Screws | M3x12mm Phillips | 8 | ₹8 |
| Wall Plugs | M3 Rawl plugs | 8 | ₹4 |
| Rubber Feet | 8mm self-adhesive | 8 | ₹8 |
| **Hardware Subtotal** | | | **₹28** |

### Packaging

| Item | Basic Kit | Extended Kit |
|------|-----------|--------------|
| Product Box (printed, matte + spot UV) | ₹35 | ₹40 |
| Internal Foam Tray | ₹10 | ₹12 |
| Quick Start Guide (A5, color) | ₹10 | ₹10 |
| Warranty Card | ₹5 | ₹5 |
| Serial Number Stickers | ₹4 | ₹6 |
| **Packaging Subtotal** | **₹64** | **₹73** |

### Kit Totals

| Kit Type | Items | Cost |
|----------|-------|------|
| **Basic Kit (2 nodes)** | | |
| Sensor Node PCB | 1 | ₹469 |
| Controller Node PCB | 1 | ₹404 |
| JSN-SR04T Sensor | 1 | ₹150 |
| JST-XH Cable | 1 | ₹15 |
| USB-C Cables | 2 | ₹100 |
| Pump Wire | 1 | ₹30 |
| Cable Gland + Heat Shrink | 1 | ₹30 |
| Screws & Mounting Hardware | Set | ₹28 |
| Packaging (box, tray, docs) | 1 | ₹64 |
| **Total COGS** | | **₹1,290** |
| **Selling Price** | | ₹2,999 |
| **Gross Margin** | | **₹1,709 (57%)** |
| | | |
| **Extended Kit (3 nodes)** | | |
| Sensor Node PCB | 1 | ₹469 |
| Controller Node PCB | 1 | ₹404 |
| Router Node PCB | 1 | ₹335 |
| JSN-SR04T Sensor | 1 | ₹150 |
| JST-XH Cable | 1 | ₹15 |
| USB-C Cables | 3 | ₹150 |
| Pump Wire | 1 | ₹30 |
| Cable Gland + Heat Shrink | 1 | ₹30 |
| Screws & Mounting Hardware | Set | ₹42 |
| Packaging (box, tray, docs) | 1 | ₹73 |
| **Total COGS** | | **₹1,698** |
| **Selling Price** | | ₹3,999 |
| **Gross Margin** | | **₹2,301 (58%)** |

---

## 🎯 Cost Reduction Roadmap

| Phase | Volume | COGS | Action |
|-------|--------|------|--------|
| Prototype | 10 | ₹1,200 | Use dev boards |
| Pilot | 100 | ₹900 | Custom PCB + modules |
| Launch | 500 | ₹800 | Bulk from LCSC |
| Scale | 1000+ | ₹650 | Direct from China |
| Mass | 5000+ | ₹500 | Contract manufacturing |

---

## 📞 Recommended Suppliers Contact

### PCB
- **JLCPCB**: jlcpcb.com (best price)
- **PCBWay**: pcbway.com (good quality)

### Components (Bulk)
- **LCSC**: lcsc.com
- **UTSource**: utsource.net
- **Alibaba**: alibaba.com (MOQ 100+)

### India (Fast)
- **Robu**: robu.in
- **Evelta**: evelta.com
- **QuartzComponents**: quartzcomponents.com

---

*Next: See PCB_DESIGN.md for schematic and layout*

