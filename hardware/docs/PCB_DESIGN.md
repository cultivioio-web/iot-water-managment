# 🔌 PCB Design Guide

## AquaSense Custom PCB Design for Production

---

## 📋 Overview

We need 3 custom PCBs:
1. **Sensor Node PCB** - ESP32-H2 + Ultrasonic sensor interface
2. **Controller Node PCB** - ESP32-H2 + Relay driver + Buzzer
3. **Router Node PCB** - ESP32-H2 only (range extender)

---

## 🎯 Design Specifications

### General Requirements

| Spec | Value |
|------|-------|
| PCB Layers | 2 (double-sided) |
| PCB Thickness | 1.6mm |
| Copper Weight | 1oz |
| Min Trace Width | 0.2mm (8mil) |
| Min Clearance | 0.2mm (8mil) |
| Via Size | 0.3mm hole, 0.6mm pad |
| Surface Finish | HASL (cheapest) or ENIG |
| Solder Mask | Green (cheapest) |
| Silkscreen | White |

### Board Dimensions

| Board | Size | Shape |
|-------|------|-------|
| Sensor Node | 45mm x 35mm | Rectangle |
| Controller Node | 55mm x 45mm | Rectangle |
| Router Node | 30mm x 25mm | Rectangle (compact!) |

---

## 📐 Sensor Node Schematic

### Block Diagram

```
                    ┌─────────────────────────────────────┐
                    │         SENSOR NODE PCB             │
                    │                                     │
  USB-C ───────────►│ 5V ──► AMS1117 ──► 3.3V             │
  (Power)           │         │                           │
                    │         ▼                           │
                    │    ┌─────────────┐                  │
                    │    │ ESP32-H2    │                  │
                    │    │   MINI-1    │◄── Button        │
                    │    │             │                  │
                    │    │  GPIO4 ─────┼──► Trig ──┐      │
                    │    │  GPIO5 ◄────┼─── Echo ◄─┤      │
                    │    │             │           │      │
                    │    │  GPIO8 ─────┼──► LED    │      │
                    │    │  GPIO9 ─────┼──► LED    │      │
                    │    └─────────────┘     ▼     │      │
                    │                    JSN-SR04T │      │
                    │                    Connector │      │
                    └─────────────────────────────────────┘
```

### Detailed Schematic

```
                                 USB-C
                                   │
                              ┌────┴────┐
                              │  VBUS   │
                              │   GND   │
                              │  D+     │ (Not used - power only)
                              │  D-     │
                              └────┬────┘
                                   │
                    ┌──────────────┼──────────────┐
                    │              │              │
                   ─┴─            ─┴─            ─┴─
                   ─┬─ C1         ─┬─ C2         ─┬─ C3
                    │  10µF        │  100nF       │  100nF
                    │              │              │
                   GND            GND            GND
                    
                              5V │
                                 ▼
                    ┌────────────────────────┐
                    │       AMS1117-3.3      │
                    │   IN          OUT      │───► 3.3V
                    │        GND             │
                    └───────────┬────────────┘
                                │
                               GND
                    
     3.3V ──────────────────────────────────────────────────
           │                                               │
           │    ┌───────────────────────────────────┐      │
           └───►│ VDD                           GND │◄─────┘
                │                                   │
        ────────│ EN ◄─── 10K ─── 3.3V (Pull-up)    │
                │                                   │
    Button ────►│ GPIO0 (Boot)    GPIO4 ────────────┼──► TRIG (JSN-SR04T)
                │                                   │
        ◄───────│ GPIO5 ◄───────────────────────────┼─── ECHO (JSN-SR04T)
                │                                   │
   LED Green ◄──│ GPIO8           GPIO9 ────────────┼──► LED Red
                │                                   │
                │        ESP32-H2-MINI-1            │
                └───────────────────────────────────┘
                
     JSN-SR04T Connector (4-pin):
     ┌─────────────┐
     │ VCC (5V)    │───► 5V from USB
     │ TRIG        │◄─── GPIO4
     │ ECHO        │───► GPIO5 (with voltage divider if needed)
     │ GND         │───► GND
     └─────────────┘
```

