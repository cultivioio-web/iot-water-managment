# Water Node Provisioning App

A beautiful web-based provisioning application for configuring your IoT water management devices using Web Bluetooth.

## Features

- 🔵 **BLE Connection** - Connect to your ESP32-H2 devices via Bluetooth
- ⚙️ **Tank Configuration** - Set tank dimensions and sensor offset
- 🔧 **Pump Control** - Configure ON/OFF thresholds and timeout
- 📡 **Zigbee Settings** - Set PAN ID and channel
- 💾 **Persistent Storage** - Configuration saved to device NVS
- 🎨 **Beautiful UI** - Modern dark theme with smooth animations

## Usage

### Method 1: Local File
Simply open `index.html` in Chrome or Edge browser.

### Method 2: Web Server
```bash
# Python 3
python -m http.server 8080

# Then open http://localhost:8080
```

### Method 3: Host Online
Upload to any static hosting (GitHub Pages, Netlify, etc.)

## Device Provisioning Flow

1. **Power on your device** - The ESP32-H2 will enter provisioning mode automatically on first boot (or hold button for 3 seconds to force)

2. **Open the app** - Use Chrome or Edge browser (Web Bluetooth required)

3. **Click "Scan for Devices"** - Select your WaterNode-XXXX device

4. **Configure settings**:
   - **Sensor Node**: Tank height, diameter, sensor offset, report interval
   - **Controller Node**: Pump ON/OFF thresholds, timeout

5. **Click "Save & Activate"** - Device will save config and restart

## Browser Compatibility

| Browser | Support |
|---------|---------|
| Chrome (Desktop) | ✅ Full |
| Edge (Desktop) | ✅ Full |
| Chrome (Android) | ✅ Full |
| Opera | ✅ Full |
| Firefox | ❌ No Web Bluetooth |
| Safari | ❌ No Web Bluetooth |

## BLE Service Details

| UUID | Description |
|------|-------------|
| 0x00FF | Main Service |
| 0xFF01 | Configuration Characteristic |
| 0xFF02 | Status Characteristic |
| 0xFF03 | Command Characteristic |

## Troubleshooting

### Device not appearing
- Ensure device is in provisioning mode (LED blinking rapidly)
- Check if Bluetooth is enabled on your computer/phone
- Try refreshing the page

### Connection failed
- Move closer to the device
- Ensure no other device is connected
- Power cycle the device

### Configuration not saving
- Check browser console for errors
- Ensure you clicked "Save & Activate"
- Verify device is still connected

## Screenshots

The app features a modern dark theme with:
- Gradient accents
- Smooth animations
- Responsive design
- Touch-friendly controls

## License

MIT License - Part of the IoT Water Management System

