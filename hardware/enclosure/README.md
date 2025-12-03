# 📦 3D Printable Enclosures

## Cultivio AquaSense Enclosure Designs

---

## 📁 Files

| File | Description | Size |
|------|-------------|------|
| `sensor_node_enclosure.scad` | Sensor node case (OpenSCAD) | 45x35mm PCB |
| `controller_node_enclosure.scad` | Controller case (OpenSCAD) | 55x45mm PCB |
| `router_node_enclosure.scad` | Router/repeater case (OpenSCAD) | 30x25mm PCB |

---

## 🔧 Software Required

### OpenSCAD (Free)
Download: https://openscad.org/downloads.html

OpenSCAD is a parametric 3D CAD modeler - perfect for technical enclosures.

---

## 🚀 Quick Start

### Generate STL Files

1. **Open OpenSCAD**
2. **Load file**: File → Open → `sensor_node_enclosure.scad`
3. **Preview**: Press F5
4. **Render**: Press F6 (wait for completion)
5. **Export STL**: Press F7 or File → Export → STL

### Export Each Part

**For Sensor Node:**
```
// Comment out the visualization, uncomment export line:
// bottom_case();    ← Uncomment this, export as sensor_bottom.stl
// top_case();       ← Uncomment this, export as sensor_top.stl
```

**For Controller Node:**
```
// bottom_case();       ← Export as controller_bottom.stl
// top_case();          ← Export as controller_top.stl
// wall_mount_bracket(); ← Export as wall_bracket.stl (optional)
```

---

## 🖨️ Print Settings

### Sensor Node Enclosure

| Setting | Value |
|---------|-------|
| **Material** | PETG (water resistant) |
| **Layer Height** | 0.2mm |
| **Infill** | 20% |
| **Supports** | No (designed without overhangs) |
| **Brim** | Optional |
| **Print Time** | ~2 hours |
| **Filament** | ~25g |

### Controller Node Enclosure

| Setting | Value |
|---------|-------|
| **Material** | ABS or PETG |
| **Layer Height** | 0.2mm |
| **Infill** | 25% (more for heat dissipation) |
| **Supports** | Yes (for ventilation cutouts) |
| **Brim** | Recommended for ABS |
| **Print Time** | ~3 hours |
| **Filament** | ~40g |

### Router Node Enclosure (Smallest!)

| Setting | Value |
|---------|-------|
| **Material** | PLA or PETG |
| **Layer Height** | 0.2mm |
| **Infill** | 15% |
| **Supports** | No |
| **Brim** | Optional |
| **Print Time** | ~45 minutes |
| **Filament** | ~15g |

---

## 📐 Design Features

### Sensor Node Enclosure

```
┌────────────────────────────────────────┐
│                                        │
│  ╔════════════════════════╗            │
│  ║    Sensor Cable Hole   ║  ← 8mm gland
│  ╚════════════════════════╝            │
│                                        │
│  ┌──────────────────────────────────┐  │
│  │                                  │  │
│  │           PCB Area               │  │
│  │                                  │  │
│  └──────────────────────────────────┘  │
│                                        │
│  ○ ○  LED Windows                      │
│                                        │
│  [●] Button Access                     │
│                                        │
└────────────────────────────────────────┘
   └──────── USB-C Port ────────┘
```

**Features:**
- ✅ USB-C power port cutout
- ✅ Cable gland hole for sensor cable
- ✅ LED light pipes
- ✅ Button access hole
- ✅ 4x M3 mounting holes
- ✅ Snap-fit lid
- ✅ CULTIVIO logo embossed

### Controller Node Enclosure

```
┌────────────────────────────────────────────────┐
│                                                │
│  ═══════════════════════════════════           │ ← Ventilation slots
│                                                │
│  ┌──────────────────────────────────────────┐  │
│  │                                          │  │
│  │              PCB Area                    │  │
│  │                                          │  │
│  │    [RELAY]           [ESP32-H2]          │  │
│  │                                          │  │
│  └──────────────────────────────────────────┘  │
│                                                │
│  ○ ○  LEDs    🔊 Buzzer Holes                  │
│                                                │
│  [●] Button                                    │
│                                                │
│  ═══════════════════════════════════           │ ← Ventilation slots
│                                                │
├────────────────────────────────────────────────┤
│     [  Screw Terminals  ]                      │ ← Pump wires
└────────────────────────────────────────────────┘
   └──────── USB-C Port ────────┘
```

