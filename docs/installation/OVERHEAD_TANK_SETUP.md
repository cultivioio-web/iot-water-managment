# Cultivio AquaSense - Overhead Tank Setup Guide

## Common Home Setup: Tank on Terrace, Pump at Ground

This guide covers the most common residential scenario:
- **Tank**: 200-500 liter overhead tank on terrace/roof
- **Pump**: At ground level (ground floor or basement)
- **Distance**: ~50 feet (15 meters) vertical

## Typical Home Layout

```
┌──────────────────────────────────────────────────────────────┐
│                        TERRACE                               │
│                                                              │
│    ┌────────────────────────┐     ┌──────────────────┐       │
│    │   OVERHEAD TANK        │     │  SENSOR NODE     │       │
│    │   200-500 Ltr          │     │  (ESP32-H2)      │       │
│    │                        │     │                  │       │
│    │    ~~~~~~~~~~~~~~~     │◄────│  • HC-SR04       │       │
│    │    ~~~~~~~~~~~~~~~     │     │  • USB Power     │       │
│    │    ~~~~~~~~~~~~~~~     │     │  • Status LED    │       │
│    │                        │     └──────────────────┘       │
│    └────────────────────────┘                                │
│                                                              │
├────────────────── 2nd FLOOR ─────────────────────────────────┤
│                                                              │
│    [Optional: Router Node if signal is weak]                 │
│                                                              │
├────────────────── 1st FLOOR ─────────────────────────────────┤
│                                                              │
├────────────────── GROUND FLOOR ──────────────────────────────┤
│                                                              │
│    ┌──────────────────┐         ┌──────────────────┐         │
│    │   WATER PUMP     │◄────────│ CONTROLLER NODE  │         │
│    │   (AC Mains)     │  Relay  │  (ESP32-H2)      │         │
│    │                  │         │                  │         │
│    └──────────────────┘         │  • Relay Module  │         │
│                                 │  • USB Power     │         │
│                                 │  • Status LEDs   │         │
│                                 └──────────────────┘         │
│                                                              │
│    Distance: ~15m (50 ft) through 2-3 concrete floors        │
└──────────────────────────────────────────────────────────────┘
```

## Building Height Guide (1 to 5 Stories)

### Quick Reference: Do You Need a Router?

| Building | Height | Distance | Signal | Router? |
|----------|--------|----------|--------|---------|
| **1 Story** | ~10 ft (3m) | Short | 80-100% | ❌ No |
| **2 Story** | ~20 ft (6m) | Medium | 60-80% | ❌ No |
| **3 Story** | ~30 ft (9m) | Medium | 40-60% | ⚠️ Maybe |
| **4 Story** | ~40 ft (12m) | Long | 25-45% | ✅ Likely |
| **5 Story** | ~50 ft (15m) | Long | 15-35% | ✅ Yes |

### Detailed Signal Estimation by Building Type

| Stories | RCC Concrete | Brick/Block | Wood Frame |
|---------|--------------|-------------|------------|
| 1 | 85% ✅ | 90% ✅ | 95% ✅ |
| 2 | 65% ✅ | 75% ✅ | 85% ✅ |
| 3 | 45% ⚠️ | 55% ✅ | 70% ✅ |
| 4 | 30% ⚠️ | 40% ⚠️ | 55% ✅ |
| 5 | 20% ❌ | 30% ⚠️ | 45% ⚠️ |

**Legend:** ✅ Works | ⚠️ Test First | ❌ Need Router

### Visual Guide by Building Height

```
5-STORY BUILDING (50 ft / 15m)
┌─────────┐
│ TERRACE │ [Sensor]
├─────────┤                    ROUTER PLACEMENT
│ 5th Flr │                    for 4-5 Story:
├─────────┤                    
│ 4th Flr │                    [Router] ← Middle floor
├─────────┤                    
│ 3rd Flr │ ← Router here      
├─────────┤   for 5-story      
│ 2nd Flr │                    
├─────────┤                    
│ 1st Flr │                    
├─────────┤                    
│ GROUND  │ [Controller]
└─────────┘
Signal: 15-35% → Router NEEDED

─────────────────────────────────

3-STORY BUILDING (30 ft / 9m)
┌─────────┐
│ TERRACE │ [Sensor]
├─────────┤
│ 2nd Flr │ ← Router here
├─────────┤   if needed
│ 1st Flr │
├─────────┤
│ GROUND  │ [Controller]
└─────────┘
Signal: 40-60% → Usually works, test first

─────────────────────────────────

1-2 STORY BUILDING (10-20 ft / 3-6m)
┌─────────┐
│ TERRACE │ [Sensor]
├─────────┤
│ 1st Flr │
├─────────┤
│ GROUND  │ [Controller]
└─────────┘
Signal: 60-90% → Works WITHOUT router
```

