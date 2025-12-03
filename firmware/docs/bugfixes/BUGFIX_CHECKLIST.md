# 🐛 Firmware Bug Fix Checklist

**Priority:** Complete in order (Critical → High → Medium → Low)

---

## 🔴 **CRITICAL (Fix Immediately)**

### [ ] BUG #1: Add Mutex Protection to Config Access
**File:** `shared/ble_provision/ble_provision.c`
- [ ] Create `g_config_mutex` semaphore
- [ ] Wrap all `g_device_config` access with mutex
- [ ] Protect BLE callbacks
- [ ] Protect Zigbee attribute registration

**Code:**
```c
static SemaphoreHandle_t g_config_mutex;
g_config_mutex = xSemaphoreCreateMutex();
// Wrap critical sections
xSemaphoreTake(g_config_mutex, portMAX_DELAY);
// Access config
xSemaphoreGive(g_config_mutex);
```

---

### [ ] BUG #2: Fix Ultrasonic Sensor Infinite Loop
**Files:** `sensor_node/main/sensor_node.c`, `unified/main/unified_main.c`
- [ ] Add `esp_rom_delay_us(10)` in while loops
- [ ] Add `taskYIELD()` to prevent watchdog
- [ ] Test with disconnected sensor

**Lines to fix:** 139-150, 161-173

---

### [ ] BUG #3: Fix Pump Timeout Overflow
**File:** `controller_node/main/controller_node.c`
- [ ] Change to `uint32_t` for timeout calculation
- [ ] Add cast: `(uint32_t)g_config.pump_timeout_minutes * 60`
- [ ] Add maximum limit (7200 seconds = 2 hours)
- [ ] Add validation log

**Line:** 216

---

### [ ] BUG #6: Fix Buffer Overflow in Device Name
**File:** `shared/ble_provision/ble_provision.c`
- [ ] Add strict length validation (>= not just >)
- [ ] Force null termination
- [ ] Use larger temp buffer
- [ ] Validate final name length

**Lines:** 221-232

---

### [ ] BUG #9: Fix Zigbee Attribute Race Condition
**Files:** `sensor_node/main/sensor_node.c`, `controller_node/main/controller_node.c`
- [ ] Use `esp_zb_lock_acquire()` before attribute updates
- [ ] Use `esp_zb_zcl_set_attribute_val()` instead of direct pointer
- [ ] Release lock after update
- [ ] Test concurrent access

**Lines:** 215-225, 236-240

---

### [ ] SEC #1: Remove Hardcoded Default Password
**File:** `shared/ble_provision/ble_provision.h`
- [ ] Generate unique password from MAC address
- [ ] Force password change on first setup
- [ ] Add password strength validation
- [ ] Update documentation

**Line:** 36

---

### [ ] SEC #2: Add BLE Encryption
**File:** `shared/ble_provision/ble_provision.c`
- [ ] Enable BLE pairing
- [ ] Configure secure connections
- [ ] Set IO capabilities
- [ ] Test pairing flow

**Add to ble_provision_start():**
```c
esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
```

---

## 🟠 **HIGH PRIORITY (Fix This Week)**

### [ ] BUG #4: Add Return Value Checks
**Files:** All main files
- [ ] Check all `gpio_set_level()` returns
- [ ] Check all `gpio_config()` returns
- [ ] Check `esp_zb_set_channel_mask()` return
- [ ] Add error logging

---

### [ ] BUG #7: Improve NVS Error Handling
**Files:** `sensor_node/main/sensor_node.c`, `controller_node/main/controller_node.c`
- [ ] Check `nvs_flash_erase()` return
- [ ] Check second `nvs_flash_init()` return
- [ ] Add fallback to safe mode on NVS failure
- [ ] Consider factory reset option

**Lines:** 456-461, 592-597

---

### [ ] BUG #8: Fix Manual Override Time Calculation
**File:** `controller_node/main/controller_node.c`
- [ ] Use `esp_timer_get_time()` instead of ticks
- [ ] Handle 64-bit time properly
- [ ] OR use relative tick counts
- [ ] Test with long runtime

**Lines:** 184-185, 209-210, 468

---

### [ ] BUG #12: Add Zigbee Join Retry Limit
**Files:** `sensor_node/main/sensor_node.c`, `controller_node/main/controller_node.c`
- [ ] Add `g_join_retry_count` counter
- [ ] Set `MAX_JOIN_RETRIES` to 10
- [ ] Enter sleep/error mode after max retries
- [ ] Reset counter on success

**Lines:** 293-296, 396-399

---