### Component List (Sensor Node)

| Ref | Component | Value/Part | Footprint |
|-----|-----------|------------|-----------|
| U1 | ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | Module |
| U2 | Voltage Regulator | AMS1117-3.3 | SOT-223 |
| J1 | USB-C Connector | USB-C 16P | SMD |
| J2 | Ultrasonic Connector | JST-XH 4P | Through-hole |
| C1 | Capacitor | 10µF/16V | 0805 |
| C2, C3 | Capacitor | 100nF | 0805 |
| C4 | Capacitor | 10µF/10V | 0805 |
| R1 | Resistor | 10K | 0805 |
| R2, R3 | Resistor | 330Ω | 0805 |
| LED1 | LED Green | 0805 | 0805 |
| LED2 | LED Red | 0805 | 0805 |
| SW1 | Button | 6x6mm | SMD |

---

## 📐 Controller Node Schematic

### Block Diagram

```
                    ┌─────────────────────────────────────────────┐
                    │           CONTROLLER NODE PCB               │
                    │                                             │
  USB-C ───────────►│ 5V ──► AMS1117 ──► 3.3V                     │
  (Power)           │  │                                          │
                    │  │     ┌─────────────┐                      │
                    │  │     │ ESP32-H2    │                      │
                    │  │     │   MINI-1    │◄── Button            │
                    │  │     │             │                      │
                    │  │     │  GPIO4 ─────┼──► Relay Driver ──► RELAY
                    │  │     │             │                      │   │
                    │  │     │  GPIO8 ─────┼──► LED (Status)      │   │
                    │  │     │  GPIO9 ─────┼──► LED (Pump)        │   ▼
                    │  │     │  GPIO10 ────┼──► Buzzer            │ PUMP
                    │  │     └─────────────┘                      │ OUT
                    │  │                                          │
                    │  └──────────────────────► 5V Relay Coil     │
                    └─────────────────────────────────────────────┘
```

### Detailed Schematic

```
                              USB-C
                                │
                           ┌────┴────┐
                           │  VBUS   │───► 5V
                           │   GND   │───► GND
                           └─────────┘
                    
        5V ────────────────────────────────────────────────
           │              │                               │
          ─┴─            ─┴─                              │
         ─┬─ C1         ─┬─ C2                            │
          │  10µF        │  100nF                         │
         GND            GND                               │
                                                          │
                    ┌───────────────────┐                 │
            5V ────►│ IN    AMS1117 OUT │───► 3.3V        │
                    │        GND        │                 │
                    └────────┬──────────┘                 │
                            GND                           │
                                                          │
    3.3V ─────────────────────────────────                │
           │                             │                │
           │    ┌───────────────────┐    │                │
           └───►│ VDD           GND │◄───┘                │
                │                   │                     │
        ────────│ EN                │                     │
                │                   │                     │
    Button ────►│ GPIO0             │                     │
                │                   │                     │
                │ GPIO4 ────────────┼───► To Relay Driver │
                │                   │                     │
   LED Green ◄──│ GPIO8             │                     │
                │                   │                     │
   LED Blue ◄───│ GPIO9             │                     │
                │                   │                     │
   Buzzer ◄─────│ GPIO10            │                     │
                │                   │                     │
                │  ESP32-H2-MINI-1  │                     │
                └───────────────────┘                     │
                                                          │
                                                          │
     RELAY DRIVER CIRCUIT:                                │
                                                          │
     GPIO4 ──────┬─────────────────────────────           │
                 │                                        │
                 │    1K                                  │
                 └───/\/\/───┐                            │
                             │                            │
                            ─┴─                           │
                           │   │ BC547                    │
                          E│   │C                         │
                           │   │                          │
                          GND  │                          │
                               │                          │
                               └──────┐                   │
                                      │                   │
                    5V ───────────────┤                   │
                                      │                   │
                               ┌──────┴──────┐            │
                               │             │            │
                               │    RELAY    │◄───────────┘
                               │  COIL (5V)  │
                               │             │
                               └──────┬──────┘
                                      │
                            ──────────┼────── 1N4007 (Flyback)
                           │          │          │
                           └──────────┴──────────┘
                                      │
                                     GND

     RELAY OUTPUT:
     ┌─────────────────────┐
     │    COM ─────────────┼──► To Pump (Live)
     │    NO  ─────────────┼──► From Mains
     │    NC  ─────────────┼──► (Not used)
     └─────────────────────┘
```

