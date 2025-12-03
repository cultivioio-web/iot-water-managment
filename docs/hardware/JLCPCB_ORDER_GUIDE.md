# 🛒 JLCPCB Ordering Guide

## Step-by-Step Guide to Order PCBs from JLCPCB

---

## 📋 Overview

JLCPCB offers:
- **PCB Manufacturing**: 5 PCBs for ~$2
- **SMT Assembly**: Components soldered for ~$8 setup
- **Fast Shipping**: 5-10 days to India

Website: https://jlcpcb.com

---

## 🚀 Step 1: Prepare Files

### Required Files

Before ordering, generate these files from KiCad:

#### Gerber Files (PCB Manufacturing)
```
📁 gerber/
├── sensor_node-F_Cu.gtl       # Top copper
├── sensor_node-B_Cu.gbl       # Bottom copper
├── sensor_node-F_SilkS.gto    # Top silkscreen
├── sensor_node-B_SilkS.gbo    # Bottom silkscreen
├── sensor_node-F_Mask.gts     # Top solder mask
├── sensor_node-B_Mask.gbs     # Bottom solder mask
├── sensor_node-Edge_Cuts.gko  # Board outline
└── sensor_node.drl            # Drill file
```

#### Assembly Files (SMT Assembly - Optional)
```
📁 assembly/
├── sensor_node_bom.csv        # Bill of Materials
└── sensor_node_cpl.csv        # Component Placement List
```

### Generate Gerbers in KiCad

1. Open PCB in KiCad Pcbnew
2. **File → Plot**
3. Select output directory: `gerber/`
4. Select layers:
   - [x] F.Cu
   - [x] B.Cu
   - [x] F.SilkS
   - [x] B.SilkS
   - [x] F.Mask
   - [x] B.Mask
   - [x] Edge.Cuts
5. Plot format: **Gerber**
6. Check: **Use Protel filename extensions**
7. Click **Plot**
8. Click **Generate Drill Files** → select **Excellon** → click **Generate**

### Create ZIP File

```powershell
# Windows PowerShell
Compress-Archive -Path .\gerber\* -DestinationPath sensor_node_gerber.zip
```

---

## 🌐 Step 2: Upload to JLCPCB

### Create Account

1. Go to https://jlcpcb.com
2. Click "Sign Up" (top right)
3. Enter email and create password
4. Verify email

### Upload Gerber Files

1. Click **"Order Now"** or **"Quote Now"**
2. Click **"Add gerber file"**
3. Upload your `sensor_node_gerber.zip`
4. Wait for file processing (~30 seconds)

---

## ⚙️ Step 3: Configure PCB Options

### Basic Options (Cheapest)

| Option | Value | Notes |
|--------|-------|-------|
| **Base Material** | FR-4 | Standard |
| **Layers** | 2 | Double-sided |
| **Dimensions** | Auto-detected | From Gerber |
| **PCB Qty** | 5 | Minimum |
| **Product Type** | Industrial/Consumer electronics | |
| **Different Design** | 1 | Single design |
| **Delivery Format** | Single PCB | |
| **PCB Thickness** | 1.6mm | Standard |
| **PCB Color** | Green | Cheapest |
| **Silkscreen** | White | Standard |
| **Surface Finish** | HASL (with lead) | Cheapest |
| **Outer Copper Weight** | 1 oz | Standard |
| **Gold Fingers** | No | |
| **Confirm Production File** | No | Unless you want to review |
| **Flying Probe Test** | Fully Test | Recommended |
| **Castellated Holes** | No | |
| **Remove Order Number** | No | $1.50 extra if yes |

### Screenshot Reference

