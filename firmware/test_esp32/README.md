# Cultivio AquaSense - Test Firmware for ESP32-WROOM

## 🧪 Purpose

This test firmware allows you to validate the **BLE provisioning** and **controller logic** on a standard **ESP32-WROOM** board while waiting for ESP32-H2 hardware.

## ✅ What You Can Test

| Feature | Status | Notes |
|---------|--------|-------|
| BLE GATT Server | ✅ Works | Full implementation |
| Mobile App Connection | ✅ Works | Same provisioning app |
| Device Role Selection | ✅ Works | Sensor/Controller/Router |
| Tank Configuration | ✅ Works | Height, diameter, offset |
| Pump Configuration | ✅ Works | Thresholds, timeout |
| Custom Device Name | ✅ Works | Cultivio-YourName |
| Password Protection | ✅ Works | Set via app |
| Manual Pump Control | ✅ Works | LED simulates relay |
| Status Monitoring | ✅ Works | Simulated values |
| NVS Storage | ✅ Works | Config persists |
| Factory Reset | ✅ Works | Hold button 3 sec |
| Zigbee Communication | ❌ Simulated | No actual Zigbee on ESP32 |

## 🔧 Hardware Setup

### Minimal (Just ESP32-WROOM)

```
ESP32-WROOM Board
├── GPIO2  → Built-in LED (simulates pump relay)
├── GPIO0  → Boot button (factory reset)
└── USB    → Power + Serial monitor
```

### Optional (Better Testing)

```
ESP32-WROOM Board
├── GPIO2  → External LED + 330Ω resistor (pump indicator)
├── GPIO4  → Status LED + 330Ω resistor (heartbeat)
├── GPIO0  → Boot button
└── USB    → Power + Serial monitor
```

## 🚀 Build & Flash

### 1. Open ESP-IDF Terminal

```powershell
# Run ESP-IDF export script
C:\Espressif\frameworks\esp-idf-v5.3.1\export.ps1
```

### 2. Navigate to Test Firmware

```powershell
cd C:\test\iot-water-managment\firmware\test_esp32
```

### 3. Set Target & Build

```powershell
# Set target to ESP32 (not ESP32-H2!)
idf.py set-target esp32

# Build firmware
idf.py build
```

### 4. Flash to ESP32

```powershell
# Find your COM port (e.g., COM3)
idf.py -p COM3 flash

# Monitor serial output
idf.py -p COM3 monitor

# Or both together
idf.py -p COM3 flash monitor
```

## 📱 Testing with Provisioning App

### 1. Open the Provisioning App

```
Open in Chrome/Edge:
C:\test\iot-water-managment\provisioning-app\index.html
```

### 2. Connect to Device

1. Click **"Scan for Devices"**
2. Select **"Cultivio-XXXX"** from the list
3. Click **"Pair"**

### 3. Test Configuration

1. **Select Role:** Controller (for pump testing)
2. **Set Custom Name:** e.g., "TestDev"
3. **Configure Pump:**
   - ON Threshold: 20%
   - OFF Threshold: 90%
   - Timeout: 30 min
4. Click **"Save & Activate"**

### 4. Test Manual Pump Control

1. View **Status Dashboard** (water level, pump state)
2. Click **Manual Pump Control**
3. Select duration (e.g., 10 min)
4. Click **Start Pump**
5. Watch GPIO2 LED turn ON
6. Click **Stop Pump** to turn OFF

## 🖥️ Serial Monitor Output

```
╔═══════════════════════════════════════════════════════╗
║     CULTIVIO AQUASENSE - TEST FIRMWARE                ║
║     For ESP32-WROOM (BLE Provisioning Test)           ║
╚═══════════════════════════════════════════════════════╝

=== TEST FIRMWARE READY ===

Hardware:
  - Pump LED (simulated relay): GPIO2
  - Status LED: GPIO4
  - Button: GPIO0

BLE Device: Cultivio-A3B2

Features to test:
  1. Open provisioning app in Chrome/Edge
  2. Scan for 'Cultivio-A3B2'
  3. Connect and configure device role
  4. Set tank/pump parameters
  5. Test manual pump control
  6. Monitor simulated water level

Simulated behavior:
  - Water level decreases slowly (usage)
  - Pump turns ON when level <= 20%
  - Pump turns OFF when level >= 90%
  - GPIO2 LED indicates pump state

Status: Water=45%, Pump=OFF, Connected=No, Uptime=10s
Status: Water=44%, Pump=OFF, Connected=Yes, Uptime=20s
Config command: 0x00, len: 2
Role set to: CONTROLLER
*** PROVISIONING COMPLETE ***
```

## 🔄 Simulated Behavior

### Water Level Simulation

```
1. Water level starts at 50%
2. Level decreases by 1% every 5 seconds (simulates usage)
3. When level <= 20% (ON threshold):
   → Pump turns ON automatically
   → GPIO2 LED lights up
4. Level increases by 2% every 5 seconds (pump filling)
5. When level >= 90% (OFF threshold):
   → Pump turns OFF automatically
   → GPIO2 LED turns off
6. Cycle repeats
```

### Manual Pump Control

```
1. User selects duration (10-60 min) in app
2. Pump turns ON immediately
3. Countdown timer starts
4. When timer expires:
   → Pump turns OFF
   → Manual mode ends
5. User can click "Stop" anytime
```

## 🔧 Factory Reset

Hold **GPIO0 button** for **3 seconds**:
- All configuration erased
- Device restarts
- Returns to default state

## 📊 Differences from Production Firmware

| Aspect | Test (ESP32) | Production (ESP32-H2) |
|--------|--------------|----------------------|
| **BLE** | Bluedroid stack | NimBLE stack |
| **Zigbee** | Simulated (always connected) | Real Zigbee mesh |
| **Water Level** | Simulated (random) | Real ultrasonic sensor |
| **Pump Control** | LED on GPIO2 | Relay module |
| **Multi-device** | Single device | Mesh network |

## ✅ Test Checklist

- [ ] Firmware builds without errors
- [ ] Flash successful
- [ ] BLE advertising starts (see serial log)
- [ ] Device appears in provisioning app scan
- [ ] Can connect via BLE
- [ ] Role selection works
- [ ] Tank config saves
- [ ] Pump config saves
- [ ] Custom name updates device name
- [ ] Status dashboard shows data
- [ ] Manual pump ON works (LED lights)
- [ ] Manual pump OFF works (LED off)
- [ ] Manual timer countdown works
- [ ] Config persists after restart
- [ ] Factory reset clears config

## ❓ Troubleshooting

### Device Not Found in Scan

1. Check serial monitor for "BLE advertising started"
2. Ensure Bluetooth is enabled on phone/computer
3. Try refreshing the page and scanning again

### Connection Fails

1. Disconnect any existing connections
2. Restart the ESP32
3. Clear browser Bluetooth cache

### LED Not Working

1. Verify GPIO2 is the correct pin for your board
2. Some boards have inverted LED logic
3. Check wiring if using external LED

### Build Errors

```powershell
# Clean and rebuild
idf.py fullclean
idf.py build
```

---

## 🎯 Next Steps

After validating with ESP32-WROOM:

1. ✅ Provisioning app works correctly
2. ✅ All configurations save properly
3. ✅ Manual pump control responds
4. ✅ Status updates display correctly

→ You're ready for **ESP32-H2** with real Zigbee!

---

*Test Firmware v1.0 - December 2024*

