# Cultivio AquaSense
## End User Manual

### Simple Guide for Daily Use

---

## 🏠 What This System Does

Your Cultivio AquaSense water management system automatically monitors your water tank and controls the pump:

- **Monitors** water level 24/7
- **Turns ON** the pump when water is low
- **Turns OFF** the pump when tank is full
- **Prevents** pump damage with safety timeout
- **View Status** on your phone anytime via Bluetooth

You don't need to do anything - it works automatically!

---

## 📍 System Components

### Two Small Boxes

| Box | Location | Purpose |
|-----|----------|---------|
| **Sensor Box** | Near/on water tank | Measures water level |
| **Controller Box** | Near water pump | Controls the pump |

---

## 💡 LED Lights Explained

### Sensor Box LEDs

| Light | Meaning |
|-------|---------|
| 🔵 **Blue blinking slowly** | Normal - measuring water |
| 🔵 **Blue solid** | Just sent data |
| ⚪ **White blinking fast** | Setup mode (needs configuration) |
| 🔴 **No lights** | Check power connection |

### Controller Box LEDs

| Light | Meaning |
|-------|---------|
| 🔵 **Blue solid** | Everything OK - connected to sensor |
| 🔵 **Blue blinking** | Waiting for sensor |
| 🔴 **Red ON** | Pump is running |
| 🔴 **Red OFF** | Pump is off |
| ⚪ **White blinking fast** | Setup mode |

---

## ✅ Normal Operation

When everything is working correctly:

```
Sensor Box:  🔵 Slow blink (measuring)
Controller:  🔵 Solid + 🔴 Off (pump waiting)

When water is low:
Controller:  🔵 Solid + 🔴 ON (pump running)

When tank is full:
Controller:  🔵 Solid + 🔴 OFF (pump stopped)
```

---

## ⚠️ Warning Signs

### Call for Help If:

| Situation | What It Means |
|-----------|---------------|
| All LEDs off | Power problem |
| Controller LED blinking for hours | Sensor disconnected |
| Pump runs for very long time | May have a leak |
| Water not filling | Check pump/pipes |

---

## 🔧 Simple Troubleshooting

### Pump Not Starting?

1. ✅ Check if controller has power (any LED on?)
2. ✅ Check if sensor has power (any LED on?)
3. ✅ Wait 5 minutes for system to reconnect
4. ✅ Check if water is actually low

### Pump Won't Stop?

1. ✅ System has safety timeout (max 60 minutes)
2. ✅ Check if tank is actually filling
3. ✅ If pump runs > 60 min, system will auto-stop

### No LED Lights?

1. ✅ Check USB power cable is connected
2. ✅ Check power adapter is working
3. ✅ Try different USB cable

### Both Boxes Blinking White?

This means setup mode. Your system needs configuration:
1. Contact your installer, OR
2. Follow the provisioning guide

---

## 📱 How to Change Settings

If you need to change pump thresholds or tank settings:

1. **Find the button** on the box (small push button)
2. **Hold it for 3 seconds** - white LED will blink
3. **Open provisioning app** on your phone/computer
4. **Connect and change settings**
5. **Save** - device will restart

---

## 🔋 Power Options

### USB Power (Default)
- Connect USB-C cable to power adapter
- Use 5V 2A adapter (like phone charger)
- System runs 24/7

### Battery Power (Optional)
- Uses rechargeable battery
- May need recharging monthly
- Solar panel option available

---

## 📊 Understanding Water Levels

Your system is configured with these levels:

```
100% ═══════════════════ Tank Full
 │
 │   Pump turns OFF here (default 80%)
80% ─ ─ ─ ─ ─ ─ ─ ─ ─ ─
 │
 │   Normal operating range
 │
20% ─ ─ ─ ─ ─ ─ ─ ─ ─ ─
 │   Pump turns ON here (default 20%)
 │
 0% ═══════════════════ Tank Empty
```

---

## 🗓️ Maintenance Schedule

### Weekly
- [ ] Glance at LEDs to confirm system is working

