# 🚀 Cultivio AquaSense Firmware v1.0.0 Release Notes

**Release Date:** December 3, 2025  
**Status:** Production Ready  
**Target Hardware:** ESP32-H2

---

## 🎉 First Production Release!

This is the first production-ready release of the Cultivio AquaSense firmware. All critical bugs have been fixed, security hardened, and the system is ready for deployment.

---

## 📊 What's New

### Security Enhancements
- ✅ **BLE Encryption** - All Bluetooth connections now use 128-bit encryption with Secure Connections
- ✅ **Input Validation** - All configuration parameters are validated with safe ranges
- ✅ **Buffer Overflow Protection** - All string operations have strict bounds checking

### Stability Improvements
- ✅ **Thread Safety** - Mutex protection on all shared configuration access
- ✅ **Watchdog Timer** - 30-second watchdog prevents system hangs
- ✅ **Graceful Error Handling** - NVS failures, sensor disconnects, network issues all handled

### Bug Fixes
- ✅ Fixed infinite loop in ultrasonic sensor polling
- ✅ Fixed pump timeout overflow (was limited to 18 hours, now unlimited)
- ✅ Fixed manual override time calculation after long uptime
- ✅ Fixed button debounce for reliable input
- ✅ Fixed Zigbee join retry limit (no more infinite retries)

---

## 📦 Files Included

```
firmware/
├── unified/
│   └── build/
│       ├── cultivio_unified.bin     ← Main firmware binary
│       ├── bootloader/bootloader.bin
│       └── partition_table/partition-table.bin
```

---

## 🔧 Installation

### Flash via ESP-IDF
```bash
cd firmware/unified
idf.py -p COMx flash monitor
```

### Flash via esptool.py
```bash
esptool.py --chip esp32h2 --port COMx \
  --baud 460800 write_flash \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 cultivio_unified.bin
```

---

## ⚠️ Upgrade Notes

### From Beta (v0.9.x)
1. Flash new firmware to all devices
2. Devices will require re-pairing (new BLE encryption)
3. All configurations are preserved in NVS

### New Installations
1. Flash firmware
2. Power on device
3. Connect via Cultivio app
4. Configure device role and settings

---

## 🔒 Security Notes

### BLE Pairing
- First connection requires pairing confirmation
- Use "Just Works" pairing (no PIN required)
- Optional password protection for multi-tenant buildings

### Configuration Limits
| Parameter | Min | Max |
|-----------|-----|-----|
| Tank Height | 50 cm | 1000 cm |
| Tank Diameter | 30 cm | 500 cm |
| Sensor Offset | 0 cm | 50 cm |
| Report Interval | 1 sec | 300 sec |
| Pump Thresholds | 0% | 100% |
| Pump Timeout | 1 min | 120 min |
| Zigbee Channel | 11 | 26 |

---

## 🧪 Testing Completed

- [x] 7-day continuous operation test
- [x] Power cycling stress test (100+ cycles)
- [x] BLE security audit
- [x] Zigbee mesh network test (multi-floor)
- [x] Input validation fuzzing
- [x] Memory leak analysis

---

## 📋 Known Issues

None. All identified issues have been resolved.

---

## 📞 Support

- **WhatsApp:** +91 9354484998
- **Email:** support@cultivio.in
- **Documentation:** [Support Page](https://cultivio.in/pages/support.html)

---

## 🙏 Acknowledgments

Thanks to everyone who contributed to making this release production-ready!

---

**Happy Water Saving! 💧**

_Cultivio Technologies_

