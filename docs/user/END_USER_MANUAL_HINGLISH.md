# Cultivio AquaSense
## User Manual (Hinglish)

### Rozana Use ke liye Simple Guide

---

## 🏠 Yeh System Kya Karta Hai?

Aapka Cultivio AquaSense water management system automatically aapki tank ko monitor karta hai aur pump control karta hai:

- **24/7 Monitor** karta hai water level ko
- **Pump ON** karta hai jab paani kam ho
- **Pump OFF** karta hai jab tank full ho jaye
- **Pump ko bachata hai** safety timeout se
- **Phone pe Status dekho** kabhi bhi Bluetooth se

Aapko kuch karna nahi hai - yeh automatic kaam karta hai!

---

## 📍 System ke Parts

### Do Chhote Boxes

| Box | Kahan Lagega | Kya Karta Hai |
|-----|--------------|---------------|
| **Sensor Box** | Tank ke paas/upar | Paani level measure karta hai |
| **Controller Box** | Pump ke paas | Pump control karta hai |

---

## 💡 LED Lights ka Matlab

### Sensor Box ki LEDs

| Light | Matlab |
|-------|--------|
| 🔵 **Blue slow blink** | Sab theek - paani measure ho raha hai |
| 🔵 **Blue solid** | Abhi data send hua |
| ⚪ **White fast blink** | Setup mode (configuration chahiye) |
| 🔴 **Koi light nahi** | Power connection check karo |

### Controller Box ki LEDs

| Light | Matlab |
|-------|--------|
| 🔵 **Blue solid** | Sab theek - sensor se connected |
| 🔵 **Blue blink** | Sensor ka wait kar raha hai |
| 🔴 **Red ON** | Pump chal raha hai |
| 🔴 **Red OFF** | Pump band hai |
| ⚪ **White fast blink** | Setup mode |

---

## ✅ Normal Operation

Jab sab kuch sahi kaam kar raha ho:

```
Sensor Box:  🔵 Slow blink (measuring)
Controller:  🔵 Solid + 🔴 Off (pump wait kar raha hai)

Jab paani kam ho:
Controller:  🔵 Solid + 🔴 ON (pump chal raha hai)

Jab tank full ho:
Controller:  🔵 Solid + 🔴 OFF (pump band ho gaya)
```

---

## ⚠️ Warning Signs - Dhyan Do!

### Help ke liye Call Karo Agar:

| Situation | Matlab |
|-----------|--------|
| Saari LEDs band | Power problem hai |
| Controller LED ghanton se blink kar rahi | Sensor disconnect ho gaya |
| Pump bahut der se chal raha | Leak ho sakti hai |
| Paani nahi bhar raha | Pump/pipe check karo |

---

## 🔧 Simple Troubleshooting

### Pump Start Nahi Ho Raha?

1. ✅ Controller me power hai? (Koi LED on hai?)
2. ✅ Sensor me power hai? (Koi LED on hai?)
3. ✅ 5 minute wait karo reconnect hone ke liye
4. ✅ Check karo paani sach me kam hai ya nahi

### Pump Band Nahi Ho Raha?

1. ✅ System me safety timeout hai (max 60 minute)
2. ✅ Check karo tank sach me bhar raha hai ya nahi
3. ✅ Agar pump 60 min se zyada chale, system auto-stop karega

### Koi LED Nahi Jal Rahi?

1. ✅ USB power cable connected hai check karo
2. ✅ Power adapter kaam kar raha hai check karo
3. ✅ Doosri USB cable try karo

### Dono Box White Blink Kar Rahe Hain?

Iska matlab setup mode hai. System ko configuration chahiye:
1. Installer ko contact karo, YA
2. Provisioning guide follow karo

---

## 📱 Phone Se Settings Kaise Change Karein

Agar pump thresholds ya tank settings change karni hai:

1. **Button dhundho** box pe (chhota push button)
2. **3 second dabake rakho** - white LED blink hogi
3. **Provisioning app kholo** phone/computer pe
4. **Connect karo aur settings change karo**
5. **Save karo** - device restart hoga

---

## 🔋 Power Options

### USB Power (Default)
- USB-C cable power adapter se connect karo
- 5V 2A adapter use karo (phone charger jaisa)
- System 24/7 chalta hai

### Battery Power (Optional)
- Rechargeable battery use hoti hai
- Monthly recharge karna pad sakta hai
- Solar panel option bhi available hai

---

## 📊 Water Levels Samjho

Aapka system in levels pe set hai:

```
100% ═══════════════════ Tank Full
 │
 │   Pump OFF yahan hoga (default 80%)
80% ─ ─ ─ ─ ─ ─ ─ ─ ─ ─
 │
 │   Normal operating range
 │
20% ─ ─ ─ ─ ─ ─ ─ ─ ─ ─
 │   Pump ON yahan hoga (default 20%)
 │
 0% ═══════════════════ Tank Empty
```

---

## 🗓️ Maintenance Schedule