```
┌─────────────────────────────────────────────────────────────┐
│ JLCPCB PCB Quote                                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Base Material:    [FR-4        ▼]                          │
│  Layers:           [2           ▼]  ✓ Correct               │
│  Dimensions:       45 x 35 mm       ✓ Auto-detected         │
│  PCB Qty:          [5           ▼]  ← Minimum               │
│                                                             │
│  PCB Thickness:    [1.6         ▼]                          │
│  PCB Color:        [Green       ▼]  ← Cheapest              │
│  Surface Finish:   [HASL(lead)  ▼]  ← Cheapest              │
│                                                             │
│  ──────────────────────────────────────────────────────     │
│  PCB Price:                              $1.00              │
│  ──────────────────────────────────────────────────────     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔧 Step 4: Add SMT Assembly (Optional)

If you want JLCPCB to solder the components:

### Enable SMT Assembly

1. Scroll down to **"SMT Assembly"**
2. Toggle **ON**

### Assembly Options

| Option | Value | Notes |
|--------|-------|-------|
| **PCBA Type** | Economic | Cheaper |
| **Assembly Side** | Top Side | Most components on top |
| **PCBA Qty** | 5 | Match PCB qty |
| **Tooling holes** | Added by JLCPCB | Free |
| **Confirm Parts Placement** | No | Unless complex design |

### Upload BOM and CPL

#### BOM Format (CSV)
```csv
Comment,Designator,Footprint,LCSC Part #
10uF,C1,0805,C15850
100nF,C2 C3,0805,C49678
10K,R1,0805,C25744
330R,R2 R3,0805,C25198
LED Green,LED1,0805,C2297
LED Red,LED2,0805,C2286
ESP32-H2-MINI-1,U1,Module,C2976052
AMS1117-3.3,U2,SOT-223,C6186
Push Button,SW1,6x6mm,C127509
USB-C 16P,J1,SMD,C2765186
```

#### CPL Format (CSV)
```csv
Designator,Mid X,Mid Y,Layer,Rotation
C1,5.5,3.5,top,0
C2,8.2,5.1,top,90
C3,12.3,5.1,top,90
R1,15.5,8.2,top,0
R2,18.0,12.5,top,90
R3,21.0,12.5,top,90
LED1,35.0,17.5,top,0
LED2,35.0,25.5,top,0
U1,22.5,17.5,top,0
U2,10.0,10.0,top,0
SW1,5.0,17.5,top,0
J1,22.5,2.0,top,180
```

### Check Parts Availability

1. JLCPCB will show which parts are available in their stock
2. Green checkmark = Available
3. Yellow warning = Extended parts (extra fee)
4. Red X = Not available (need to substitute)

---

## 💰 Step 5: Review Pricing

### Pricing Breakdown (Example)

**PCB Only (5 pcs)**
```
PCB Manufacturing:         $1.00
PCB Shipping (DHL):        $15.00
──────────────────────────────────
Total:                     $16.00 (~₹1,350)
```

**PCB + SMT Assembly (5 pcs)**
```
PCB Manufacturing:         $1.00
SMT Setup Fee:             $8.00
Component Cost:            $12.00
Extended Parts Fee:        $3.00
SMT Shipping (DHL):        $18.00
──────────────────────────────────
Total:                     $42.00 (~₹3,500)
```

### Cost per Board

| Order Type | 5 PCBs | 10 PCBs | 20 PCBs |
|------------|--------|---------|---------|
| PCB Only | $3.20/board | $2.00/board | $1.50/board |
| PCB + SMT | $8.40/board | $6.00/board | $4.50/board |

---

## 📦 Step 6: Shipping Options

### Shipping to India

| Method | Time | Cost (5 PCBs) |
|--------|------|---------------|
| **DHL Express** | 3-5 days | $15-20 |
| **FedEx** | 3-5 days | $15-20 |
| **Standard Post** | 15-30 days | $5-8 |
| **OCS/SPSR** | 7-15 days | $8-12 |

**Recommendation**: DHL Express for small orders (fast + tracking)

### Customs & Duties

- PCBs under $50 usually pass without duty
- If charged: ~18% GST
- Declare as "Printed Circuit Boards - Prototype"

---

## ✅ Step 7: Place Order

### Final Checklist

- [ ] Gerber files uploaded and verified
- [ ] Board dimensions correct
- [ ] PCB options selected (color, thickness, finish)
- [ ] SMT assembly configured (if needed)
- [ ] BOM and CPL uploaded (if SMT)
- [ ] Parts availability checked
- [ ] Shipping method selected
- [ ] Payment method ready

### Payment Methods

- Credit/Debit Card (Visa, Mastercard)
- PayPal
- Wire Transfer

### Order Confirmation

1. Click **"Save to Cart"**
2. Review cart
3. Click **"Checkout Securely"**
4. Enter shipping address
5. Select payment method
6. Complete payment
7. **Save Order Number**

---

## 📬 Step 8: Track & Receive

### Track Order

1. Log in to JLCPCB
2. Go to **"Order History"**
3. Click on your order
4. View status and tracking number

### Order Statuses

```
1. Payment Confirmed     ← Order received
2. In Production        ← PCB being manufactured
3. Production Completed ← Ready for shipping
4. Shipped              ← On the way
5. Delivered            ← Received!
```

### Typical Timeline

| Step | Time |
|------|------|
| Order → Production Start | 1 day |
| Production | 2-3 days |
| Shipping (DHL) | 3-5 days |
| **Total** | **6-9 days** |

---

## 🔍 Quality Check on Arrival

### Visual Inspection

- [ ] Correct board dimensions
- [ ] All holes drilled properly
- [ ] Silkscreen readable
- [ ] No scratches or defects
- [ ] Copper traces intact

### Electrical Testing

```
1. Check for shorts between power rails:
   - Multimeter: Continuity between VCC and GND → Should be OPEN
   
