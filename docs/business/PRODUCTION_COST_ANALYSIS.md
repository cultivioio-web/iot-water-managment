# 💰 Total Production Cost Analysis

## Cultivio AquaSense - Complete Cost Breakdown

---

## 📊 Cost Categories Overview

```
Total Production Cost = BOM + Labor + Overhead + Quality + Logistics

┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│   BOM (Materials)           ████████████████████████  60-65%    │
│   Assembly Labor            █████████                 15-18%    │
│   Testing & QC              ████                       8-10%    │
│   Packaging Labor           ███                        5-7%     │
│   Overhead & Admin          ███                        5-8%     │
│   Defects & Warranty        ██                         3-5%     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1️⃣ BOM (Bill of Materials)

### Basic Kit (2 Nodes)

| Category | Cost |
|----------|------|
| Sensor Node PCB + Components | ₹469 |
| Controller Node PCB + Components | ₹404 |
| JSN-SR04T Sensor | ₹150 |
| Cables & Connectors | ₹145 |
| Screws & Hardware | ₹28 |
| Packaging Materials | ₹64 |
| **BOM Total** | **₹1,260** |

### Extended Kit (3 Nodes)

| Category | Cost |
|----------|------|
| All Basic Kit items | ₹1,260 |
| Router Node PCB + Components | ₹335 |
| Additional USB Cable | ₹50 |
| Additional Hardware | ₹14 |
| Larger Box | ₹9 |
| **BOM Total** | **₹1,668** |

---

## 2️⃣ Assembly Labor

### Per Unit Assembly Time

| Task | Basic Kit | Extended Kit |
|------|-----------|--------------|
| PCB inspection | 5 min | 7 min |
| Through-hole soldering (relay, terminals) | 15 min | 15 min |
| Enclosure assembly (PCB mount, close case) | 10 min | 15 min |
| Sensor cable preparation | 5 min | 5 min |
| Final assembly check | 5 min | 8 min |
| **Total Assembly Time** | **40 min** | **50 min** |

### Labor Cost Calculation

```
Assumptions:
- Assembly worker wage: ₹15,000/month
- Working hours: 8 hrs/day × 26 days = 208 hrs/month
- Hourly rate: ₹15,000 ÷ 208 = ₹72/hour
- Per minute: ₹1.20/min

Basic Kit:  40 min × ₹1.20 = ₹48
Extended Kit: 50 min × ₹1.20 = ₹60

Add 30% for supervision + benefits = 
Basic Kit: ₹48 × 1.30 = ₹62
Extended Kit: ₹60 × 1.30 = ₹78
```

| Kit | Assembly Labor |
|-----|----------------|
| Basic | ₹62 |
| Extended | ₹78 |

---

## 3️⃣ Testing & Quality Control

### Testing Time Per Unit

| Test | Time | Equipment |
|------|------|-----------|
| Visual inspection | 3 min | Magnifier |
| Power-on test | 2 min | USB power, multimeter |
| Firmware flash | 3 min | Computer, USB cable |
| BLE connectivity test | 2 min | Phone/app |
| Sensor accuracy test | 3 min | Test tank |
| Relay function test | 2 min | Multimeter |
| Burn-in (batch) | 30 min shared | Power rack |
| Final inspection | 2 min | Checklist |
| **Total QC Time** | **~20 min** | |

### QC Labor Cost

```
QC technician wage: ₹18,000/month (higher skill)
Hourly rate: ₹87/hour
Per minute: ₹1.45/min

Basic Kit: 20 min × ₹1.45 = ₹29
Extended Kit: 25 min × ₹1.45 = ₹36

Add equipment depreciation: ₹5/unit
Add test consumables: ₹2/unit
```

| Kit | Testing & QC Cost |
|-----|-------------------|
| Basic | ₹36 |
| Extended | ₹43 |

---

## 4️⃣ Packaging Labor

### Packaging Time

| Task | Time |
|------|------|
| Place items in tray | 3 min |
| Add accessories pouch | 1 min |
| Insert documentation | 1 min |
| Close box, apply seals | 2 min |
| Apply serial stickers | 1 min |
| Final weight check | 1 min |
| **Total Packaging Time** | **9 min** |

### Packaging Labor Cost

```
Packing worker wage: ₹12,000/month
Hourly rate: ₹58/hour
Per minute: ₹0.97/min

