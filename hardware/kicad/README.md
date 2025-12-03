# 🔌 KiCad Project Files

## Cultivio AquaSense PCB Design

---

## 📁 Project Structure

```
kicad/
├── sensor_node/
│   ├── sensor_node.kicad_pro      # KiCad project file
│   ├── sensor_node.kicad_sch      # Schematic
│   └── sensor_node.kicad_pcb      # PCB layout (create in KiCad)
├── controller_node/
│   ├── controller_node.kicad_pro  # KiCad project file
│   ├── controller_node.kicad_sch  # Schematic
│   └── controller_node.kicad_pcb  # PCB layout (create in KiCad)
├── router_node/
│   ├── router_node.kicad_pro      # KiCad project file
│   ├── router_node.kicad_sch      # Schematic
│   └── router_node.kicad_pcb      # PCB layout (create in KiCad)
├── libraries/
│   ├── cultivio.kicad_sym         # Custom symbols
│   └── cultivio.pretty/           # Custom footprints (create folder)
└── README.md
```

---

## 🚀 Getting Started

### 1. Install KiCad

Download from: https://www.kicad.org/download/

**Recommended Version**: KiCad 7.0 or later

### 2. Open Project

1. Launch KiCad
2. File → Open Project
3. Navigate to `hardware/kicad/sensor_node/`, `controller_node/`, or `router_node/`
4. Open `.kicad_pro` file

### 3. Add Custom Library

1. In KiCad, go to Preferences → Manage Symbol Libraries
2. Click "Add existing library to table"
3. Browse to `libraries/cultivio.kicad_sym`
4. Click OK

---

## 📐 Schematic Design Steps

### Sensor Node

1. **Open Schematic Editor** (Eeschema)
2. **Add Components**:
   - Place ESP32-H2-MINI-1 (from cultivio library)
   - Place AMS1117-3.3 (from cultivio library)
   - Place USB-C connector (from Connector library)
   - Place JST-XH 4P connector
   - Place capacitors (10µF, 100nF)
   - Place resistors (10K, 330Ω)
   - Place LEDs (0805)
   - Place push button

3. **Wire Connections**:
   ```
   USB-C VBUS → 5V Rail → AMS1117 IN
   AMS1117 OUT → 3.3V Rail → ESP32 VDD
   GND → All grounds connected
   
   ESP32 GPIO4 → JST-XH Pin 2 (TRIG)
   ESP32 GPIO5 → JST-XH Pin 3 (ECHO)
   JST-XH Pin 1 → 5V
   JST-XH Pin 4 → GND
   
   ESP32 GPIO8 → 330Ω → LED Green → GND
   ESP32 GPIO9 → 330Ω → LED Red → GND
   ESP32 GPIO0 → Button → GND (with 10K pull-up)
   ESP32 EN → 10K → 3.3V
   ```

4. **Run ERC** (Electrical Rules Check)

### Controller Node

1. **Open Schematic Editor** (Eeschema)
2. **Add Components**:
   - All components from Sensor Node
   - Plus: BC547 transistor
   - Plus: 1N4007 diode
   - Plus: Relay SRD-05VDC
   - Plus: Screw terminals (2x)
   - Plus: Active buzzer

3. **Wire Relay Driver**:
   ```
   ESP32 GPIO4 → 1K Resistor → BC547 Base
   BC547 Emitter → GND
   BC547 Collector → Relay Coil-
   Relay Coil+ → 5V
   1N4007 across relay coil (cathode to 5V)
   
   Relay COM → Pump Live wire
   Relay NO → Mains Live
   ```

---

## 🖥️ PCB Layout Steps

### 1. Create PCB from Schematic

1. Tools → Update PCB from Schematic
2. Click "Update PCB"

### 2. Set Board Outline

**Sensor Node**: 45mm x 35mm
```
Edge.Cuts layer:
- Rectangle from (0,0) to (45,35)
- Corner radius: 2mm
```

