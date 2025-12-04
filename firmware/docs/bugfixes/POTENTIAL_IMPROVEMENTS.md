# 🔍 Potential Firmware Improvements

Found during unit testing on December 3, 2025.

---

## Issue #1: Zero Height Check Ordering

**Location:** `sensor_node.c` → `measure_water_level()`

**Current Code:**
```c
uint16_t tank_height = g_config.tank_height_cm > 0 ? g_config.tank_height_cm : 200;
// ... sampling ...
if (tank_height == 0) {  // This check never triggers because of line above!
    ESP_LOGE(TAG, "FATAL: Tank height is 0!");
```

**Issue:**
The zero check happens AFTER the default is applied. If `g_config.tank_height_cm` is 0, `tank_height` becomes 200, and the zero check never triggers.

**Impact:** Low - The default value (200cm) is used, which is safe behavior.

**Recommendation:**
```c
// Option 1: Check config value directly
if (g_config.tank_height_cm == 0) {
    ESP_LOGE(TAG, "FATAL: Tank height not configured!");
    // ...
}
uint16_t tank_height = g_config.tank_height_cm; // No default, error handled above

// Option 2: Check at provisioning time (better - already done in ble_provision.c)
// Input validation rejects height < 50, so 0 is never saved
```

**Status:** ⏳ Enhancement (current behavior is safe)

---

## Notes

All critical bugs have been fixed. This is a minor enhancement for code clarity.

The input validation in `ble_provision.c` already rejects tank heights less than 50cm, so a value of 0 should never be saved to NVS in practice.

---

*Found: December 3, 2025*

