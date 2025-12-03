# 🔍 Firmware Code Review

**Date:** December 3, 2025  
**Reviewer:** Automated Code Analysis  
**Files Reviewed:**
- `sensor_node/main/sensor_node.c`
- `controller_node/main/controller_node.c`
- `shared/ble_provision/ble_provision.c`

---

## 📊 Review Summary

| Category | Issues Found | Fixed | Remaining |
|----------|-------------|-------|-----------|
| Edge Cases | 3 | ✅ 3 | 0 |
| Code Style | 8 | ✅ 1 (magic numbers) | 7 |
| Potential Bugs | 2 | ✅ 2 | 0 |
| Security | 0 | N/A | 0 |

**Status:** All critical bugs fixed ✅

---

## 🔴 CRITICAL: Remaining Edge Cases

### 1. **49-Day Timer Overflow (Partially Fixed)**

**Status:** 🟠 **PARTIALLY FIXED**

The manual override timer was fixed to use `esp_timer_get_time()`, but there's still a 49-day overflow risk in **sensor timeout tracking**:

```c:212:215:firmware/controller_node/main/controller_node.c
static void pump_control_logic(void)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;  // ⚠️ OVERFLOW AFTER 49 DAYS
    // ...
    bool sensor_online = (now - g_last_sensor_update) < SENSOR_TIMEOUT_MS;
```

**Problem:**
- `xTaskGetTickCount()` is 32-bit and wraps after ~49.7 days
- `g_last_sensor_update` is set using the same approach (line 348, 368)
- After 49 days, `now - g_last_sensor_update` produces incorrect results

**Fix Required:**

```c
// Option 1: Use esp_timer for all time tracking
static int64_t g_last_sensor_update = 0;

static void pump_control_logic(void)
{
    int64_t now_ms = esp_timer_get_time() / 1000;  // Microseconds to milliseconds
    bool sensor_online = (now_ms - g_last_sensor_update) < SENSOR_TIMEOUT_MS;
    // ...
}

// Update sensor timestamp
g_last_sensor_update = esp_timer_get_time() / 1000;
```

```c
// Option 2: Use tick difference (handles wrap correctly)
static TickType_t g_last_sensor_update_tick = 0;

static void pump_control_logic(void)
{
    TickType_t now_tick = xTaskGetTickCount();
    TickType_t elapsed_ticks = now_tick - g_last_sensor_update_tick;  // Handles wrap!
    uint32_t elapsed_ms = elapsed_ticks * portTICK_PERIOD_MS;
    bool sensor_online = elapsed_ms < SENSOR_TIMEOUT_MS;
}
```

**Note:** Option 2 works because unsigned integer subtraction handles wrap-around correctly in C, as long as the elapsed time is less than 2^32 ticks (~49 days).

---

### 2. **Zero Height Edge Case**

**Status:** ✅ **HANDLED** (but redundant code)

```c:187:195:firmware/sensor_node/main/sensor_node.c
    if (valid_samples > 0) {
        // FIX: BUG #5 - Validate tank height before division
        if (tank_height == 0) {
            ESP_LOGE(TAG, "FATAL: Tank height is 0! Cannot calculate water level.");
            g_sensor_status = 1;
            g_water_level_percent = 0;
            g_water_level_cm = 0;
            return;
        }
```

**Current Protection Layers:**
1. ✅ BLE validation: `height < 50 || height > 1000` rejects zero
2. ✅ Default fallback: `tank_height_cm > 0 ? tank_height_cm : 200`
3. ✅ Runtime check: `if (tank_height == 0)` (belt and suspenders)

**Observation:** The runtime zero check (layer 3) can never trigger because layer 2 ensures minimum value of 200. This is acceptable defense-in-depth, but the log message "FATAL" is misleading.

**Recommendation:** Change log level or message:

```c
// Option: Remove unreachable code (cleaner)
// The ternary on line 175 guarantees tank_height >= 200

// Or: Keep as defense-in-depth but adjust message
if (tank_height == 0) {
    ESP_LOGE(TAG, "BUG: Tank height is 0 after default assignment!");
    // ... recovery code
}
```

---

### 3. **Pump Runtime Tracking Overflow**

**Status:** 🟡 **POTENTIAL ISSUE**

```c:154:155:firmware/controller_node/main/controller_node.c
        g_pump_running = true;
        g_pump_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
```

