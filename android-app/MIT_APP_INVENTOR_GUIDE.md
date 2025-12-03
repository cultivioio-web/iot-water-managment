# Cultivio Android App - MIT App Inventor Guide

## 🎯 Create Android App WITHOUT Coding!

MIT App Inventor is a free, visual tool to create Android apps with Bluetooth support.

---

## Step 1: Open MIT App Inventor

1. Go to: https://appinventor.mit.edu/
2. Click "Create Apps!"
3. Sign in with Google account
4. Click "Start new project"
5. Name it: `CultivioAquaSense`

---

## Step 2: Design the UI

### Screen1 Properties:
- Title: `Cultivio AquaSense`
- BackgroundColor: `Dark Gray`
- ScreenOrientation: `Portrait`

### Add Components (from Palette):

```
Layout:
├── VerticalArrangement (main container)
│   ├── Label (title): "💧 Cultivio AquaSense"
│   ├── Label (status): "Not Connected"
│   ├── Button (scan): "🔍 Scan Devices"
│   ├── ListPicker (devices): "Select Device"
│   ├── HorizontalArrangement
│   │   ├── Label: "Water Level:"
│   │   └── Label (waterLevel): "-- %"
│   ├── HorizontalArrangement
│   │   ├── Label: "Pump Status:"
│   │   └── Label (pumpStatus): "OFF"
│   ├── Button (startPump): "▶️ Start Pump (10 min)"
│   ├── Button (stopPump): "⏹️ Stop Pump"
│   └── Label (timer): ""
│
├── BluetoothLE (from Connectivity)
├── Clock (from Sensors) - for timer
└── Notifier (from User Interface)
```

---

## Step 3: Blocks (Logic)

### When Scan Button Clicked:
```
when ScanButton.Click
  call BluetoothLE1.StartScanning
```

### When Device Found:
```
when BluetoothLE1.DeviceFound
  if deviceName contains "Cultivio"
    add deviceName to DeviceListPicker.Elements
```

### When Device Selected:
```
when DeviceListPicker.AfterPicking
  call BluetoothLE1.Connect (address)
  set StatusLabel.Text to "Connecting..."
```

### When Connected:
```
when BluetoothLE1.Connected
  set StatusLabel.Text to "Connected!"
  call BluetoothLE1.ReadBytes (serviceUUID, charUUID)
```

### Read Water Level:
```
when BluetoothLE1.BytesReceived
  set waterLevel = get byte at index 2
  set WaterLevelLabel.Text to waterLevel + "%"
  
  set pumpState = get byte at index 3
  if pumpState > 0
    set PumpStatusLabel.Text to "ON 🟢"
  else
    set PumpStatusLabel.Text to "OFF ⚪"
```

### Start Pump Button:
```
when StartPumpButton.Click
  call BluetoothLE1.WriteBytes
    serviceUUID: "000000ff-0000-1000-8000-00805f9b34fb"
    charUUID: "0000ff03-0000-1000-8000-00805f9b34fb"
    bytes: [1, 10, 0]  // Command: Start, 10 minutes
```

### Stop Pump Button:
```
when StopPumpButton.Click
  call BluetoothLE1.WriteBytes
    serviceUUID: "000000ff-0000-1000-8000-00805f9b34fb"
    charUUID: "0000ff03-0000-1000-8000-00805f9b34fb"
    bytes: [0, 0, 0]  // Command: Stop
```

---

## Step 4: BLE UUIDs for Cultivio

```
Service UUID:    000000ff-0000-1000-8000-00805f9b34fb
Config Char:     0000ff01-0000-1000-8000-00805f9b34fb
Status Char:     0000ff02-0000-1000-8000-00805f9b34fb
Command Char:    0000ff03-0000-1000-8000-00805f9b34fb
```

---

## Step 5: Build APK

1. Click "Build" menu
2. Select "Android App (.apk)"
3. Wait for build (1-2 minutes)
4. Download APK
5. Send to phone via WhatsApp/Email
6. Install on phone!

---

## Step 6: Install on Phone

1. Open APK file on phone
2. Allow "Install from unknown sources"
3. Install app
4. Open "Cultivio AquaSense"
5. Grant Bluetooth & Location permissions
6. Scan and connect!

---

## 📱 Result:

You get a REAL Android app:
- Icon on home screen
- Works with native Bluetooth
- No browser needed!
- Can share APK file with customers

---

## 🔗 Resources:

- MIT App Inventor: https://appinventor.mit.edu/
- BLE Extension: Built-in (BluetoothLE component)
- Tutorials: https://appinventor.mit.edu/explore/ai2/tutorials

---

**Time to create: ~1-2 hours**
**Cost: FREE**
**Skill needed: None (drag & drop)**