### Component List (Controller Node)

| Ref | Component | Value/Part | Footprint |
|-----|-----------|------------|-----------|
| U1 | ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | Module |
| U2 | Voltage Regulator | AMS1117-3.3 | SOT-223 |
| J1 | USB-C Connector | USB-C 16P | SMD |
| J2 | Relay Output | Screw Terminal 2P | Through-hole |
| J3 | Pump Input | Screw Terminal 2P | Through-hole |
| K1 | Relay | SRD-05VDC-SL-C | Through-hole |
| Q1 | Transistor | BC547 | TO-92 |
| D1 | Diode | 1N4007 | DO-41 |
| C1 | Capacitor | 10µF/16V | 0805 |
| C2, C3 | Capacitor | 100nF | 0805 |
| C4 | Capacitor | 10µF/10V | 0805 |
| R1 | Resistor | 10K | 0805 |
| R2 | Resistor | 1K | 0805 |
| R3, R4 | Resistor | 330Ω | 0805 |
| LED1 | LED Green | 0805 | 0805 |
| LED2 | LED Blue | 0805 | 0805 |
| BZ1 | Buzzer | Active 5V | Through-hole |
| SW1 | Button | 6x6mm | SMD |

---

## 🖥️ PCB Layout Guidelines

### Layer Stack

```
┌─────────────────────────────────────┐
│       TOP COPPER (Components)       │
├─────────────────────────────────────┤
│           FR4 SUBSTRATE             │
│              1.6mm                  │
├─────────────────────────────────────┤
│      BOTTOM COPPER (Ground)         │
└─────────────────────────────────────┘
```

### Layout Rules

#### 1. Power Distribution
```
┌───────────────────────────────────────────────┐
│                                               │
│  USB ──► 5V Rail (top) ──► Relay, Sensor      │
│           │                                   │
│           ▼                                   │
│       AMS1117                                 │
│           │                                   │
│           ▼                                   │
│       3.3V Rail ──► ESP32-H2, LEDs            │
│                                               │
│  GND Plane (bottom layer - solid copper)      │
│                                               │
└───────────────────────────────────────────────┘
```

#### 2. ESP32-H2 Module Placement
```
┌─────────────────────────────────────────┐
│                                         │
│   Keep antenna area clear!              │
│   ┌───────────────┐                     │
│   │               │ ◄── No copper/GND   │
│   │   ESP32-H2    │     under antenna   │
│   │               │                     │
│   │   [ANTENNA]   │ ◄── 5mm clearance   │
│   └───────────────┘                     │
│                                         │
└─────────────────────────────────────────┘
```

#### 3. Component Placement Priority
1. ESP32-H2 module (center)
2. USB-C connector (edge)
3. Decoupling caps (close to ICs)
4. Voltage regulator (near USB)
5. Connectors (edges)
6. LEDs (visible edge)
7. Relay (away from antenna)

#### 4. Trace Width Guidelines

| Signal Type | Width | Current |
|-------------|-------|---------|
| Power (5V, 3.3V) | 0.5mm | Up to 1A |
| Ground | 0.5mm+ | - |
| Signal (GPIO) | 0.2mm | <100mA |
| USB Data | 0.2mm | Differential |
| Relay coil | 0.4mm | 70mA |

### Antenna Considerations

