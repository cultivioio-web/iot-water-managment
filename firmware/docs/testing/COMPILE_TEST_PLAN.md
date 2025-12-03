# 🔧 Firmware Compile Test Plan

**Target:** ESP32-H2 with ESP-IDF v5.x  
**Date:** December 3, 2025

---

## 📋 Prerequisites

### 1. Install ESP-IDF

**Windows (Recommended):**
```powershell
# Download ESP-IDF Tools Installer from:
# https://dl.espressif.com/dl/esp-idf/

# OR use Git + PowerShell:
mkdir C:\Espressif
cd C:\Espressif
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
.\install.ps1 esp32h2
```

**After Installation:**
```powershell
# Open ESP-IDF PowerShell (from Start Menu)
# OR run:
C:\Espressif\esp-idf\export.ps1
```

### 2. Verify Installation
```powershell
idf.py --version
# Should show: ESP-IDF v5.x.x
```

---

## 🏗️ Compile Test Procedure

### Step 1: Sensor Node

```powershell
cd C:\test\iot-water-managment\firmware\sensor_node

# Set target chip
idf.py set-target esp32h2

# Build
idf.py build

# Expected: "Project build complete"
# Check for: 0 warnings, 0 errors
```

**Success Criteria:**
- [ ] Build completes without errors
- [ ] No compiler warnings
- [ ] Binary size < 1MB
- [ ] Output files created: `build/sensor_node.bin`

### Step 2: Controller Node

```powershell
cd C:\test\iot-water-managment\firmware\controller_node

idf.py set-target esp32h2
idf.py build
```

**Success Criteria:**
- [ ] Build completes without errors
- [ ] No compiler warnings
- [ ] Binary size < 1MB
- [ ] Output files created: `build/controller_node.bin`

### Step 3: Router Node

```powershell
cd C:\test\iot-water-managment\firmware\router_node

idf.py set-target esp32h2
idf.py build
```

**Success Criteria:**
- [ ] Build completes without errors
- [ ] No compiler warnings
- [ ] Binary size < 500KB (simpler code)
- [ ] Output files created: `build/router_node.bin`

### Step 4: Unified Node (Optional)

```powershell
cd C:\test\iot-water-managment\firmware\unified

idf.py set-target esp32h2
idf.py build
```

---

## 📊 Build Report Template

| Node | Build Status | Warnings | Errors | Binary Size | Time |
|------|--------------|----------|--------|-------------|------|
| Sensor | ⏳ | - | - | - | - |
| Controller | ⏳ | - | - | - | - |
| Router | ⏳ | - | - | - | - |
| Unified | ⏳ | - | - | - | - |

---

## ⚠️ Common Issues & Solutions

### Issue 1: "idf.py not found"
```powershell
# Run ESP-IDF export script first:
C:\Espressif\esp-idf\export.ps1
```

### Issue 2: "esp32h2 not supported"
```powershell
# Update ESP-IDF to v5.1+
cd C:\Espressif\esp-idf
git fetch
git checkout v5.1.2
git submodule update --init --recursive
.\install.ps1 esp32h2
```

### Issue 3: "Component not found"
```powershell
# Clear managed components and rebuild
rm -r managed_components
rm dependencies.lock
idf.py build
```

### Issue 4: "Permission denied"
```powershell
# Run PowerShell as Administrator
# OR change execution policy:
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

---

## 🔄 Clean Build (If Needed)

```powershell
# Full clean
idf.py fullclean

# Then rebuild
idf.py build
```

---

## 📁 Expected Build Output

```
build/
├── sensor_node.bin           # Main firmware binary
├── sensor_node.elf           # Debug symbols
├── sensor_node.map           # Memory map
├── bootloader/
│   └── bootloader.bin        # Bootloader
├── partition_table/
│   └── partition-table.bin   # Partition table
└── flash_args                # Flash arguments
```

---

## 🚀 Flash to Device (After Build)

```powershell
# Connect ESP32-H2 via USB
# Find COM port in Device Manager

idf.py -p COM3 flash monitor

# Replace COM3 with your port
```

---

## ✅ Compile Test Checklist

### Pre-Test
- [ ] ESP-IDF v5.1+ installed
- [ ] ESP-IDF environment activated
- [ ] Project cloned/updated

### Sensor Node
- [ ] `idf.py set-target esp32h2` successful
- [ ] `idf.py build` completes
- [ ] 0 warnings
- [ ] 0 errors
- [ ] Binary created

### Controller Node
- [ ] `idf.py set-target esp32h2` successful
- [ ] `idf.py build` completes
- [ ] 0 warnings
- [ ] 0 errors
- [ ] Binary created

### Router Node
- [ ] `idf.py set-target esp32h2` successful
- [ ] `idf.py build` completes
- [ ] 0 warnings
- [ ] 0 errors
- [ ] Binary created

### Post-Test
- [ ] Build report completed
- [ ] Issues documented
- [ ] Ready for unit tests

---

*Last Updated: December 3, 2025*