### Monthly
- [ ] Wipe ultrasonic sensor clean
- [ ] Check for any water near electronics

### Yearly
- [ ] Have installer check all connections
- [ ] Update firmware if available

---

## 📞 When to Call Your Installer

Call if you notice:
- System not working for more than a day
- Water overflowing despite sensor
- Pump running continuously
- Strange noises from pump
- Any electrical issues

---

## 📱 Check Status on Your Phone

You can check your water level and pump status anytime using your phone!

### How to View Status

1. **Open the Cultivio App** - Open `provisioning-app/index.html` in Chrome or Edge browser on your phone
2. **Tap "Scan for Devices"** - Your phone will search for nearby devices
3. **Select Your Device** - Look for `Cultivio-XXXX` (your device name)
4. **View Live Status** - See real-time information:
   - 💧 **Water Level** - Current percentage and visual gauge
   - ⚡ **Pump Status** - Running, Off, or Manual
   - 📶 **Connection Status** - Zigbee network status
   - ⏱️ **Uptime** - How long since last restart

### Status Screen

```
┌─────────────────────────────────────┐
│      💧 Water Level: 75%            │
│      ████████████░░░░               │
│                                     │
│   ⚡ Pump: OFF    📶 Network: OK    │
│   ⏱️ Uptime: 2h 34m 12s            │
└─────────────────────────────────────┘
```

**Tip:** The app auto-refreshes every 5 seconds when connected!

---

## 🔧 Manual Pump Control (Emergency Override)

If the sensor goes down or you need to run the pump manually, you can do it from your phone!

### When to Use Manual Control

- 🚨 Sensor is offline or broken
- 🔧 Testing the pump
- 💧 Need extra water quickly

### How to Start Pump Manually

1. **Connect to Controller Node** via the Cultivio app
2. **Scroll to "Manual Pump Control"** section
3. **Select duration**: 10, 15, 20, 30, 45, or 60 minutes
4. **Tap the time button** - Pump will start immediately
5. **Watch the countdown** - Shows time remaining

```
┌─────────────────────────────────────┐
│    ⚠️ MANUAL PUMP CONTROL           
│                                     │
│   ┌──────┐ ┌──────┐ ┌──────┐        │
│   │10 min│ │15 min│ │20 min│        │
│   └──────┘ └──────┘ └──────┘        │
│   ┌──────┐ ┌──────┐ ┌──────┐        │
│   │30 min│ │45 min│ │60 min│        │
│   └──────┘ └──────┘ └──────┘        │
│                                     │
│   ┌─────────────────────────────┐   │
│   │        ⏹️ STOP PUMP         │   │
│   └─────────────────────────────┘   │
└─────────────────────────────────────┘
```

### How to Stop Pump

- **Tap "STOP PUMP"** button anytime
- Or wait for timer to expire automatically

### Safety Features

- ⏱️ **Auto-Stop**: Pump automatically stops when timer expires
- 🔄 **Returns to Auto**: After manual mode ends, system returns to automatic control
- 📱 **Visual Indicator**: Pump shows "MANUAL" status in orange

---

## ❓ Frequently Asked Questions

**Q: What if power goes out?**
A: System will restart automatically when power returns. You don't need to do anything.

**Q: Will it work during monsoon?**
A: Yes! The enclosures are weather-resistant. Just ensure no direct water spray on boxes.

**Q: How do I know if pump is actually running?**
A: Red LED on controller box = pump is ON. You should also hear the pump.

**Q: Can I turn off the system?**
A: Simply unplug the USB power. Pump will stop. Plug back in to restart.

**Q: What's the range between sensor and controller?**
A: Up to 10 meters indoors, 30+ meters outdoors with clear line of sight.

**Q: Can I check status on my phone?**
A: Yes! Open the Cultivio app in Chrome/Edge, tap "Scan for Devices", and connect to your device to see live water level and pump status.

**Q: Do I need internet to check status?**
A: No! The app uses Bluetooth to connect directly to your device. No internet required.