```
                    ESP32-H2 Module
                    ┌─────────────────────┐
                    │                     │
                    │    ┌───────────┐    │
                    │    │  ANTENNA  │    │  ◄── PCB antenna
                    │    │   AREA    │    │      (built-in)
                    │    └───────────┘    │
                    │                     │
                    │                     │
                    │                     │
                    │   [Module Body]     │
                    │                     │
                    └─────────────────────┘
                    
     KEEP-OUT ZONE:
     ┌─────────────────────────────────────┐
     │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   │
     │  ░░░░░░░ NO COPPER HERE ░░░░░░░░░   │
     │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   │
     │  ░░░░░░░░░ 5mm around ░░░░░░░░░░░   │
     │  ░░░░░░░░░░ antenna ░░░░░░░░░░░░░   │
     └─────────────────────────────────────┘
```

---

## 📁 PCB Design Files

### Recommended EDA Tools

| Tool | Cost | Best For |
|------|------|----------|
| **KiCad** | Free | Best free option |
| **EasyEDA** | Free | Integrated with JLCPCB |
| **Altium Designer** | Paid | Professional |

### File Requirements for Manufacturing

```
Manufacturing Files (Gerber):
├── Top Copper (GTL)
├── Bottom Copper (GBL)
├── Top Solder Mask (GTS)
├── Bottom Solder Mask (GBS)
├── Top Silkscreen (GTO)
├── Bottom Silkscreen (GBO)
├── Drill File (DRL or XLN)
├── Board Outline (GKO or GML)
└── NC Drill Report (optional)

Assembly Files (for SMT):
├── BOM (Bill of Materials) - CSV
├── Pick & Place / Centroid - CSV
└── Assembly Drawing - PDF
```

---

## 🏭 Manufacturing Options

### JLCPCB (Recommended)

```
Order Settings:
- Base Material: FR-4
- Layers: 2
- Dimensions: 45x35mm (Sensor), 55x45mm (Controller)
- PCB Qty: 5 (minimum)
- Thickness: 1.6mm
- Color: Green
- Surface Finish: HASL
- Copper Weight: 1oz
- Min Track: 0.2mm
- Min Hole: 0.3mm

Price: ~$2 for 5 PCBs + $5-15 shipping
```

### Assembly Service (JLCPCB SMT)

```
SMT Assembly:
- Parts: They source from LCSC
- Setup Fee: $8
- Per Board: $0.5-2
- Total (10 boards): ~$30-50

Worth it if:
- You're ordering 10+ boards
- You don't have SMD soldering equipment
- You want professional quality
```

---

## 🔧 GPIO Pin Assignments

### Sensor Node

| GPIO | Function | Notes |
|------|----------|-------|
| GPIO0 | Boot Button | Pull-up, boot mode |
| GPIO4 | Ultrasonic TRIG | Output |
| GPIO5 | Ultrasonic ECHO | Input |
| GPIO8 | LED Green | Status |
| GPIO9 | LED Red | Error |
| GPIO10 | (Reserved) | Future |

### Controller Node

| GPIO | Function | Notes |
|------|----------|-------|
| GPIO0 | Boot Button | Pull-up, boot mode |
| GPIO4 | Relay Control | Output, active high |
| GPIO8 | LED Green | Status |
| GPIO9 | LED Blue | Pump state |
| GPIO10 | Buzzer | Alert sound |
| GPIO11 | (Reserved) | Future |

### Router Node

| GPIO | Function | Notes |
|------|----------|-------|
| GPIO0 | Boot Button | Pull-up, boot mode |
| GPIO8 | LED Green | Status/Heartbeat |
| GPIO9 | LED Yellow | Zigbee Activity |

---

## 📐 Router Node Schematic

### Block Diagram (Simplest!)