2. Check trace continuity:
   - Select important traces
   - Test end-to-end connectivity

3. Visual solder check (if SMT):
   - All components present
   - No solder bridges
   - Components aligned properly
```

---

## 🆘 Troubleshooting

### Common Issues

**Issue**: "Gerber file error"
- **Solution**: Re-export Gerbers with Protel extensions enabled

**Issue**: "Board size too small"
- **Solution**: Check Edge.Cuts layer in KiCad

**Issue**: "Minimum order 5 PCBs"
- **Solution**: Accept it - $2 for 5 is still cheap!

**Issue**: "Part not in stock"
- **Solution**: Choose alternative LCSC part or order parts separately

---

## 📊 Quick Reference

### Sensor Node Order

```
Settings:
- Layers: 2
- Size: 45 x 35 mm
- Qty: 5
- Thickness: 1.6mm
- Color: Green
- Finish: HASL
- Copper: 1oz

Estimated Cost: $2 PCB + $15 shipping = $17 total
```

### Controller Node Order

```
Settings:
- Layers: 2
- Size: 55 x 45 mm
- Qty: 5
- Thickness: 1.6mm
- Color: Green
- Finish: HASL
- Copper: 1oz

Estimated Cost: $2 PCB + $15 shipping = $17 total
```

### Combined Order (Both)

```
- Order both designs together
- Select "Different Design: 2"
- Or order separately for simplicity

Total: ~$30-35 for 5 of each
```

---

## 🔗 Useful Links

| Resource | URL |
|----------|-----|
| JLCPCB | https://jlcpcb.com |
| LCSC (Components) | https://lcsc.com |
| JLCPCB Capabilities | https://jlcpcb.com/capabilities/pcb-capabilities |
| Assembly Guide | https://jlcpcb.com/smt-assembly |
| Shipping Calculator | https://jlcpcb.com/quote |

---

## 💡 Pro Tips

1. **Order in bulk**: 10 PCBs costs barely more than 5
2. **Use standard colors**: Green is cheapest
3. **Combine shipping**: Order all boards together
4. **Check stock first**: Use LCSC to verify component availability
5. **Keep designs under 100x100mm**: Larger boards cost more
6. **Use panelization**: Multiple small boards in one panel saves money

---

*See BOM_OPTIMIZATION.md for component sourcing*
*See PCB_DESIGN.md for design guidelines*