```c:283:288:firmware/controller_node/main/controller_node.c
    // Check pump timeout
    if (g_pump_running) {
        uint32_t runtime = now_sec - g_pump_start_time;
        if (runtime >= pump_timeout_sec) {
```

**Problem:** `g_pump_start_time` uses tick-based time, but `now_sec` uses `esp_timer`. This mixes time bases!

**Fix:**

```c
// Line 154: Use esp_timer consistently
g_pump_start_time = esp_timer_get_time() / 1000000;  // Seconds since boot

// Line 177 (pump_off): Also use esp_timer
uint32_t runtime = (esp_timer_get_time() / 1000000) - g_pump_start_time;
```

---

## 🟡 Code Style Issues

### 1. **Magic Numbers**

Several magic numbers should be named constants:

```c
// sensor_node.c, controller_node.c
vTaskDelay(pdMS_TO_TICKS(50));   // What is 50? Debounce delay?
vTaskDelay(pdMS_TO_TICKS(100));  // Button check interval?
vTaskDelay(pdMS_TO_TICKS(200));  // LED blink delay?
count < 30                       // 3 seconds? Why 30?

// Recommendations:
#define DEBOUNCE_DELAY_MS           50
#define BUTTON_CHECK_INTERVAL_MS    100
#define LED_BLINK_DELAY_MS          200
#define PROVISIONING_HOLD_TIME_MS   3000
#define PROVISIONING_CHECK_COUNT    (PROVISIONING_HOLD_TIME_MS / BUTTON_CHECK_INTERVAL_MS)
```

```c
// sensor_node.c line 180
if (distance > 0 && distance < tank_height + 50) {  // Why +50?

// Recommendation:
#define SENSOR_TOLERANCE_CM  50  // Allow readings slightly beyond tank height
```

---

### 2. **Duplicate Code**

The following functions are duplicated between sensor_node.c and controller_node.c:

| Function | Lines in sensor_node.c | Lines in controller_node.c |
|----------|------------------------|---------------------------|
| `led_init()` | 71-83 | 90-102 |
| `led_blink()` | 97-105 | 121-129 |
| `button_init()` | 85-95 | 104-114 |
| `check_provisioning_button()` | 485-512 | 606-632 |
| NVS error handling | 529-557 | 649-677 |

**Recommendation:** Move to shared library:

```c
// shared/hardware_common.h
void hw_led_init(gpio_num_t status_pin, gpio_num_t aux_pin);
void hw_led_blink(gpio_num_t pin, int times, int delay_ms);
void hw_button_init(gpio_num_t pin);
bool hw_check_provisioning_button(gpio_num_t btn_pin, gpio_num_t led_pin);
esp_err_t hw_nvs_init_with_recovery(void);
```

---

### 3. **Inconsistent Variable Naming**

```c
// sensor_node.c
static device_config_t g_config;           // Short name

// ble_provision.c  
static device_config_t g_device_config;    // Long name

// Recommendation: Use consistent naming
static device_config_t g_device_config;    // All files
```

---

### 4. **Missing Function Documentation**

No Doxygen-style documentation:

```c
// Current:
static float measure_distance_cm(void)
{
    // ...
}

// Recommended:
/**
 * @brief Measure distance using ultrasonic sensor
 * 
 * Sends trigger pulse and measures echo time to calculate distance.
 * Uses averaging and timeout handling for reliability.
 * 
 * @return Distance in centimeters, or -1.0f on timeout/error
 * 
 * @note Non-blocking but uses busy-wait during measurement (~30ms max)
 * @warning Must be called from a FreeRTOS task context
 */
static float measure_distance_cm(void)
{
    // ...
}
```

---

### 5. **Inconsistent Error Handling**

Three different patterns used:

```c
// Pattern 1: Crash on error (too aggressive for production)
ESP_ERROR_CHECK(esp_zb_start(false));

// Pattern 2: Log and continue (may hide issues)
esp_err_t ret = esp_zb_set_channel_mask(...);
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed: %s", esp_err_to_name(ret));
}

// Pattern 3: Log and recover (best for production)
ret = nvs_flash_init();
if (ret != ESP_OK) {
    // Recovery logic...
}
```

**Recommendation:** Define error handling policy:

```c
// shared/error_policy.h

// For initialization failures that prevent operation
#define HANDLE_FATAL_ERROR(ret, msg) do { \
    if (ret != ESP_OK) { \
        ESP_LOGE(TAG, "FATAL: %s: %s", msg, esp_err_to_name(ret)); \
        hw_show_error_pattern(); \
        esp_restart(); \
    } \
} while(0)

// For recoverable errors
#define HANDLE_RECOVERABLE_ERROR(ret, msg) do { \
    if (ret != ESP_OK) { \
        ESP_LOGW(TAG, "%s: %s (continuing)", msg, esp_err_to_name(ret)); \
    } \
} while(0)
```