Basic Kit: 9 min × ₹0.97 = ₹9
Extended Kit: 11 min × ₹0.97 = ₹11
```

| Kit | Packaging Labor |
|-----|-----------------|
| Basic | ₹9 |
| Extended | ₹11 |

---

## 5️⃣ Overhead & Administrative

### Monthly Fixed Costs (Small Setup)

| Item | Monthly Cost | Per Unit (500/month) |
|------|--------------|----------------------|
| Rent (small workshop) | ₹15,000 | ₹30 |
| Electricity | ₹5,000 | ₹10 |
| Internet + Phone | ₹2,000 | ₹4 |
| Accounting/Legal | ₹5,000 | ₹10 |
| Insurance | ₹2,000 | ₹4 |
| Misc supplies | ₹3,000 | ₹6 |
| **Total Overhead** | **₹32,000** | **₹64** |

### Scaling Impact

| Volume | Overhead/Unit |
|--------|---------------|
| 100 units/month | ₹320 |
| 250 units/month | ₹128 |
| 500 units/month | ₹64 |
| 1000 units/month | ₹45 |

**For calculation, using 500 units/month:**

| Kit | Overhead Cost |
|-----|---------------|
| Basic | ₹64 |
| Extended | ₹64 |

---

## 6️⃣ Defects, Rework & Warranty

### Defect Rate Assumptions

| Stage | Defect Rate | Action |
|-------|-------------|--------|
| PCB (from JLCPCB) | 1% | Replace board |
| Assembly defect | 2% | Rework |
| Testing failure | 1% | Rework or scrap |
| DOA (Dead on Arrival) | 1% | Replace unit |
| Warranty returns (6 mo) | 3% | Repair or replace |
| **Total Defect Allowance** | **~5%** | |

### Defect Cost Calculation

```
Average rework cost: ₹100/unit (labor + parts)
Average replacement cost: ₹500/unit
Weighted defect cost: 5% × average

Basic Kit: 5% × ₹1,290 (BOM) = ₹65
Extended Kit: 5% × ₹1,698 (BOM) = ₹85
```

| Kit | Defect/Warranty Reserve |
|-----|-------------------------|
| Basic | ₹65 |
| Extended | ₹85 |

---

## 7️⃣ Shipping & Handling (B2B to Dealer)

### Outbound Logistics

```
Shipping to dealer/distributor:
- Average shipment: 20 units
- Courier cost: ₹400 per shipment
- Per unit: ₹400 ÷ 20 = ₹20

Handling & packing for shipment:
- Outer carton, padding: ₹10/unit
- Labor: ₹5/unit
```

| Kit | Shipping & Handling |
|-----|---------------------|
| Basic | ₹35 |
| Extended | ₹40 |

---

## 📊 TOTAL PRODUCTION COST SUMMARY

### Basic Kit (2 Nodes)

| Cost Category | Amount | % of Total |
|---------------|--------|------------|
| BOM (Materials) | ₹1,260 | 62.9% |
| Assembly Labor | ₹62 | 3.1% |
| Testing & QC | ₹36 | 1.8% |
| Packaging Labor | ₹9 | 0.4% |
| Overhead | ₹64 | 3.2% |
| Defect/Warranty Reserve | ₹65 | 3.2% |
| Shipping & Handling | ₹35 | 1.7% |
| **TOTAL PRODUCTION COST** | **₹1,531** | **76.3%** |
| | | |
| **Selling Price (MRP)** | **₹2,999** | |
| **Gross Profit** | **₹1,468** | **49.0%** |

### Extended Kit (3 Nodes)

| Cost Category | Amount | % of Total |
|---------------|--------|------------|
| BOM (Materials) | ₹1,668 | 60.2% |
| Assembly Labor | ₹78 | 2.8% |
| Testing & QC | ₹43 | 1.6% |
| Packaging Labor | ₹11 | 0.4% |
| Overhead | ₹64 | 2.3% |
| Defect/Warranty Reserve | ₹85 | 3.1% |
| Shipping & Handling | ₹40 | 1.4% |
| **TOTAL PRODUCTION COST** | **₹1,989** | **71.8%** |
| | | |
| **Selling Price (MRP)** | **₹3,999** | |
| **Gross Profit** | **₹2,010** | **50.3%** |

---

## 📈 Cost Breakdown Visualization

```
BASIC KIT - ₹1,531 Total Production Cost
═══════════════════════════════════════════════════════════════

