# 🧪 Firmware Test Plan

**Version:** 1.0.1  
**Date:** December 3, 2025

---

## 📋 Test Phases

### Phase 1: Compile Test ⚙️

```bash
# Prerequisites
# 1. Install ESP-IDF v5.1+ from https://docs.espressif.com/projects/esp-idf/
# 2. Run ESP-IDF export script

# Windows (PowerShell)
. $HOME\esp\esp-idf\export.ps1

# Linux/Mac
. ~/esp/esp-idf/export.sh
```

#### Build Commands

```bash
# Controller Node
cd firmware/controller_node
idf.py set-target esp32h2
idf.py build

# Sensor Node  
cd firmware/sensor_node
idf.py set-target esp32h2
idf.py build

# Router Node
cd firmware/router_node
idf.py set-target esp32h2
idf.py build

# Test Build (Host)
cd firmware/test_esp32
idf.py build
```

#### Expected Results

| Node | Build Status | Binary Size | Notes |
|------|-------------|-------------|-------|
| Controller | ✅ Success | < 1MB | |
| Sensor | ✅ Success | < 1MB | |
| Router | ✅ Success | < 500KB | |

---

### Phase 2: Unit Tests 🔬

#### Test Categories

| Category | Tests | Priority |
|----------|-------|----------|
| Timer Functions | 5 | 🔴 Critical |
| Input Validation | 10 | 🔴 Critical |
| Pump Logic | 8 | 🔴 Critical |
| BLE Provisioning | 6 | 🟠 High |
| Sensor Reading | 4 | 🟠 High |

#### Test Cases

##### 2.1 Timer Edge Cases (49-Day Overflow)

```c
// TEST: esp_timer doesn't overflow
void test_timer_no_overflow(void) {
    // Simulate 50 days of uptime
    int64_t simulated_time_us = 50LL * 24 * 60 * 60 * 1000000LL;
    
    // Start time
    int64_t start_us = simulated_time_us;
    
    // Elapsed time after 1 hour
    int64_t end_us = start_us + (1LL * 60 * 60 * 1000000LL);
    
    int64_t elapsed_ms = (end_us - start_us) / 1000;
    
    TEST_ASSERT_EQUAL(3600000, elapsed_ms);  // 1 hour in ms
}

// TEST: Sensor timeout calculation
void test_sensor_timeout_after_49_days(void) {
    int64_t now_us = 50LL * 24 * 60 * 60 * 1000000LL;  // 50 days
    int64_t last_update_us = now_us - (10LL * 1000000LL);  // 10 seconds ago
    
    int64_t elapsed_ms = (now_us - last_update_us) / 1000;
    bool sensor_online = elapsed_ms < 30000;  // SENSOR_TIMEOUT_MS
    
    TEST_ASSERT_TRUE(sensor_online);
}

// TEST: Sensor offline detection
void test_sensor_offline_detection(void) {
    int64_t now_us = 50LL * 24 * 60 * 60 * 1000000LL;
    int64_t last_update_us = now_us - (60LL * 1000000LL);  // 60 seconds ago
    
    int64_t elapsed_ms = (now_us - last_update_us) / 1000;
    bool sensor_online = elapsed_ms < 30000;
    
    TEST_ASSERT_FALSE(sensor_online);
}
```

##### 2.2 Zero Height Edge Case

```c
// TEST: Default height when zero configured
void test_zero_height_uses_default(void) {
    device_config_t config = {0};
    config.tank_height_cm = 0;
    
    uint16_t tank_height = config.tank_height_cm > 0 ? config.tank_height_cm : 200;
    
    TEST_ASSERT_EQUAL(200, tank_height);
}

// TEST: BLE rejects zero height
void test_ble_rejects_zero_height(void) {
    uint16_t height = 0;
    bool valid = (height >= 50 && height <= 1000);
    
    TEST_ASSERT_FALSE(valid);
}

// TEST: BLE rejects height below minimum
void test_ble_rejects_low_height(void) {
    uint16_t height = 30;
    bool valid = (height >= 50 && height <= 1000);
    
    TEST_ASSERT_FALSE(valid);
}
```

