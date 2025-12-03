# 🔧 Hardware Design Files

## Cultivio AquaSense Hardware

---

## 📁 Directory Structure

```
hardware/
├── kicad/                    # PCB Design Files
│   ├── sensor_node/          # Sensor node KiCad project
│   ├── controller_node/      # Controller node KiCad project
│   ├── router_node/          # Router node KiCad project
│   ├── libraries/            # Custom component libraries
│   └── README.md             # KiCad usage guide
│
├── enclosure/                # 3D Printable Enclosures
│   ├── sensor_node_enclosure.scad      # Sensor enclosure (OpenSCAD)
│   ├── controller_node_enclosure.scad  # Controller enclosure (OpenSCAD)
│   ├── router_node_enclosure.scad      # Router enclosure (OpenSCAD)
│   └── README.md             # 3D printing guide
│
├── manufacturing/            # Production Files
│   ├── sensor_node_bom.csv   # BOM for JLCPCB
│   ├── sensor_node_cpl.csv   # Component placement
│   ├── controller_node_bom.csv
│   ├── controller_node_cpl.csv
│   ├── router_node_bom.csv
│   ├── router_node_cpl.csv
│   ├── kit_accessories_bom.csv  # Cables, screws, packaging
│   └── README.md             # Manufacturing guide
│
├── packaging/                # Product Box Design
│   └── product_box_design.md # Retail packaging specs & artwork
│
└── README.md                 # This file
```

---

## 📐 Quick Specs

### Sensor Node PCB

| Spec | Value |
|------|-------|
| Dimensions | 45mm x 35mm |
| Layers | 2 |
| Main IC | ESP32-H2-MINI-1 |
| Sensor | JSN-SR04T (waterproof ultrasonic) |
| Power | USB-C (5V) |
| Estimated Cost | ₹470 |

### Controller Node PCB

| Spec | Value |
|------|-------|
| Dimensions | 55mm x 45mm |
| Layers | 2 |
| Main IC | ESP32-H2-MINI-1 |
| Output | 10A Relay (SRD-05VDC) |
| Power | USB-C (5V) |
| Estimated Cost | ₹400 |

### Router Node PCB (Range Extender)

| Spec | Value |
|------|-------|
| Dimensions | 30mm x 25mm (compact!) |
| Layers | 2 |
| Main IC | ESP32-H2-MINI-1 |
| Function | Zigbee mesh router |
| Power | USB-C (5V) |
| Estimated Cost | ₹335 |

---

## 🚀 Getting Started

### 1. Design PCBs

```bash
# Install KiCad
# Open: hardware/kicad/sensor_node/sensor_node.kicad_pro
# Follow: hardware/kicad/README.md
```

### 2. Order PCBs

```bash
# Generate Gerber files from KiCad
# Upload to JLCPCB.com
# Follow: docs/hardware/JLCPCB_ORDER_GUIDE.md
```

### 3. Print Enclosures

```bash
# Install OpenSCAD
# Open: hardware/enclosure/sensor_node_enclosure.scad
# Export STL and print
# Follow: hardware/enclosure/README.md
```

### 4. Order Packaging

```bash
# Review: hardware/packaging/product_box_design.md
# Get quotes from local printing press
# MOQ: 100 boxes for best pricing
```

---

## 📦 Product Packaging

| Kit | Box Size | Cost |
|-----|----------|------|
| Basic (2 nodes) | 200×150×80mm | ₹35 |
| Extended (3 nodes) | 250×180×100mm | ₹40 |

**Box includes:** Matte finish, Spot UV logo, Internal foam tray

---

## 📊 Cost Summary (Per Kit)

### BOM (Materials Only)

| Component | Prototype | Production (500+) |
|-----------|-----------|-------------------|
| **Sensor Node** | | |
| PCB + Components | ₹520 | ₹400 |
| Enclosure | ₹50 | ₹30 |
| **Controller Node** | | |
| PCB + Components | ₹460 | ₹380 |
| Enclosure | ₹60 | ₹35 |
| **Router Node** | | |
| PCB + Components | ₹350 | ₹285 |
| Enclosure | ₹30 | ₹20 |
| **Accessories** | | |
| Sensor, Cables, Screws | ₹330 | ₹280 |
| Packaging | ₹64 | ₹50 |

### Total Production Cost (incl. Labor, QC, Overhead)

| Kit | BOM | Total Cost | MRP | Margin |
|-----|-----|------------|-----|--------|
| **Basic (2 nodes)** | ₹1,290 | ₹1,531 | ₹2,999 | **49%** |
| **Extended (3 nodes)** | ₹1,698 | ₹1,989 | ₹3,999 | **50%** |

*See `docs/business/PRODUCTION_COST_ANALYSIS.md` for detailed breakdown*

---

## 🔗 Related Documentation

| Document | Location |
|----------|----------|
| BOM Optimization | `docs/hardware/BOM_OPTIMIZATION.md` |
| PCB Design Guide | `docs/hardware/PCB_DESIGN.md` |
| JLCPCB Order Guide | `docs/hardware/JLCPCB_ORDER_GUIDE.md` |
| Assembly Guide | `docs/hardware/ASSEMBLY_GUIDE.md` |
| Test Procedures | `docs/hardware/TEST_PROCEDURES.md` |
| Wiring Diagrams | `docs/hardware/WIRING_DIAGRAMS.md` |
| **Packaging Design** | `hardware/packaging/product_box_design.md` |
| **Production Costs** | `docs/business/PRODUCTION_COST_ANALYSIS.md` |

---

## 📞 Support

- **KiCad Forum**: https://forum.kicad.info/
- **OpenSCAD Forum**: https://forum.openscad.org/
- **Cultivio Support**: support@cultivio.io

---

*© 2025 Cultivio Technologies*