BOM Materials     ████████████████████████████████████████  ₹1,260
Assembly Labor    ████                                       ₹62
Testing & QC      ██                                         ₹36
Packaging Labor   █                                          ₹9
Overhead          ████                                       ₹64
Defect Reserve    ████                                       ₹65
Shipping          ██                                         ₹35

                  ├────────────────────────────────────────┤
                  0              ₹500           ₹1,000    ₹1,500
```

---

## 💹 Margin Analysis

### At Different Price Points

| MRP | Production Cost | Gross Profit | Margin % |
|-----|-----------------|--------------|----------|
| ₹2,499 | ₹1,531 | ₹968 | 38.7% |
| ₹2,799 | ₹1,531 | ₹1,268 | 45.3% |
| **₹2,999** | **₹1,531** | **₹1,468** | **49.0%** |
| ₹3,299 | ₹1,531 | ₹1,768 | 53.6% |
| ₹3,499 | ₹1,531 | ₹1,968 | 56.2% |

### After Dealer Discount

```
If selling to dealer at 30% discount:

Dealer Price: ₹2,999 × 0.70 = ₹2,099
Production Cost: ₹1,531
Gross Profit: ₹568
Margin: 27.1%

Still profitable, but need volume!
```

---

## 📉 Cost Reduction at Scale

### Volume Impact on Total Cost

| Volume/Month | Production Cost | Margin at ₹2,999 |
|--------------|-----------------|------------------|
| 100 units | ₹1,787 | 40.4% |
| 250 units | ₹1,615 | 46.1% |
| **500 units** | **₹1,531** | **49.0%** |
| 1,000 units | ₹1,420 | 52.7% |
| 2,000 units | ₹1,350 | 55.0% |

### Where Savings Come From

| Factor | Savings at 1000+ units |
|--------|------------------------|
| Bulk component pricing | -₹80 |
| PCB volume discount | -₹20 |
| Packaging volume discount | -₹15 |
| Overhead spread | -₹20 |
| Assembly efficiency | -₹10 |
| **Total Savings** | **-₹145/unit** |

---

## 🎯 Break-Even Analysis

### Monthly Fixed Costs

| Item | Cost |
|------|------|
| Rent | ₹15,000 |
| Utilities | ₹5,000 |
| Salaries (2 workers) | ₹30,000 |
| Admin | ₹5,000 |
| **Total Fixed** | **₹55,000** |

### Break-Even Calculation

```
Variable Cost per Unit (Basic): ₹1,467
(Total production - fixed overhead allocation)

Contribution Margin: ₹2,999 - ₹1,467 = ₹1,532

Break-Even Units: ₹55,000 ÷ ₹1,532 = 36 units/month

At 36 units/month → Break even
At 100 units/month → ₹98,200 profit
At 500 units/month → ₹711,000 profit
```

---

## 📋 Cost Control Checklist

### To Minimize Production Cost:

- [ ] Order components in bulk (MOQ 100+)
- [ ] Use JLCPCB SMT assembly (vs manual)
- [ ] Batch test multiple units together
- [ ] Train workers for efficiency
- [ ] Reduce defect rate below 3%
- [ ] Negotiate volume discounts with suppliers
- [ ] Optimize packaging (smaller box if possible)
- [ ] Consolidate shipments to dealers

---

## 🔮 Future Cost Reduction Roadmap

| Phase | Volume | Target Cost | Target Margin |
|-------|--------|-------------|---------------|
| Launch | 100/mo | ₹1,787 | 40% |
| Growth | 500/mo | ₹1,531 | 49% |
| Scale | 1000/mo | ₹1,420 | 53% |
| Mature | 5000/mo | ₹1,200 | 60% |

---

## 📞 Summary

| Metric | Basic Kit | Extended Kit |
|--------|-----------|--------------|
| **BOM Only** | ₹1,260 | ₹1,668 |
| **Total Production Cost** | ₹1,531 | ₹1,989 |
| **MRP** | ₹2,999 | ₹3,999 |
| **Gross Profit** | ₹1,468 | ₹2,010 |
| **Gross Margin** | 49.0% | 50.3% |

**Key Insight:** BOM is only ~62% of total cost. Labor, testing, and overhead add ~₹270 per Basic Kit!

---

*See BOM_OPTIMIZATION.md for material cost details*
*See FINANCIAL_MODEL.md for business projections*

