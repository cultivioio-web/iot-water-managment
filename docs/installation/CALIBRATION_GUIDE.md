# Cultivio AquaSense - Sensor Calibration Guide

## Why Calibration Matters

The ultrasonic sensor measures the distance from itself to the water surface. For accurate water level percentages, you must correctly configure:

1. **Tank Height** - Total height of the water tank
2. **Sensor Offset** - Distance from top of tank to sensor face

```
┌─────────────────────────────────────────────────┐
│                  TANK DIAGRAM                   │
├─────────────────────────────────────────────────┤
│                                                 │
│     ┌───────────┐                               │
│     │  SENSOR   │ ← Mounted above tank          │
│     └─────┬─────┘                               │
│           │                                     │
│     ══════╪══════ ← Top of Tank                 │
│       ↑   │   ↑                                 │
│   Sensor  │   │                                 │
│   Offset  │   │ ← Empty Space (measured)        │
│       ↓   │   │                                 │
│     ~~~~~~│~~~~~~ ← Water Surface               │
│     ~~~~~~│~~~~~~                               │
│     ~~~~~~│~~~~~~ ← Water (calculated)          │
│     ~~~~~~│~~~~~~                               │
│     ══════╧══════ ← Bottom of Tank              │
│           ↑                                     │
│      Tank Height                                │
│           ↓                                     │
└─────────────────────────────────────────────────┘
```

## Measurement Procedure

### Step 1: Measure Tank Height

**What:** Total internal height from tank bottom to tank top opening.

**How:**
1. Use a measuring tape
2. Measure from inside bottom to top rim
3. Record in centimeters

```
Example: Tank Height = 200 cm
```

### Step 2: Measure Sensor Offset

**What:** Distance from tank top rim to the sensor face (where sound comes out).

**How:**
1. Mount the sensor above the tank
2. Measure from tank rim to sensor face
3. Record in centimeters

```
Example: Sensor Offset = 5 cm
```

### Step 3: Calculate Expected Readings

| Water Level | Distance Sensor Sees | Water % |
|-------------|---------------------|---------|
| Empty (0 cm water) | 200 cm + 5 cm = 205 cm | 0% |
| Half (100 cm water) | 100 cm + 5 cm = 105 cm | 50% |
| Full (200 cm water) | 0 cm + 5 cm = 5 cm | 100% |

**Formula:**
```
Water Level % = 100 - ((Distance - Offset) / Tank Height * 100)
```

## Calibration Steps

### 1. Initial Setup (Empty Tank Recommended)

1. **Drain tank** or note current water level
2. **Open provisioning app** on phone
3. **Connect to Sensor Node**
4. **Enter measurements:**
   - Tank Height: ___ cm
   - Tank Diameter: ___ cm (for volume calculation)
   - Sensor Offset: ___ cm
5. **Save & Activate**

### 2. Verification

1. **Check reading in app** after provisioning
2. **Compare with actual level:**

| Actual Level | App Reading | Difference | Status |
|--------------|-------------|------------|--------|
| Empty | Should be 0-5% | | □ OK □ Adjust |
| 25% full | Should be 20-30% | | □ OK □ Adjust |
| 50% full | Should be 45-55% | | □ OK □ Adjust |
| 75% full | Should be 70-80% | | □ OK □ Adjust |
| Full | Should be 95-100% | | □ OK □ Adjust |

### 3. Fine-Tuning

**If readings are consistently too HIGH:**
- Increase Sensor Offset by 2-5 cm
- Re-provision and test

**If readings are consistently too LOW:**
- Decrease Sensor Offset by 2-5 cm
- Re-provision and test

**If readings are erratic:**
- Check sensor mounting (must be perpendicular)
- Ensure no obstructions in beam path
- Check for water condensation on sensor
- Verify stable power supply

## Sensor Mounting Best Practices

### DO ✓

- Mount sensor directly above water, pointing straight down
- Keep sensor 5-20 cm above maximum water level
- Ensure clear path to water (no pipes, floats, debris)
- Protect sensor from direct water splash
- Use stable mounting bracket

### DON'T ✗

- Mount at an angle
- Place too close to tank walls
- Mount where waves cause interference
- Expose directly to rain/spray
- Use near ultrasonic noise sources

## Typical Tank Configurations

### Underground Sump (Small)
```
Tank Height: 100-150 cm
Tank Diameter: 80-120 cm
Sensor Offset: 5-10 cm (mounted on lid)
Pump ON: 20%
Pump OFF: 90%
```

### Overhead Tank (Medium)
```
Tank Height: 150-250 cm
Tank Diameter: 100-200 cm
Sensor Offset: 10-20 cm (external mounting)
Pump ON: 25%
Pump OFF: 85%
```

### Large Storage Tank
```
Tank Height: 200-400 cm
Tank Diameter: 200-400 cm
Sensor Offset: 15-30 cm
Pump ON: 30%
Pump OFF: 80%
```

## Troubleshooting Calibration Issues

| Problem | Likely Cause | Solution |
|---------|-------------|----------|
| Always shows 0% | Sensor offset too large | Reduce offset value |
| Always shows 100% | Sensor offset too small | Increase offset value |
| Jumps between values | Waves or turbulence | Wait for calm water, add baffle |
| Reads higher than actual | Offset too small | Increase offset |
| Reads lower than actual | Offset too large | Decrease offset |
| Inconsistent readings | Sensor not perpendicular | Adjust mounting angle |
| "Sensor Error" | Blocked or too far | Check mounting, max range 400cm |

## Quick Calibration Card

```
┌─────────────────────────────────────────────────┐
│         CALIBRATION QUICK REFERENCE             │
├─────────────────────────────────────────────────┤
│                                                 │
│  1. Measure tank height (bottom to top)         │
│  2. Mount sensor above tank opening             │
│  3. Measure sensor offset (tank rim to sensor)  │
│  4. Enter values in provisioning app            │
│  5. Verify reading matches actual water level   │
│  6. Adjust offset if needed (±5cm)              │
│                                                 │
│  FORMULA:                                       │
│  Water % = 100 - ((Distance - Offset) / Height) │
│                                                 │
│  TIPS:                                          │
│  - Sensor must point straight down              │
│  - Clear path to water surface                  │
│  - 5-20 cm above max water level                │
│                                                 │
└─────────────────────────────────────────────────┘
```

---

**Version:** 1.0  
**Document:** CALIBRATION_GUIDE.md  
**Product:** Cultivio AquaSense Phase 1

