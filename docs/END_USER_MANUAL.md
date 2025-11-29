# Water Management System - User Manual

## Welcome!

Your Water Management System automatically monitors your water tank and controls pumps to ensure you always have water. Once installed, it works automatically - no daily interaction needed!

---

## What's In The Box

| Item | Purpose |
|------|---------|
| Control Unit (ESP32 box) | The "brain" of the system |
| Water Level Sensor | Mounted on top of your tank |
| Water Flow Sensor | Detects government water supply |
| Status Light Panel | Shows system status at a glance |
| Buzzer/Speaker | Audio alerts |
| Alert Button | Silence alerts |

---

## Understanding The Status Lights

| Light Color | What It Means |
|-------------|---------------|
| 🟢 **Green (Solid)** | Everything is normal. Tank has enough water. |
| 🟡 **Yellow (Solid)** | Low water warning. System is monitoring. |
| 🔴 **Red (Solid)** | Emergency pump is running to fill the tank. |
| 🔵 **Blue (Solid)** | Government water is available and being used. |
| 🔵 **Blue (Blinking)** | System is in Setup Mode (needs configuration). |

---

## Understanding The Sounds

| Sound Pattern | What It Means | Action Needed |
|---------------|---------------|---------------|
| **Single beep** (every 2 seconds) | Government water detected | None - good news! |
| **Medium beeps** (every second) | Low water alert | Water is being filled automatically |
| **Fast beeps** (continuous) | Emergency pump running | Press Alert Button to silence |
| **No sound** | Normal operation | None |

### To Silence an Alert
Press the **Alert Button** once. The buzzer will stop, but the system continues working.

---

## How The System Works

### Normal Operation
1. The sensor checks your water level every 30 seconds
2. When water is low, the system waits for government water
3. If government water arrives → uses the main pump to fill tank
4. If no government water after waiting period → uses emergency pump
5. Pumps automatically stop when tank is sufficiently full

### Automatic Safety Features
- ✅ Pumps cannot run more than 2 hours continuously
- ✅ System prioritizes government water over emergency pump
- ✅ Different settings for dry season vs monsoon season

---

## First-Time Setup

If you see a **blinking blue light**, the system needs to be configured:

### Step 1: Connect to System WiFi
On your phone or laptop:
1. Go to WiFi settings
2. Connect to network: **`WaterSystem-Setup`**
3. Password: **`watersystem123`**

### Step 2: Open Configuration Page
1. Open your web browser (Chrome, Safari, etc.)
2. Go to: **`http://192.168.4.1`**

### Step 3: Enter Your Settings

| Setting | What to Enter |
|---------|---------------|
| **Tank Height** | Height of your tank in centimeters |
| **Tank Diameter** | Width of your tank in centimeters |
| **Season** | Select Dry Season or Monsoon |
| **Power Source** | Mains Power or Battery |

### Step 4: Save
Click **"Save Configuration & Start System"**

The system will restart and begin automatic operation.

---

## Seasonal Adjustment

The system behaves differently based on the season:

| Setting | Dry Season | Monsoon Season |
|---------|------------|----------------|
| Emergency water level | 10% | 15% |
| Wait time for govt water | 2 hours | 4 hours |

### To Change Season:
1. Connect to **WaterSystem-Setup** WiFi
2. Go to **http://192.168.4.1**
3. Change the season setting
4. Click Save

---

## Daily Use - What You Need To Know

### ✅ Normal Day
- Green light is on
- No sounds
- **Do nothing** - system is working automatically

### ⚠️ Low Water Alert
- Yellow light turns on
- Medium beeping starts
- **Do nothing** - system will fill the tank automatically
- Press Alert Button if sound is annoying

### 🔵 Government Water Available
- Blue light turns on
- Single beep every 2 seconds
- **Good news!** System is using government water
- Tank will fill to 90%

### 🔴 Emergency Pump Active
- Red light turns on
- Fast beeping
- This means no government water for extended time
- Emergency pump is filling tank to 60%
- Press Alert Button to silence

---

## Troubleshooting

### No Lights At All
- Check power connection
- Verify the power adapter is plugged in
- Check if there's a power outage

### Blinking Blue Light Won't Stop
- System is stuck in setup mode
- Complete the WiFi setup process (see First-Time Setup)

### Water Level Seems Wrong
- Check if sensor is properly mounted above water
- Ensure nothing is blocking the sensor
- Contact installer if problem persists

### Pump Not Working
- Check if pump is receiving power
- Verify water source is available
- Listen for relay click sound when pump should activate
- Contact installer if no click sound

### Can't Connect to WiFi Setup
- Make sure you're close to the control unit
- Try restarting the control unit (unplug and plug back)
- Ensure no other device is connected to the setup network

---

## Important Safety Information

⚠️ **DO NOT:**
- Open the control unit enclosure
- Touch any electrical connections
- Submerge the control unit in water
- Block ventilation holes on the enclosure

✅ **DO:**
- Keep the control unit dry
- Allow air circulation around the unit
- Contact your installer for any repairs
- Press Alert Button to silence loud alerts

---

## When To Call Your Installer

Call for service if you notice:
- System not responding for more than 1 hour
- Continuous error sounds that won't stop
- Water leaking near sensors or pumps
- Burnt smell from the control unit
- Physical damage to any component

---

## Quick Reference Card

```
┌─────────────────────────────────────────┐
│        WATER MANAGEMENT SYSTEM          │
├─────────────────────────────────────────┤
│                                         │
│  LIGHTS:                                │
│    🟢 Green  = All OK                   │
│    🟡 Yellow = Low Water                │
│    🔴 Red    = Emergency Pump On        │
│    🔵 Blue   = Govt Water Available     │
│                                         │
│  SOUNDS:                                │
│    Slow beep = Govt water detected      │
│    Medium beep = Low water alert        │
│    Fast beep = Emergency pump running   │
│                                         │
│  BUTTON:                                │
│    Press once = Silence alert           │
│                                         │
│  SETUP WiFi: WaterSystem-Setup          │
│  PASSWORD: watersystem123               │
│  WEB PAGE: http://192.168.4.1           │
│                                         │
└─────────────────────────────────────────┘
```

---

## Contact Information

**Installer:** ________________________

**Phone:** ________________________

**Installation Date:** ________________________

**Notes:**
_________________________________________________
_________________________________________________
_________________________________________________

---

*Thank you for choosing our Water Management System!*