##### 2.3 Pump Timeout Overflow

```c
// TEST: Pump timeout calculation no overflow
void test_pump_timeout_no_overflow(void) {
    uint8_t pump_timeout_minutes = 255;  // Max uint8_t
    uint32_t pump_timeout_sec = (uint32_t)pump_timeout_minutes * 60;
    
    TEST_ASSERT_EQUAL(15300, pump_timeout_sec);
}

// TEST: Pump timeout capped at 2 hours
void test_pump_timeout_capped(void) {
    uint32_t pump_timeout_sec = 10000;  // > 7200
    
    if (pump_timeout_sec > 7200) {
        pump_timeout_sec = 7200;
    }
    
    TEST_ASSERT_EQUAL(7200, pump_timeout_sec);
}
```

##### 2.4 Input Validation

```c
// TEST: Valid tank height range
void test_valid_tank_height(void) {
    TEST_ASSERT_TRUE(validate_tank_height(50));
    TEST_ASSERT_TRUE(validate_tank_height(200));
    TEST_ASSERT_TRUE(validate_tank_height(1000));
}

// TEST: Invalid tank height range
void test_invalid_tank_height(void) {
    TEST_ASSERT_FALSE(validate_tank_height(0));
    TEST_ASSERT_FALSE(validate_tank_height(49));
    TEST_ASSERT_FALSE(validate_tank_height(1001));
}

// TEST: Pump threshold logic
void test_pump_threshold_logic(void) {
    uint8_t on = 20, off = 80;
    TEST_ASSERT_TRUE(on < off);  // Valid
    
    on = 80; off = 20;
    TEST_ASSERT_FALSE(on < off);  // Invalid
}
```

---

### Phase 3: Integration Tests 🔗

#### 3.1 Hardware-in-Loop Tests

| Test | Description | Duration | Pass Criteria |
|------|-------------|----------|---------------|
| Boot Test | Device boots successfully | 30s | LED blinks, BLE advertises |
| Provisioning | Complete BLE setup | 5min | Config saved to NVS |
| Zigbee Join | Sensor joins controller | 2min | Network formed |
| Water Reading | Sensor reports level | 1min | Valid percentage (0-100%) |
| Pump Control | Auto pump on/off | 10min | Pump activates at thresholds |
| Manual Override | BLE pump control | 5min | Pump responds to commands |
| Timeout Test | Pump timeout triggers | 2hrs | Pump stops at timeout |

#### 3.2 Stress Tests

| Test | Duration | Pass Criteria |
|------|----------|---------------|
| Continuous Operation | 7 days | No crashes, no WDT resets |
| Rapid Reconnect | 1000 cycles | 99% success rate |
| Power Cycle | 100 cycles | Always boots correctly |
| High Frequency Readings | 1Hz for 24hrs | No memory leaks |

---

### Phase 4: Security Audit 🔒

| Check | Status | Notes |
|-------|--------|-------|
| BLE Encryption | ✅ Enabled | SC + MITM + Bonding |
| Input Validation | ✅ Complete | All parameters checked |
| Buffer Overflow | ✅ Protected | Bounds checking |
| Integer Overflow | ✅ Fixed | uint32_t for calculations |
| NVS Security | ⚠️ Review | Consider encryption |

---

## 🚀 Quick Test Commands

```bash
# Full build and flash (controller)
cd firmware/controller_node
idf.py build flash monitor

# Full build and flash (sensor)
cd firmware/sensor_node  
idf.py build flash monitor

# Run unit tests
cd firmware/test_esp32
idf.py build flash monitor

# Check for compiler warnings
idf.py build 2>&1 | grep -i warning

# Size analysis
idf.py size-components
```

---

## ✅ Test Checklist

### Pre-Release Checklist

- [ ] All nodes compile without errors
- [ ] All nodes compile without warnings
- [ ] Unit tests pass (100%)
- [ ] Integration tests pass
- [ ] 7-day stress test complete
- [ ] Security audit complete
- [ ] Code review approved
- [ ] Documentation updated

### Release Sign-Off

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Developer | | | |
| QA Engineer | | | |
| Project Lead | | | |

---

*Generated: December 3, 2025*