**Q: What if the sensor breaks? Can I still fill the tank?**
A: Yes! Use Manual Pump Control in the app. Connect to the Controller, select a duration (10-60 min), and the pump will run for that time then stop automatically.

**Q: Will the pump run forever in manual mode?**
A: No, it's safe! Manual mode has a timer (max 60 min). The pump stops automatically when time expires.

**Q: There are many Cultivio devices in my building. How do I find mine?**
A: Each device has a unique name. During setup, you can set a custom name like "Cultivio-Flat301" to easily identify your device.

**Q: Can my neighbor control my pump by mistake?**
A: No, if you enable password protection! Set a password during setup and only you can connect.

---

## 🏢 Multi-Device Buildings (Device Identification)

If your building has multiple Cultivio devices (one per flat), here's how to identify yours:

### Custom Device Name

Give your device a unique, memorable name:

```
Default name:     Cultivio-A3B2  (hard to remember)
Custom name:      Cultivio-Flat301  (easy!)
```

**Name Examples:**
- `Cultivio-Flat301` - Flat 301
- `Cultivio-3rdFloor` - 3rd Floor
- `Cultivio-TankA` - Tank A
- `Cultivio-BuildingB` - Building B

### Password Protection

**Default Password: `1234`**

⚠️ **Important:** Change the password after first setup!

When password is enabled:
- Only you can connect to your device
- Neighbors cannot accidentally control your pump

### How to Set Name & Password

1. **Open Provisioning App**
2. **Connect to your device**
3. **In "Device Identification" section:**
   - Enter Custom Name (e.g., Flat301)
   - Change Password
   - Check "Require password to connect"
4. **Save configuration**

### Example: 5-Story Building

```
┌─────────────────────────────────────────────┐
│  BUILDING WITH MULTIPLE CULTIVIO DEVICES    │
├─────────────────────────────────────────────┤
│                                             │
│  5th Floor:  Cultivio-Flat501  🔒           │
│  4th Floor:  Cultivio-Flat401  🔒           │
│  3rd Floor:  Cultivio-Flat301  🔒 ← Yours   │
│  2nd Floor:  Cultivio-Flat201  🔒           │
│  1st Floor:  Cultivio-Flat101  🔒           │
│                                             │
│  When scanning, all devices appear.         │
│  But only YOUR password works on YOUR device│
│                                             │
└─────────────────────────────────────────────┘
```

---

## 📋 Quick Reference Card

```
┌─────────────────────────────────────────────────┐
│        CULTIVIO AQUASENSE SYSTEM                │
├─────────────────────────────────────────────────┤
│                                                 │
│  SENSOR BOX:                                    │
│  🔵 Blinking = OK, measuring                    │
│  ⚪ Fast blink = Setup mode                     │
│                                                 │
│  CONTROLLER BOX:                                │
│  🔵 Solid = Connected to sensor                 │
│  🔵 Blinking = Waiting for sensor               │
│  🔴 ON = Pump running                           │
│  🔴 OFF = Pump stopped                          │
│                                                 │
│  MOBILE APP FEATURES:                           │
│  📱 Check Status → Scan → Connect               │
│  🔧 Manual Pump → Select time → Start           │
│                                                 │
│  DEVICE IDENTIFICATION:                         │
│  📍 Custom name: Cultivio-YourName              │
│  🔒 Default password: 1234                      │
│                                                 │
│  EMERGENCY PUMP CONTROL:                        │
│  If sensor offline, use app to run pump         │
│  manually for 10-60 min                         │
│                                                 │
│  NEED HELP?                                     │
│  Hold button 3 sec = Enter setup mode           │
│  All LEDs off = Check power                     │
│                                                 │
│  MY DEVICE: ______________________              │
│  MY PASSWORD: ____________________              │
│                                                 │
└─────────────────────────────────────────────────┘
```

Print this card and keep it near your controller box!

---

**Version:** 2.1  
**Date:** December 2024

*For technical details, see USER_GUIDE.md*