### Recommendation by Building Height

| Building | Setup | Notes |
|----------|-------|-------|
| **1-2 Story** | 2 nodes only | Direct connection works great |
| **3 Story** | 2 nodes, test | Add router if signal < 50% |
| **4 Story** | Plan for router | Test first, likely need router |
| **5 Story** | 3 nodes required | Place router at 2nd-3rd floor |

### Router Placement Guide

| Building | Ideal Router Location |
|----------|----------------------|
| 3 Story | 1st or 2nd floor |
| 4 Story | 2nd floor |
| 5 Story | 2nd or 3rd floor |

**Rule of thumb:** Place router roughly in the middle between Sensor and Controller.

### Test First!

1. Install Sensor Node on terrace
2. Install Controller Node at ground level
3. Check signal strength in mobile app
4. If signal < 40%, add Router Node at middle floor

## Phase 1 Installation (2 Nodes)

### Step 1: Prepare Components

**Sensor Node (Terrace)**
- ESP32-H2 Mini
- HC-SR04 Ultrasonic Sensor
- USB power adapter + cable (long enough to reach outlet)
- Small weatherproof box
- Cable ties for mounting

**Controller Node (Ground)**
- ESP32-H2 Mini
- 3.3V Relay Module
- USB power adapter
- Wires for pump connection

### Step 2: Install Controller Node (Ground Level)

1. **Location**: Near water pump, protected from water
2. **Power**: Connect USB power adapter
3. **Pump Wiring**:
   ```
   AC Mains Live ──┬── Relay COM
                   │
   Pump Power ─────┴── Relay NO (Normally Open)
   ```
4. **First Boot**: Device enters provisioning mode
5. **Configure via App**:
   - Node Type: Controller
   - Pump ON: 20% (adjust based on your needs)
   - Pump OFF: 80%

### Step 3: Install Sensor Node (Terrace)

1. **Location**: Above tank opening, pointing down at water
2. **Mounting**:
   ```
   ┌─────────────┐
   │  Sensor Box │
   │  ┌───────┐  │
   │  │HC-SR04│  │ ← Mount securely
   │  └───┬───┘  │
   └──────┼──────┘
          │
          ▼ (Pointing straight down)
   ═══════════════  ← Tank opening
   ~~~~~~~~~~~~~~~  ← Water surface
   ```
3. **Power**: Run USB cable from nearest outlet
4. **Configure via App**:
   - Node Type: Sensor
   - Tank Height: Measure your tank (e.g., 80 cm)
   - Tank Diameter: Measure your tank (e.g., 100 cm)
   - Sensor Offset: Distance from top to sensor (e.g., 5 cm)

### Step 4: Power On Sequence

```
1. Power ON Controller first     → Creates Zigbee network
2. Wait 30 seconds               → Network stabilizes
3. Power ON Sensor               → Joins network
4. Check app                     → Should show water level
```

### Step 5: Verify Signal Strength

Open mobile app and check:

```
Signal: ████░ 65%  → Good! No router needed
Signal: ██░░░ 35%  → Weak - Consider adding router
Signal: █░░░░ 15%  → Poor - Router definitely needed
```

## If You Need Router Node (3 Nodes)

### When Router is Needed
- Signal consistently below 40%
- Frequent disconnections
- 3+ concrete floors between nodes

### Router Placement

```
TERRACE:     [Sensor Node]
                  │
                  │ ~7m
                  ▼
1st FLOOR:   [Router Node] ← Place here (middle floor)
                  │           Just needs USB power!
                  │ ~7m
                  ▼
GROUND:      [Controller Node]
```

### Router Installation

1. **Flash router firmware** to third ESP32-H2
2. **Place at middle floor** (hallway, stairwell)
3. **Connect USB power**
4. **Auto-joins network** - no configuration needed!
5. **LEDs indicate status**:
   - Status LED solid: Connected to network
   - Activity LED blinks: Relaying messages

## Power Options for Sensor Node

### Option 1: USB Extension (Recommended)
- Run USB cable from terrace outlet to sensor
- Use outdoor-rated extension if needed
- Cheapest and most reliable