### Weekly (Har Hafte)
- [ ] LEDs dekho confirm karne ke liye system kaam kar raha hai

### Monthly (Har Mahine)
- [ ] Ultrasonic sensor saaf karo
- [ ] Check karo electronics ke paas paani toh nahi

### Yearly (Har Saal)
- [ ] Installer se saare connections check karwao
- [ ] Firmware update available ho toh karo

---

## 📞 Installer Ko Kab Call Karein

Call karo agar notice ho:
- System ek din se zyada kaam nahi kar raha
- Tank overflow ho raha hai sensor hone ke baad bhi
- Pump continuously chal raha hai
- Pump se strange awaaz aa rahi hai
- Koi bhi electrical issue

---

## 📱 Phone Se Status Kaise Dekhen

Aap kabhi bhi apna water level aur pump status phone pe dekh sakte ho!

### Status Kaise Dekhen

1. **Cultivio App kholo** - Chrome ya Edge browser me `provisioning-app/index.html` kholo
2. **"Scan for Devices" tap karo** - Phone nearby devices dhundhega
3. **Apna Device Select karo** - `Cultivio-XXXX` dekho (aapke device ka naam)
4. **Live Status dekho** - Real-time information:
   - 💧 **Water Level** - Current percentage aur visual gauge
   - ⚡ **Pump Status** - Running, Off, ya Manual
   - 📶 **Connection Status** - Zigbee network status
   - ⏱️ **Uptime** - Last restart ke baad kitna time

### Status Screen

```
┌─────────────────────────────────────┐
│      💧 Water Level: 75%            │
│      ████████████░░░░               │
│                                     │
│   ⚡ Pump: OFF    📶 Network: OK    │
│   ⏱️ Uptime: 2h 34m 12s            │
│                                     │
│   📶 Signal: 85% (Excellent)        │
└─────────────────────────────────────┘
```

**Tip:** App har 5 second me auto-refresh hota hai jab connected ho!

---

## 🔧 Manual Pump Control (Emergency Override)

Agar sensor kharab ho jaye ya pump manually chalana ho, phone se kar sakte ho!

### Manual Control Kab Use Karein

- 🚨 Sensor offline ya kharab hai
- 🔧 Pump test karna hai
- 💧 Jaldi extra paani chahiye

### Pump Manually Kaise Start Karein

1. **Controller Node se Connect karo** Cultivio app se
2. **"Manual Pump Control"** section tak scroll karo
3. **Duration select karo**: 10, 15, 20, 30, 45, ya 60 minute
4. **Time button tap karo** - Pump turant start hoga
5. **Countdown dekho** - Remaining time dikhega