---

### 6. **Long Functions**

Some functions exceed recommended length:

| Function | File | Lines | Recommendation |
|----------|------|-------|----------------|
| `pump_control_logic()` | controller_node.c | 88 | Split into sub-functions |
| `parse_config_data()` | ble_provision.c | 230 | Use dispatch table |
| `app_main()` | both | ~100 | Extract init sequences |

**Example refactor for pump_control_logic():**

```c
static bool check_manual_override_expired(uint32_t now_sec) { /* ... */ }
static void handle_manual_mode(uint32_t now_sec) { /* ... */ }
static void handle_sensor_offline(void) { /* ... */ }
static void handle_automatic_mode(uint32_t now_sec) { /* ... */ }

static void pump_control_logic(void) {
    uint32_t now_sec = esp_timer_get_time() / 1000000;
    
    if (g_manual_override) {
        handle_manual_mode(now_sec);
        return;
    }
    
    if (!is_sensor_online()) {
        handle_sensor_offline();
        return;
    }
    
    handle_automatic_mode(now_sec);
}
```

---

### 7. **Unused Variables**

```c
// controller_node.c line 60
static uint8_t  g_sensor_status = 0xFF;  // FIX: BUG #13 - Removed unused attribute
```

While marked as "will be updated", it's currently only set but never read meaningfully. Consider removing or properly integrating.

---

### 8. **Header File Best Practices**

`ble_provision.h` should have:
- Include guards (has `#pragma once` - OK)
- Forward declarations for opaque types
- Const correctness for pointer parameters

---

## ✅ Good Practices Observed

1. **Mutex Protection**: Config access is properly mutex-protected (FIX #1)
2. **Input Validation**: BLE inputs are validated with clear range checks
3. **Watchdog Integration**: Tasks properly feed the watchdog
4. **Buffer Safety**: snprintf() used with sizeof() for buffer safety
5. **NVS Recovery**: Multi-stage NVS recovery with factory reset fallback
6. **Default Values**: Sensible defaults prevent null/zero issues
7. **Logging**: Good use of ESP_LOG levels for different severity

---

## 📋 Action Items

### High Priority (Fix Before Production)

| # | Issue | File | Status |
|---|-------|------|--------|
| 1 | Sensor timeout uses tick time (49-day overflow) | controller_node.c | ✅ **FIXED** - Now uses `esp_timer` with `int64_t` |
| 2 | Mixed time bases in pump runtime | controller_node.c | ✅ **FIXED** - Consistent `esp_timer` usage |

### Medium Priority (Fix in Next Sprint)

| # | Issue | Action | Status |
|---|-------|--------|--------|
| 3 | Magic numbers | Create constants header | ✅ **FIXED** - Added named constants |
| 4 | Duplicate code | Extract to shared library | ⏳ Pending |
| 5 | Long functions | Refactor pump_control_logic() | ⏳ Pending |

### Low Priority (Technical Debt)

| # | Issue | Action | Status |
|---|-------|--------|--------|
| 6 | Missing documentation | Add Doxygen comments | ⏳ Pending |
| 7 | Inconsistent naming | Standardize variable names | ⏳ Pending |
| 8 | Inconsistent error handling | Create error policy macros | ⏳ Pending |

---

## 🧪 Recommended Tests

### Edge Case Tests

```c
// Test 1: Zero tank height (should use default 200)
device_config_t cfg = {0};
cfg.tank_height_cm = 0;
// Expected: Uses 200cm default

// Test 2: 49-day uptime simulation
// Set xTaskGetTickCount() to near-overflow value
// Verify sensor timeout still works correctly

// Test 3: Manual override at tick overflow boundary
// Start manual override just before tick wrap
// Verify it expires correctly after wrap

// Test 4: Pump timeout with uint8_t overflow
cfg.pump_timeout_minutes = 255;  // Max uint8_t
// Verify: 255 * 60 = 15300 seconds (no overflow in uint32_t)
```

---

## 📝 Sign-Off

- [ ] High priority issues addressed
- [ ] Unit tests added for edge cases
- [ ] Code review feedback incorporated
- [ ] Integration testing passed

---

*Last Updated: December 3, 2025*

