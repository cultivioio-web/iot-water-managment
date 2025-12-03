# 🏭 Manufacturing Files

## JLCPCB-Ready BOM and CPL Files

---

## 📁 Files

### PCB Assembly (JLCPCB)

| File | Description | Use |
|------|-------------|-----|
| `sensor_node_bom.csv` | Bill of Materials | JLCPCB SMT Assembly |
| `sensor_node_cpl.csv` | Component Placement | JLCPCB SMT Assembly |
| `controller_node_bom.csv` | Bill of Materials | JLCPCB SMT Assembly |
| `controller_node_cpl.csv` | Component Placement | JLCPCB SMT Assembly |
| `router_node_bom.csv` | Bill of Materials | JLCPCB SMT Assembly |
| `router_node_cpl.csv` | Component Placement | JLCPCB SMT Assembly |

### Kit Accessories (Separate Purchase)

| File | Description | Use |
|------|-------------|-----|
| `kit_accessories_bom.csv` | Cables, sensors, screws, packaging | Order separately |

### Product Packaging

| File | Description | Location |
|------|-------------|----------|
| `product_box_design.md` | Retail box specs & artwork | `hardware/packaging/` |

---

## 📋 BOM Format

The BOM files use JLCPCB's required format:

```csv
Comment,Designator,Footprint,LCSC Part #,Quantity
"10uF Capacitor","C1","0805","C15850",1
```

| Column | Description |
|--------|-------------|
| Comment | Component description |
| Designator | Reference designator (R1, C2, U1, etc.) |
| Footprint | Package type |
| LCSC Part # | LCSC/JLCPCB part number |
| Quantity | Number of components |

---

## 📍 CPL Format

The CPL (Component Placement List) files specify where each component goes:

```csv
Designator,Mid X,Mid Y,Layer,Rotation
C1,5.5,3.5,top,0
```

| Column | Description |
|--------|-------------|
| Designator | Reference designator |
| Mid X | X coordinate (mm) from board origin |
| Mid Y | Y coordinate (mm) from board origin |
| Layer | top or bottom |
| Rotation | Degrees (0, 90, 180, 270) |

---

## 🛒 How to Use with JLCPCB

### Step 1: Order PCBs

1. Go to https://jlcpcb.com
2. Upload Gerber files
3. Select PCB options

### Step 2: Enable SMT Assembly

1. Toggle "SMT Assembly" ON
2. Select:
   - Assembly Side: Top
   - SMT Qty: Match PCB qty
   - Tooling holes: Added by JLCPCB

### Step 3: Upload Files

1. Click "Add BOM File"
2. Upload corresponding `*_bom.csv`
3. Click "Add CPL File"
4. Upload corresponding `*_cpl.csv`

### Step 4: Review Parts

JLCPCB will show:
- ✅ Green: Part in stock
- ⚠️ Yellow: Extended part (extra fee)
- ❌ Red: Not available

### Step 5: Confirm and Order

1. Review placement preview
2. Confirm part orientations
3. Add to cart
4. Checkout

---

## 💰 Cost Estimate (SMT Assembly)

| Item | 5 boards | 10 boards |
|------|----------|-----------|
| **Sensor Node** | | |
| PCB | $2 | $3 |
| SMT Setup | $8 | $8 |
| Components | ~$10 | ~$18 |
| **Subtotal** | ~$20 | ~$29 |
| | | |
| **Controller Node** | | |
| PCB | $2 | $3 |
| SMT Setup | $8 | $8 |
| Components | ~$12 | ~$22 |
| **Subtotal** | ~$22 | ~$33 |
| | | |
| **Router Node** | | |
| PCB | $2 | $3 |
| SMT Setup | $8 | $8 |
| Components | ~$8 | ~$14 |
| **Subtotal** | ~$18 | ~$25 |

*Note: Shipping additional ($15-20)*

---

## 🔧 Through-Hole Components

Some components must be soldered manually after receiving boards:

### Controller Node
| Component | Qty | Notes |
|-----------|-----|-------|
| Relay (K1) | 1 | SRD-05VDC-SL-C |
| Screw Terminals | 2 | J2, J3 |
| Buzzer | 1 | BZ1 |

### Sensor Node
| Component | Qty | Notes |
|-----------|-----|-------|
| JST-XH 4P | 1 | J2 - Sensor connector |

---

## 📝 Notes

1. **Part numbers are from LCSC** - JLCPCB's component partner
2. **Coordinates in CPL** are approximate - update after actual PCB layout
3. **Extended parts** cost more but have better availability
4. **Basic parts** are cheapest but may have stock issues

---

## 🔗 Useful Links

| Resource | URL |
|----------|-----|
| JLCPCB | https://jlcpcb.com |
| LCSC (Parts) | https://lcsc.com |
| Part Search | https://jlcpcb.com/parts |

---

*See JLCPCB_ORDER_GUIDE.md for detailed ordering steps*