### Option 2: USB Power Bank
- 10,000mAh bank lasts ~3-5 days
- Need to recharge periodically
- Good for temporary testing

### Option 3: Solar Power (Advanced)
- 5V solar panel + charge controller
- Rechargeable battery backup
- Fully autonomous operation
- Higher initial cost

## Weatherproofing

### Sensor Node on Terrace

```
┌─────────────────────────────────────┐
│        WEATHERPROOF BOX             │
│  ┌─────────────────────────────┐    │
│  │  ESP32-H2    [LED]          │    │
│  │              [LED]          │    │
│  └─────────────────────────────┘    │
│                                     │
│  USB Cable Entry → Sealed with      │
│                    silicone         │
│                                     │
│  ┌───────────────┐                  │
│  │   HC-SR04     │ ← Sensor faces   │
│  │   ▼ ▼ ▼ ▼     │   DOWN through   │
│  └───────────────┘   hole in box    │
└─────────────────────────────────────┘
```

**Tips:**
- Use IP65 rated junction box
- Drill hole at bottom for sensor (water can't enter from below)
- Seal USB cable entry with silicone
- Mount under small shade if possible (not in direct sun)

## Typical Tank Dimensions (India)

| Capacity | Height | Diameter | Sensor Offset |
|----------|--------|----------|---------------|
| 200 Ltr | 60 cm | 65 cm | 5 cm |
| 300 Ltr | 70 cm | 75 cm | 5 cm |
| 500 Ltr | 80 cm | 100 cm | 5-10 cm |
| 750 Ltr | 100 cm | 110 cm | 10 cm |
| 1000 Ltr | 110 cm | 120 cm | 10 cm |

## Troubleshooting

### No Water Level Reading
1. Check sensor wiring (TRIG→GPIO2, ECHO→GPIO3)
2. Ensure sensor points straight down
3. Clear any obstructions in sensor path
4. Verify sensor offset is correct

### Pump Not Turning On
1. Verify Controller has power
2. Check relay wiring to pump
3. Verify thresholds in app (ON should be < OFF)
4. Check if manual override is active

### Poor Signal / Disconnections
1. Check signal strength in app
2. Try repositioning nodes
3. Add router node at middle floor
4. Ensure no metal objects blocking signal path

### Sensor Shows Error
1. Clean ultrasonic sensor face
2. Check for water condensation
3. Verify minimum distance (2 cm from water)
4. Replace sensor if consistently failing

## Cost Estimate

### By Building Height

| Building | Nodes Needed | Estimated Cost |
|----------|--------------|----------------|
| 1-2 Story | 2 (Sensor + Controller) | ₹1,700 - ₹2,000 |
| 3 Story | 2-3 (may need router) | ₹1,700 - ₹2,500 |
| 4-5 Story | 3 (with router) | ₹2,200 - ₹2,800 |

### Detailed Cost Breakdown

**Basic 2-Node Setup (1-3 Story)**
| Item | Qty | Price (INR) | Total |
|------|-----|-------------|-------|
| ESP32-H2 Mini | 2 | ₹350 | ₹700 |
| HC-SR04 Sensor | 1 | ₹80 | ₹80 |
| Relay Module | 1 | ₹100 | ₹100 |
| USB Adapters | 2 | ₹150 | ₹300 |
| USB Cable (5m) | 1 | ₹200 | ₹200 |
| Junction Box | 1 | ₹150 | ₹150 |
| Misc (wires, etc) | - | ₹200 | ₹200 |
| **Total** | | | **₹1,730** |

**Add Router for 4-5 Story Buildings**
| Item | Qty | Price (INR) | Total |
|------|-----|-------------|-------|
| ESP32-H2 Mini | 1 | ₹350 | ₹350 |
| USB Adapter | 1 | ₹150 | ₹150 |
| **Router Total** | | | **₹500** |

**Grand Total with Router: ₹2,230**

---

## Quick Checklist

```
□ Measure tank dimensions (height, diameter)
□ Flash Controller firmware
□ Flash Sensor firmware  
□ Install Controller near pump
□ Wire relay to pump
□ Provision Controller via app
□ Install Sensor above tank
□ Provision Sensor via app
□ Power on Controller first
□ Power on Sensor
□ Verify connection in app
□ Check signal strength
□ Test pump ON/OFF
□ Test manual override
□ Done! 🎉
```

---

**Your 200-500 liter overhead tank is perfect for this system. The ~15m distance through 2-3 floors should work without a router in most cases - just check the signal strength after installation!** 💧📡