**Features:**
- ✅ USB-C power port cutout
- ✅ Screw terminal access cutouts
- ✅ Ventilation slots (for relay heat)
- ✅ Buzzer sound holes pattern
- ✅ LED light pipes
- ✅ Button access hole
- ✅ 4x M3 mounting holes
- ✅ DIN rail mounting slot
- ✅ Optional wall mount bracket
- ✅ CULTIVIO logo + "AquaSense" text
- ✅ ⚡230V warning label

### Router Node Enclosure (Compact!)

```
┌──────────────────────────┐
│                          │
│  ○ ○  Status LEDs        │
│                          │
│  ┌────────────────────┐  │
│  │                    │  │
│  │      PCB           │  │
│  │                    │  │
│  └────────────────────┘  │
│                          │
│  [●] Button              │
│                          │
└──────────────────────────┘
   └─── USB-C ───┘
```

**Features:**
- ✅ Ultra-compact (33x28mm outer)
- ✅ USB-C power port
- ✅ LED windows (status + activity)
- ✅ Button access
- ✅ Keyhole wall mount
- ✅ Optional: Wall plug housing (wraps USB charger)
- ✅ CULTIVIO "Router" text
- ✅ Signal strength icon

---

## 🎨 Customization

### Modify Parameters

Open the `.scad` file and adjust parameters at the top:

```openscad
// Change PCB dimensions
pcb_length = 45;       // Your PCB length
pcb_width = 35;        // Your PCB width

// Change wall thickness
wall_thickness = 2.0;  // Thicker = stronger

// Change USB position
usb_offset_x = 22.5;   // Adjust based on your PCB
```

### Add Your Logo

Replace the text block:
```openscad
// Original
text("CULTIVIO", size=5, halign="center", valign="center", font="Arial:style=Bold");

// Your logo
import("your_logo.svg");
```

---

## 🛠️ Assembly

### Required Hardware

| Part | Qty | Size |
|------|-----|------|
| M3 x 8mm screws | 4 | For PCB mounting |
| M3 x 12mm screws | 4 | For enclosure mounting |
| M3 nuts | 4 | For enclosure mounting |
| Cable gland | 1 | PG7 (3-6.5mm) for sensor |
| Rubber feet | 4 | Self-adhesive |

### Assembly Steps

1. **Print both halves** (bottom + top)
2. **Install cable gland** in sensor enclosure
3. **Mount PCB** using M3 standoffs
4. **Route cables** through cutouts
5. **Snap or screw** lid in place
6. **Attach rubber feet** or mount bracket

---

## 🏭 Mass Production

### Injection Molding (1000+ units)

For higher volumes, convert to injection molding:

| Factor | 3D Print | Injection Mold |
|--------|----------|----------------|
| Setup Cost | ₹0 | ₹50,000-1,00,000 |
| Per Unit Cost | ₹40-50 | ₹8-15 |
| Lead Time | Hours | 30-45 days |
| Min Quantity | 1 | 500+ |
| Surface Finish | Layer lines | Smooth |

**Injection Molding Vendors (India):**
- Godrej Tooling
- Electronica Plastic Machines
- Pratibha Syntex

---

## 📏 Dimensional Drawings

### Sensor Node (Outer Dimensions)

```
           49mm
    ┌───────────────────┐
    │                   │
    │                   │ 39mm
    │                   │
    └───────────────────┘
    
    Height: 22mm (closed)
```

### Controller Node (Outer Dimensions)

```
           60mm
    ┌───────────────────────┐
    │                       │
    │                       │ 50mm
    │                       │
    └───────────────────────┘
    
    Height: 32mm (closed)
```

---

## 🎯 Tips

### For Better Prints

1. **Level your bed** properly
2. **Use blue painter's tape** or glue stick for ABS
3. **Print with enclosure** for ABS (prevents warping)
4. **Post-process**: Light sanding for smooth finish
5. **Spray paint** for professional look

### For Water Resistance

1. Use **PETG** (not PLA)
2. Add **silicone gasket** between halves
3. **Seal cable entry** with silicone sealant
4. Apply **conformal coating** on PCB

---

## 📞 Support

- **OpenSCAD Forum**: https://forum.openscad.org/
- **3D Printing Help**: /r/3Dprinting
- **Cultivio Support**: support@cultivio.io

---

*See PCB_DESIGN.md for PCB dimensions*
*See BOM_OPTIMIZATION.md for complete BOM*

