# Frontend Integration Guide

## Changes Made

### 1. **Removed Embedded HTML from Arduino Code**
   - Deleted the `getSetupPageHTML()` function (166 lines of embedded HTML/CSS/JavaScript)
   - Moved to external `index.html` file served via SPIFFS filesystem

### 2. **Added SPIFFS Support**
   - Added `#include <SPIFFS.h>` to the Arduino sketch
   - Initialized SPIFFS filesystem in `setupWiFiAndWebServer()`
   - Added file existence checks and error handling

### 3. **Updated Web Server Routes**
   - Modified `setupWebInterface()` to serve external HTML file
   - Routes now use `SPIFFS.open()` and `server.streamFile()`
   - Both `/` and `/setup` routes serve the same `index.html`

### 4. **Created SPIFFS Data Folder**
   - Created `zigbee_water_management/data/` folder
   - Copied beautified `index.html` to this folder
   - Added `README.md` with upload instructions

## File Structure

```
iot-water-management/
├── frontend/
│   └── index.html              # Source HTML (beautified version)
├── zigbee_water_management/
│   ├── zigbee_water_management.ino  # Main Arduino sketch
│   ├── config.h
│   ├── zigbee_config.h
│   └── data/                   # SPIFFS filesystem folder
│       ├── index.html          # Copy of frontend HTML
│       └── README.md           # Upload instructions
```

## Benefits of This Approach

### ✅ **Advantages**
1. **Cleaner Code**: Arduino sketch is now 166 lines shorter
2. **Easier Maintenance**: HTML/CSS/JavaScript in separate file
3. **Better Development**: Edit HTML without recompiling Arduino code
4. **Version Control**: Better diff tracking for HTML changes
5. **Flexibility**: Can add more web resources (CSS, JS, images) easily
6. **Memory Efficient**: HTML stored in flash filesystem, not program memory

### ⚠️ **Requirements**
1. **SPIFFS Upload**: Must upload data folder to ESP32 before use
2. **Flash Space**: Requires ~1.5MB of flash for SPIFFS partition
3. **First-Time Setup**: Extra step for new ESP32 deployments

## How to Deploy

### Step 1: Upload Arduino Sketch
1. Open `zigbee_water_management.ino` in Arduino IDE
2. Select your ESP32 board and COM port
3. Click **Upload** to flash the firmware

### Step 2: Upload SPIFFS Data
Choose one of these methods:

#### Option A: Arduino IDE + ESP32 Sketch Data Upload Plugin
1. Install the plugin from GitHub
2. Tools → ESP32 Sketch Data Upload
3. Wait for completion

#### Option B: PlatformIO
```bash
pio run --target uploadfs
```

#### Option C: Manual with esptool
```bash
mkspiffs -c data -b 4096 -p 256 -s 0x170000 spiffs.bin
esptool.py --chip esp32 --port COM3 write_flash 0x290000 spiffs.bin
```

### Step 3: Verify
1. Reset the ESP32
2. Check Serial Monitor for:
   ```
   SPIFFS filesystem mounted successfully
   index.html found in SPIFFS
   WiFi AP started: WaterSystem-Setup
   Setup AP IP: 192.168.4.1
   ```
3. Connect to WiFi AP "WaterSystem-Setup" (password: watersystem123)
4. Open browser to http://192.168.4.1
5. You should see the beautiful new interface!

## Troubleshooting

### Problem: "index.html not found in SPIFFS"
**Solution**: Upload the SPIFFS data folder using one of the methods above

### Problem: "Failed to mount SPIFFS filesystem"
**Solution**: 
- Check partition scheme includes SPIFFS
- Tools → Partition Scheme → "Default 4MB with spiffs"
- Re-upload both sketch and SPIFFS data

### Problem: Blank page or 500 error
**Solution**:
- Verify index.html is in the `data/` folder
- Check file size (should be ~18KB)
- Re-upload SPIFFS data

### Problem: Configuration doesn't save
**Solution**:
- Check `/api/config` endpoint works
- Verify POST to `/configure` returns success
- Check Serial Monitor for NVS/Preferences errors

## API Endpoints

The ESP32 web server provides these endpoints:

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/` | GET | Serves main interface (index.html) |
| `/setup` | GET | Serves setup interface (index.html) |
| `/api/config` | GET | Returns current configuration as JSON |
| `/configure` | POST | Saves configuration and restarts system |

## Development Workflow

### Updating the Frontend
1. Edit `frontend/index.html` with your changes
2. Copy to `zigbee_water_management/data/index.html`:
   ```bash
   cp frontend/index.html zigbee_water_management/data/index.html
   ```
3. Upload SPIFFS data to ESP32 (no need to re-upload sketch)
4. Refresh browser to see changes

### Updating the Firmware
1. Edit `zigbee_water_management.ino`
2. Upload sketch (no need to re-upload SPIFFS unless HTML changed)
3. ESP32 will restart with new firmware

## Notes

- The beautified HTML includes all the modern styling and features
- SPIFFS auto-formats on first mount with `SPIFFS.begin(true)`
- File streaming is more memory-efficient than loading entire file
- Web server serves files with correct MIME types
- Configuration API remains unchanged and compatible

