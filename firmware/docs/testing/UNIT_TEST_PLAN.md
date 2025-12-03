# 🧪 Firmware Unit Test Plan

**Target:** ESP32-H2 / Native (Mock)  
**Framework:** Unity (ESP-IDF built-in) + Native Mock  
**Date:** December 3, 2025

---

## 📋 Test Strategy

### Two Testing Approaches

| Approach | Platform | Speed | Coverage | When to Use |
|----------|----------|-------|----------|-------------|
| **Native Mock** | PC (Windows/Linux) | ⚡ Fast | Logic only | Quick iteration |
| **Target Tests** | ESP32-H2 | 🐢 Slow | Full | Final validation |

---

## 🏗️ Test Categories

### 1. Water Level Calculation Tests
| Test Case | Input | Expected Output | Priority |
|-----------|-------|-----------------|----------|
| Normal reading | height=200, dist=50 | 75% (150cm) | High |
| Zero height | height=0 | Error, 0% | High |
| Negative water | dist > height | 0% | High |
| Full tank | dist=0 | 100% | Medium |
| Invalid samples | all -1 | sensor_status=1 | High |
| Partial samples | 3 of 5 valid | Use avg of 3 | Medium |

### 2. Pump Control Logic Tests
| Test Case | Condition | Expected Action | Priority |
|-----------|-----------|-----------------|----------|
| Low water trigger | level=15%, threshold=20% | Pump ON | High |
| High water stop | level=85%, threshold=80% | Pump OFF | High |
| Hysteresis | level=50% | No change | High |
| Timeout | running > timeout | Pump OFF | High |
| Manual override | manual=true | Ignore auto | High |
| Manual expire | time > end_time | Auto resume | High |
| Sensor offline | no update 30s | Pump OFF | High |

### 3. Time Calculation Tests
| Test Case | Scenario | Expected | Priority |
|-----------|----------|----------|----------|
| 49-day overflow | tick > 2^32 | No overflow | Critical |
| Manual timer | 10 min set | Correct countdown | High |
| Uptime counter | 1 hour | 3600 seconds | Medium |

### 4. BLE Provisioning Tests
| Test Case | Input | Expected | Priority |
|-----------|-------|----------|----------|
| Valid tank config | h=200, d=100 | Accept | High |
| Invalid height | h=0 | Reject | High |
| Invalid height | h=2000 | Reject | High |
| Invalid threshold | on > off | Reject | High |
| Buffer overflow | name=100 chars | Truncate | Critical |
| Valid password | len=8 | Accept | High |
| Short password | len=2 | Reject | High |

### 5. Input Validation Tests
| Test Case | Input | Expected | Priority |
|-----------|-------|----------|----------|
| Tank height min | 49 | Reject | High |
| Tank height max | 1001 | Reject | High |
| Tank height valid | 200 | Accept | High |
| Zigbee channel | 10 | Reject | High |
| Zigbee channel | 15 | Accept | High |
| Zigbee channel | 27 | Reject | High |

---

## 📁 Test File Structure

```
firmware/
├── test_esp32/              # Target tests (ESP32)
│   ├── main/
│   │   ├── test_main.c      # Unity test runner
│   │   └── unit_tests.c     # Test cases
│   └── CMakeLists.txt
│
└── test_native/             # Native tests (PC)
    ├── test_water_level.c
    ├── test_pump_logic.c
    ├── test_time_calc.c
    ├── test_ble_validation.c
    ├── mocks/
    │   ├── mock_esp_timer.h
    │   ├── mock_gpio.h
    │   └── mock_nvs.h
    └── Makefile
```

---

## 🔧 Native Test Setup (Windows)

### Option 1: MinGW-w64 (Recommended)

```powershell
# Install via winget
winget install -e --id MSYS2.MSYS2

# Open MSYS2 terminal and install GCC
pacman -S mingw-w64-x86_64-gcc make

# Add to PATH: C:\msys64\mingw64\bin
```

### Option 2: Visual Studio Build Tools

```powershell
# Install Visual Studio Build Tools
winget install Microsoft.VisualStudio.2022.BuildTools

# Open Developer Command Prompt
cl.exe /? # Verify installation
```

---

## 🧪 Running Native Tests

### Build & Run

```powershell
cd C:\test\iot-water-managment\firmware\test_native

# With MinGW
mingw32-make test

# With MSVC
nmake test

# Expected output:
# Running test_water_level... PASSED
# Running test_pump_logic... PASSED
# Running test_time_calc... PASSED
# Running test_ble_validation... PASSED
# ================================
# All tests PASSED (24/24)
```

---

## 🎯 Target Tests (ESP32-H2)

### Build & Flash

```powershell
cd C:\test\iot-water-managment\firmware\test_esp32

idf.py set-target esp32h2
idf.py build flash monitor -p COM3
```

### Expected Output

```
Running water_level_tests...
  test_normal_reading... PASSED
  test_zero_height... PASSED
  test_negative_water... PASSED
  
Running pump_control_tests...
  test_low_water_trigger... PASSED
  test_high_water_stop... PASSED
  test_manual_override... PASSED

================================
24 Tests 0 Failures 0 Ignored
OK
```

---

## 📊 Test Coverage Goals

| Module | Target Coverage | Priority |
|--------|-----------------|----------|
| Water Level Calculation | 100% | Critical |
| Pump Control Logic | 100% | Critical |
| Time Calculations | 100% | Critical |
| BLE Input Validation | 90% | High |
| NVS Operations | 80% | Medium |
| Zigbee Communication | 70% | Medium |

---

## ✅ Test Execution Checklist

### Pre-Test
- [ ] Test environment set up (MinGW or ESP-IDF)
- [ ] Mock files created
- [ ] Test cases reviewed

### Native Tests
- [ ] test_water_level.c passes
- [ ] test_pump_logic.c passes
- [ ] test_time_calc.c passes
- [ ] test_ble_validation.c passes

### Target Tests
- [ ] ESP32-H2 connected
- [ ] test_esp32 builds
- [ ] All Unity tests pass
- [ ] No memory leaks

### Post-Test
- [ ] Coverage report generated
- [ ] Failed tests documented
- [ ] Issues filed for failures

---

## 🐛 Test Failure Protocol

1. **Document failure** - Test name, input, expected, actual
2. **Reproduce** - Run test in isolation
3. **Debug** - Add logging, step through
4. **Fix** - Update code
5. **Verify** - Re-run all tests
6. **Commit** - Include test in PR

---

*Last Updated: December 3, 2025*