```
                    ┌─────────────────────────────┐
                    │      ROUTER NODE PCB        │
                    │         (30x25mm)           │
                    │                             │
  USB-C ───────────►│ 5V ──► AMS1117 ──► 3.3V    │
  (Power)           │                             │
                    │    ┌─────────────┐          │
                    │    │ ESP32-H2    │          │
                    │    │   MINI-1    │◄─ Button │
                    │    │             │          │
                    │    │  GPIO8 ─────┼──► LED   │
                    │    │  GPIO9 ─────┼──► LED   │
                    │    └─────────────┘          │
                    │                             │
                    └─────────────────────────────┘
```

### Component List (Router Node)

| Ref | Component | Value/Part | Footprint |
|-----|-----------|------------|-----------|
| U1 | ESP32-H2-MINI-1 | ESP32-H2-MINI-1-N4 | Module |
| U2 | Voltage Regulator | AMS1117-3.3 | SOT-223 |
| J1 | USB-C Connector | USB-C 16P | SMD |
| C1 | Capacitor | 10µF/16V | 0805 |
| C2, C3 | Capacitor | 100nF | 0805 |
| C4 | Capacitor | 10µF/10V | 0805 |
| R1 | Resistor | 10K | 0805 |
| R2, R3 | Resistor | 330Ω | 0805 |
| LED1 | LED Green | 0805 | 0805 |
| LED2 | LED Yellow | 0805 | 0805 |
| SW1 | Button | 6x6mm | SMD |

**Total Components**: 12 (cheapest node!)

---

## 🛡️ Protection Circuits

### ESD Protection (Recommended)

```
USB VBUS ──────┬────────────► 5V Rail
               │
              ─┴─
              ─┬─  TVS Diode
               │   (SMBJ5.0A)
              GND
```

### Reverse Polarity Protection

```
5V Input ────┤►├──────► 5V Rail
            1N5819
           (Schottky)
```

### Relay Contact Protection

```
                    RELAY
             ┌───────────────┐
   Pump ─────┤ COM        NO ├───┬───► Mains
   Live      └───────────────┘   │
                                 │
                                ─┴─  MOV
                                ─┬─  (07D471)
                                 │
                                GND
```

---

## 📝 Design Checklist

### Before Manufacturing
- [ ] Check ESP32-H2 module footprint matches datasheet
- [ ] Verify antenna keep-out zone
- [ ] Add decoupling caps near all ICs
- [ ] Ensure power trace width adequate
- [ ] Add mounting holes (M3, 3.2mm)
- [ ] Place test points for debugging
- [ ] Check silkscreen for component labels
- [ ] Run DRC (Design Rule Check)
- [ ] Generate Gerber files and preview

### Before Assembly
- [ ] Order extra components (10-20% spare)
- [ ] Have solder paste and stencil ready
- [ ] Test bare PCB for shorts
- [ ] Program ESP32-H2 before assembly (easier)

---

## 📦 Enclosure Design

### 3D Printable Enclosure

```
Sensor Node Enclosure:
┌────────────────────────────────┐
│  ╔════════════════════════╗    │
│  ║     Cable Entry        ║    │◄── Waterproof gland
│  ╚════════════════════════╝    │
│                                │
│  ┌──────────────────────────┐  │
│  │                          │  │
│  │         PCB              │  │
│  │                          │  │
│  └──────────────────────────┘  │
│                                │
│  ○ LED Window                  │
│                                │
│  [Button]                      │
│                                │
└────────────────────────────────┘
   └─────── USB-C ───────┘
```

### Material Recommendations

| Application | Material | Reason |
|-------------|----------|--------|
| Sensor Node | PETG | Water resistant |
| Controller Node | ABS | Heat resistant |
| Outdoor | ASA | UV resistant |

---

## 📞 Next Steps

1. **Download KiCad** - Free PCB design software
2. **Create schematic** - Based on diagrams above
3. **Layout PCB** - Follow guidelines
4. **Order from JLCPCB** - Upload Gerber files
5. **Assemble** - Solder components
6. **Test** - Flash firmware and verify

---

*See BOM_OPTIMIZATION.md for component sourcing*
*See ASSEMBLY_GUIDE.md for assembly instructions*

