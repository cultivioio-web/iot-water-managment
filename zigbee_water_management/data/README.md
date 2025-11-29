# SPIFFS Data Folder

This folder contains files that need to be uploaded to the ESP32's SPIFFS filesystem.

## Files
- `index.html` - Web interface for system configuration

## How to Upload to ESP32

### Method 1: Using Arduino IDE (with ESP32 Sketch Data Upload Tool)

1. Install the ESP32 Sketch Data Upload plugin:
   - Download from: https://github.com/me-no-dev/arduino-esp32fs-plugin
   - Extract to `<Arduino>/tools/ESP32FS/tool/esp32fs.jar`
   
2. Close Arduino IDE and reopen it

3. Open the `zigbee_water_management.ino` sketch

4. Click **Tools → ESP32 Sketch Data Upload**

5. Wait for the upload to complete

### Method 2: Using PlatformIO

1. Add to `platformio.ini`:
```ini
board_build.filesystem = spiffs
```

2. Upload filesystem:
```bash
pio run --target uploadfs
```

### Method 3: Using esptool.py (Manual)

1. Install esptool:
```bash
pip install esptool
```

2. Create SPIFFS image:
```bash
mkspiffs -c data -b 4096 -p 256 -s 0x170000 spiffs.bin
```

3. Upload to ESP32:
```bash
esptool.py --chip esp32 --port COM3 --baud 921600 write_flash 0x290000 spiffs.bin
```
*(Adjust COM port and partition address as needed)*

## Verification

After uploading, the ESP32 will check for `/index.html` on boot. You should see:
```
SPIFFS filesystem mounted successfully
index.html found in SPIFFS
```

If you see a warning, re-upload the filesystem data.