**Controller Node**: 55mm x 45mm
```
Edge.Cuts layer:
- Rectangle from (0,0) to (55,45)
- Corner radius: 2mm
```

**Router Node**: 30mm x 25mm (compact!)
```
Edge.Cuts layer:
- Rectangle from (0,0) to (30,25)
- Corner radius: 2mm
```

### 3. Component Placement

**Priority Order**:
1. ESP32-H2 module (center, antenna at edge)
2. USB-C connector (bottom edge)
3. AMS1117 (near USB)
4. Decoupling caps (near ICs)
5. JST connector / Relay (top edge)
6. LEDs (visible edge)
7. Button (accessible edge)

### 4. Routing

**Design Rules**:
| Parameter | Value |
|-----------|-------|
| Track Width (Signal) | 0.2mm |
| Track Width (Power) | 0.5mm |
| Clearance | 0.2mm |
| Via Drill | 0.3mm |
| Via Diameter | 0.6mm |

**Routing Guidelines**:
1. Route power first (5V, 3.3V, GND)
2. Use bottom layer as ground plane
3. Keep analog signals short
4. Avoid routing under antenna

### 5. Ground Plane

1. Select bottom copper layer (B.Cu)
2. Add → Zone
3. Draw around board outline
4. Net: GND
5. Fill zones (B key)

### 6. Antenna Keep-Out

Create a keep-out zone around ESP32-H2 antenna:
- No copper fill
- No traces
- 5mm clearance from module edge

---

## 📦 Manufacturing Files

### Generate Gerbers

1. File → Plot
2. Select layers:
   - F.Cu (Top Copper)
   - B.Cu (Bottom Copper)
   - F.SilkS (Top Silkscreen)
   - B.SilkS (Bottom Silkscreen)
   - F.Mask (Top Solder Mask)
   - B.Mask (Bottom Solder Mask)
   - Edge.Cuts (Board Outline)
3. Generate Drill Files (Excellon format)
4. Plot

### Output Files
```
gerber/
├── sensor_node-F_Cu.gtl
├── sensor_node-B_Cu.gbl
├── sensor_node-F_SilkS.gto
├── sensor_node-B_SilkS.gbo
├── sensor_node-F_Mask.gts
├── sensor_node-B_Mask.gbs
├── sensor_node-Edge_Cuts.gko
├── sensor_node.drl
└── sensor_node-job.gbrjob
```

### Generate BOM and Position Files

For JLCPCB SMT assembly:
1. File → Fabrication Outputs → BOM
2. File → Fabrication Outputs → Footprint Position File

---

## 🛒 Required KiCad Libraries

### Standard Libraries (Built-in)

- `Device` - Resistors, capacitors, LEDs
- `Connector` - USB, JST connectors
- `Transistor_BJT` - BC547
- `Diode` - 1N4007
- `Switch` - Push buttons

### External Libraries

**ESP32 Module**:
- Download from: https://github.com/espressif/kicad-libraries

**USB-C Connector**:
- Use: `Connector_USB:USB_C_Receptacle_GCT_USB4110-XX-A`

---

## 🎯 Design Tips

### For Beginners

1. **Start Simple**: Complete schematic first before PCB
2. **Use Built-in Libraries**: Most components available
3. **Check Footprints**: Verify package sizes match your parts
4. **Run DRC Often**: Design Rule Check catches errors early
5. **Generate 3D View**: Helps visualize final product

### Common Mistakes to Avoid

❌ Forgetting decoupling capacitors
❌ Wrong footprint size
❌ Traces too thin for power
❌ Antenna area not cleared
❌ Missing mounting holes

---

## 📞 Support

- **KiCad Forum**: https://forum.kicad.info/
- **KiCad Docs**: https://docs.kicad.org/
- **Cultivio Support**: support@cultivio.io

---

*See PCB_DESIGN.md for detailed schematic diagrams*
*See JLCPCB_ORDER_GUIDE.md for ordering instructions*