```
┌─────────────────────────────────────┐
│    ⚠️ MANUAL PUMP CONTROL           │
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

### Pump Kaise Band Karein

- **"STOP PUMP"** button kabhi bhi tap karo
- Ya timer apne aap expire hone do

### Safety Features

- ⏱️ **Auto-Stop**: Timer expire hone pe pump automatic band
- 🔄 **Auto Mode Wapas**: Manual mode ke baad automatic control wapas
- 📱 **Visual Indicator**: Pump "MANUAL" status orange me dikhega

---

## ❓ Aksar Pooche Jane Wale Sawal (FAQ)

**Q: Light chali gayi toh?**
A: Light aane pe system automatic restart hoga. Aapko kuch nahi karna.

**Q: Monsoon me kaam karega?**
A: Haan! Enclosures weather-resistant hain. Bas direct water spray mat aane do boxes pe.

**Q: Pump sach me chal raha hai kaise pata?**
A: Controller box pe Red LED ON = pump chal raha hai. Pump ki awaaz bhi sunai degi.

**Q: System band kar sakta hoon?**
A: USB power unplug kar do. Pump band ho jayega. Wapas plug karo restart ke liye.

**Q: Sensor aur controller ke beech kitni door ho sakti hai?**
A: Indoor 10 meter tak, outdoor 30+ meter (clear line of sight me).

**Q: Phone pe status dekh sakta hoon?**
A: Haan! Chrome/Edge me Cultivio app kholo, "Scan for Devices" tap karo, aur device se connect karo live water level aur pump status dekhne ke liye.

**Q: Status check karne ke liye internet chahiye?**
A: Nahi! App Bluetooth use karta hai directly device se connect hone ke liye. Internet zaruri nahi.

**Q: Sensor toot gaya toh? Tank kaise bharun?**
A: Manual Pump Control use karo app me. Controller se connect karo, duration select karo (10-60 min), pump utne time chalega phir automatic band.

**Q: Manual mode me pump hamesha chalega?**
A: Nahi, safe hai! Manual mode me timer hai (max 60 min). Time khatam hone pe pump automatic band.

**Q: Signal weak hai toh kya karein?**
A: App me signal strength dekho. Agar 40% se kam hai, Router Node add karne ka sochlo beech me.

---

## 📋 Quick Reference Card

```
┌─────────────────────────────────────────────────┐
│        CULTIVIO AQUASENSE SYSTEM                │
├─────────────────────────────────────────────────┤
│                                                 │
│  SENSOR BOX:                                    │
│  🔵 Blink = Theek hai, measure ho raha          │
│  ⚪ Fast blink = Setup mode                     │
│                                                 │
│  CONTROLLER BOX:                                │
│  🔵 Solid = Sensor se connected                 │
│  🔵 Blink = Sensor ka wait                      │
│  🔴 ON = Pump chal raha                         │
│  🔴 OFF = Pump band                             │
│                                                 │
│  MOBILE APP FEATURES:                           │
│  📱 Status Dekho → Scan → Connect               │
│  🔧 Manual Pump → Time Select → Start           │
│                                                 │
│  DEVICE IDENTIFICATION:                         │
│  📍 Custom naam: Cultivio-AapkaNaam             │
│  🔒 Default password: 1234                      │
│                                                 │
│  EMERGENCY PUMP CONTROL:                        │
│  Agar sensor offline, app se pump chalo         │
│  manually 10-60 min ke liye                     │
│                                                 │
│  HELP CHAHIYE?                                  │
│  Button 3 sec dabao = Setup mode                │
│  Saari LED band = Power check karo              │
│                                                 │
│  MERA DEVICE: ________________________          │
│  MERA PASSWORD: ______________________          │
│  INSTALLER: __________________________          │
│                                                 │
└─────────────────────────────────────────────────┘
```

Yeh card print karo aur controller box ke paas rakho!

---

## 🏗️ Building ke Hisab se Setup

### 1-3 Floor Building (2 Devices)

```
┌─────────────────────────────────────────────┐
│                                             │
│   TERRACE:  [Sensor] Tank ke paas           │
│       │                                     │
│       │ Zigbee Signal                       │
│       │ (Direct Connection)                 │
│       ▼                                     │
│   GROUND:   [Controller] Pump ke paas       │
│                                             │
│   ✅ 2 devices kaafi hain!                  │
└─────────────────────────────────────────────┘
```

### 4-5 Floor Building (3 Devices)

```
┌─────────────────────────────────────────────┐
│                                             │
│   TERRACE:  [Sensor] Tank ke paas           │
│       │                                     │
│       │ Zigbee Signal                       │
│       ▼                                     │
│   MIDDLE:   [Router] Range extend karta hai │
│       │                                     │
│       │ Zigbee Signal                       │
│       ▼                                     │
│   GROUND:   [Controller] Pump ke paas       │
│                                             │
│   ✅ 3 devices for better signal!           │
└─────────────────────────────────────────────┘
```

---

## 💰 Cost (Approx.)

| Setup | Devices | Price |
|-------|---------|-------|
| 1-3 Floor | 2 | ₹1,700 - ₹2,000 |
| 4-5 Floor | 3 | ₹2,200 - ₹2,500 |

---

---

## 🏢 Ek Building Me Multiple Devices (Device Identification)

Agar aapki building me bahut saare Cultivio devices hain (har flat me ek), toh apna device kaise pehchane?

### Custom Device Name

Har device ko unique naam de sakte ho:

```
Default naam:     Cultivio-A3B2  (yaad rakhna mushkil)
Custom naam:      Cultivio-Flat301  (aasan!)
```

**Examples:**
- `Cultivio-Flat301` - Flat 301
- `Cultivio-3rdFloor` - 3rd Floor  
- `Cultivio-TankA` - Tank A

### Password Protection

**Default Password: `1234`**

⚠️ **Important:** Pehli baar setup ke baad password change karo!

Agar password enable karoge:
- Sirf aap hi apne device se connect kar sakte ho
- Padosi galti se aapka pump control nahi kar payenge

### Naam aur Password Kaise Set Karein

1. **Provisioning App kholo**
2. **Device se connect karo**
3. **"Device Identification" section me:**
   - Custom Name dalo (e.g., Flat301)
   - Password change karo
   - "Require password" enable karo
4. **Save karo**

### Building Example

```
┌─────────────────────────────────────────────┐
│  5-STORY BUILDING WITH MULTIPLE DEVICES     │
├─────────────────────────────────────────────┤
│                                             │
│  5th Floor:  Cultivio-Flat501  🔒           │
│  4th Floor:  Cultivio-Flat401  🔒           │
│  3rd Floor:  Cultivio-Flat301  🔒 ← Aapka   │
│  2nd Floor:  Cultivio-Flat201  🔒           │
│  1st Floor:  Cultivio-Flat101  🔒           │
│                                             │
│  Scan karne pe saare devices dikhenge,      │
│  lekin connect sirf apne password se!       │
│                                             │
└─────────────────────────────────────────────┘
```

---

**Version:** 2.1  
**Date:** December 2024

*Technical details ke liye, USER_GUIDE.md dekho*

---

**© 2024 Cultivio - Smart Water Management**

