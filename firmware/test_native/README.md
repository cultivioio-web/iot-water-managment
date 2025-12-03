# 🧪 Native Unit Tests

Run firmware unit tests on your PC without ESP32 hardware.

---

## Quick Start

### Windows (with MinGW)

```powershell
cd firmware/test_native
.\run_tests.ps1
```

### Windows (batch)

```cmd
cd firmware\test_native
run_tests.bat
```

### Manual Compilation

```powershell
gcc -o test_all.exe test_all.c -I./mocks -Wall -Wextra
.\test_all.exe
```

---

## Prerequisites

### Install MinGW-w64 (GCC for Windows)

```powershell
# Option 1: MSYS2 (Recommended)
winget install -e --id MSYS2.MSYS2
# Then in MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc

# Add to PATH: C:\msys64\mingw64\bin

# Option 2: Standalone MinGW
# Download from: https://www.mingw-w64.org/downloads/
```

### Verify Installation

```powershell
gcc --version
# Should show: gcc (x86_64-...) 12.x or higher
```

---

## Test Structure

```
test_native/
├── README.md           # This file
├── run_tests.ps1       # PowerShell runner
├── run_tests.bat       # Batch runner
├── test_all.c          # All test cases
└── mocks/
    └── mock_esp.h      # ESP-IDF mock functions
```

---

## Test Categories

### 1. Water Level Calculation (7 tests)
- Normal reading
- Zero height (division by zero)
- Full tank
- Empty tank
- Invalid samples
- Partial samples
- Sensor offset

### 2. Pump Control Logic (7 tests)
- Low water trigger
- High water stop
- Hysteresis (no change at 50%)
- Timeout protection
- Manual override active
- Manual override expired
- Sensor offline safety

### 3. Time Calculations (2 tests)
- 49-day overflow prevention
- Manual override at day 50+

### 4. Input Validation (5 tests)
- Tank height validation
- Tank diameter validation
- Pump thresholds validation
- Zigbee channel validation
- Report interval validation

**Total: 21 test cases**

---

## Expected Output

```
========================================
Cultivio AquaSense - Unit Tests
========================================

Water Level Calculation Tests:
  test_water_level_normal_reading... PASSED
  test_water_level_zero_height... PASSED
  test_water_level_full_tank... PASSED
  test_water_level_empty_tank... PASSED
  test_water_level_invalid_samples... PASSED
  test_water_level_partial_samples... PASSED
  test_water_level_with_offset... PASSED

Pump Control Logic Tests:
  test_pump_low_water_trigger... PASSED
  test_pump_high_water_stop... PASSED
  test_pump_hysteresis... PASSED
  test_pump_timeout... PASSED
  test_pump_manual_override... PASSED
  test_pump_manual_override_expire... PASSED
  test_pump_sensor_offline... PASSED

Time Calculation Tests (49-day overflow):
  test_time_49_day_overflow... PASSED
  test_time_manual_override_50_days... PASSED

Input Validation Tests:
  test_validate_tank_height... PASSED
  test_validate_tank_diameter... PASSED
  test_validate_pump_thresholds... PASSED
  test_validate_zigbee_channel... PASSED
  test_validate_report_interval... PASSED

================================
Tests: 42 passed, 0 failed
All tests PASSED!
================================
```

---

## Adding New Tests

1. Add test function in `test_all.c`:
```c
void test_my_new_feature(void) {
    // Setup
    // Action
    TEST_ASSERT_EQUAL(expected, actual);
}
```

2. Add to test runner in `main()`:
```c
RUN_TEST(test_my_new_feature);
```

3. Run tests to verify.

---

## Mock Functions

The `mocks/mock_esp.h` provides:

| Function | Mock Behavior |
|----------|--------------|
| `esp_timer_get_time()` | Returns controllable mock time |
| `gpio_set_level()` | Stores in mock array |
| `gpio_get_level()` | Returns from mock array |
| `vTaskDelay()` | No-op |
| `ESP_LOGI/LOGW/LOGE()` | Prints to stdout |
| `xSemaphoreCreateMutex()` | Returns non-null |
| `xSemaphoreTake/Give()` | Always succeeds |

---

*Last Updated: December 3, 2025*