### [ ] SEC #3: Add Input Validation
**File:** `shared/ble_provision/ble_provision.c`
- [ ] Validate tank height (50-1000 cm)
- [ ] Validate pump thresholds (0-100%)
- [ ] Check ON < OFF threshold logic
- [ ] Validate timeout values
- [ ] Add range constants

**Lines:** 153-286

---

## 🟡 **MEDIUM PRIORITY (Fix Next Week)**

### [ ] BUG #5: Add Tank Height Validation
**File:** `sensor_node/main/sensor_node.c`
- [ ] Check `tank_height != 0` before division
- [ ] Log error and set status on invalid height
- [ ] Return early to prevent crash

**Line:** 184

---

### [ ] BUG #10: Configure Watchdog Timer
**Files:** All main files
- [ ] Include `esp_task_wdt.h`
- [ ] Init with 30-second timeout
- [ ] Add task to watchdog
- [ ] Feed watchdog in long operations

**Add to app_main():**
```c
esp_task_wdt_init(30, true);
esp_task_wdt_add(NULL);
// In loops:
esp_task_wdt_reset();
```

---

### [ ] BUG #11: Add Button Debounce
**Files:** `sensor_node/main/sensor_node.c`, `controller_node/main/controller_node.c`
- [ ] Create `is_button_pressed()` function
- [ ] Add 50ms debounce delay
- [ ] Re-check pin after delay
- [ ] Test with mechanical button

**Lines:** 422-438, 556-574

---

### [ ] BUG #13: Fix Unused Variable
**File:** `controller_node/main/controller_node.c`
- [ ] Remove `__attribute__((unused))`
- [ ] Update `g_sensor_status` in Zigbee callback
- [ ] Add to report attribute handler
- [ ] Test status reporting

**Line:** 59

---

### [ ] Q1: Replace Magic Numbers
**Files:** All
- [ ] Define all delay constants
- [ ] Define all threshold constants
- [ ] Define all timeout constants
- [ ] Add comments explaining values

---

### [ ] Q4: Reduce Global Variables
**Files:** All
- [ ] Create state structures
- [ ] Encapsulate related variables
- [ ] Pass pointers instead of globals
- [ ] Improve thread safety

---

## 🟢 **LOW PRIORITY (Improve Later)**

### [ ] Q3: Add Function Documentation
**Files:** All
- [ ] Add Doxygen comments to all public functions
- [ ] Document parameters and return values
- [ ] Add usage examples
- [ ] Generate HTML docs

---

### [ ] Q5: Refactor Long Functions
**Files:** All
- [ ] Break `app_main()` into smaller functions
- [ ] Split `pump_control_logic()` 
- [ ] Extract LED control functions
- [ ] Improve testability

---

## 📊 **TESTING CHECKLIST**

After each fix, verify:

### [ ] Unit Tests
- [ ] Water level calculation tests
- [ ] Pump control logic tests
- [ ] BLE command parsing tests
- [ ] Time calculation tests

### [ ] Integration Tests
- [ ] Sensor → Controller communication
- [ ] BLE provisioning flow
- [ ] Manual pump override
- [ ] Zigbee network join

### [ ] Stress Tests
- [ ] Run for 7+ days
- [ ] Rapid sensor disconnects
- [ ] BLE fuzzing
- [ ] Power failure recovery
- [ ] Flash 49+ days uptime

### [ ] Security Tests
- [ ] BLE pairing
- [ ] Unauthorized access attempts
- [ ] Buffer overflow attempts
- [ ] Configuration tampering

---

## 📈 **PROGRESS TRACKING**

| Phase | Tasks | Completed | Status |
|-------|-------|-----------|--------|
| Critical | 7 | 0 | ⏳ Not Started |
| High | 5 | 0 | ⏳ Not Started |
| Medium | 5 | 0 | ⏳ Not Started |
| Low | 2 | 0 | ⏳ Not Started |
| **TOTAL** | **19** | **0** | **0%** |

---

## 🚀 **DEPLOYMENT CHECKLIST**

Before production release:

- [ ] All Critical bugs fixed
- [ ] All High priority bugs fixed
- [ ] Security audit passed
- [ ] 7-day stress test passed
- [ ] Code review completed
- [ ] Documentation updated
- [ ] OTA update tested
- [ ] Factory reset tested
- [ ] Power failure recovery tested
- [ ] BLE pairing tested on iOS and Android
- [ ] Zigbee range tested
- [ ] Manual pump override tested
- [ ] LED feedback verified
- [ ] User manual updated

---

**Last Updated:** December 3, 2025  
**Next Review:** After Critical fixes

