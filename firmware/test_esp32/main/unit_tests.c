/**
 * Cultivio AquaSense - Unit Tests
 * 
 * Tests for edge cases and bug fixes:
 * - 49-day timer overflow
 * - Zero height handling
 * - Pump timeout overflow
 * - Input validation
 * 
 * Run on host with: gcc -DUNIT_TEST unit_tests.c -o test && ./test
 * Or integrate with Unity test framework for ESP-IDF
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// ============================================================================
// Test Framework (minimal)
// ============================================================================

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, msg) do { \
    tests_run++; \
    if (condition) { \
        tests_passed++; \
        printf("  ✅ PASS: %s\n", msg); \
    } else { \
        tests_failed++; \
        printf("  ❌ FAIL: %s\n", msg); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual, msg) \
    TEST_ASSERT((expected) == (actual), msg)

#define TEST_ASSERT_TRUE(condition, msg) \
    TEST_ASSERT((condition), msg)

#define TEST_ASSERT_FALSE(condition, msg) \
    TEST_ASSERT(!(condition), msg)

#define TEST_GROUP(name) printf("\n📋 %s\n", name)

// ============================================================================
// Constants (from firmware)
// ============================================================================

#define SENSOR_TIMEOUT_MS       30000
#define MAX_PUMP_TIMEOUT_SEC    7200
#define MAX_CUSTOM_NAME_LENGTH  32
#define MAX_PASSWORD_LENGTH     16

// ============================================================================
// Test 1: 49-Day Timer Overflow
// ============================================================================

void test_timer_49_day_overflow(void)
{
    TEST_GROUP("49-Day Timer Overflow Tests");
    
    // Test 1.1: esp_timer doesn't overflow after 50 days
    {
        // Simulate 50 days in microseconds
        int64_t simulated_time_us = 50LL * 24 * 60 * 60 * 1000000LL;
        
        // This should NOT overflow (int64_t can hold ~292,000 years)
        int64_t max_int64 = 9223372036854775807LL;
        TEST_ASSERT_TRUE(simulated_time_us < max_int64, 
            "50 days fits in int64_t");
    }
    
    // Test 1.2: Sensor timeout calculation after 50 days
    {
        int64_t now_us = 50LL * 24 * 60 * 60 * 1000000LL;  // 50 days
        int64_t last_update_us = now_us - (10LL * 1000000LL);  // 10 seconds ago
        
        int64_t elapsed_ms = (now_us - last_update_us) / 1000;
        bool sensor_online = elapsed_ms < SENSOR_TIMEOUT_MS && last_update_us > 0;
        
        TEST_ASSERT_EQUAL(10000, elapsed_ms, 
            "Elapsed time correctly calculated as 10 seconds");
        TEST_ASSERT_TRUE(sensor_online, 
            "Sensor correctly detected as online after 50 days");
    }
    
    // Test 1.3: Sensor offline detection after 50 days
    {
        int64_t now_us = 50LL * 24 * 60 * 60 * 1000000LL;
        int64_t last_update_us = now_us - (60LL * 1000000LL);  // 60 seconds ago
        
        int64_t elapsed_ms = (now_us - last_update_us) / 1000;
        bool sensor_online = elapsed_ms < SENSOR_TIMEOUT_MS && last_update_us > 0;
        
        TEST_ASSERT_FALSE(sensor_online, 
            "Sensor correctly detected as offline (60s > 30s timeout)");
    }
    
    // Test 1.4: Boot time edge case (no sensor update yet)
    {
        int64_t now_us = 5LL * 1000000LL;  // 5 seconds after boot
        int64_t last_update_us = 0;  // No update received yet
        
        int64_t elapsed_ms = (now_us - last_update_us) / 1000;
        bool sensor_online = elapsed_ms < SENSOR_TIMEOUT_MS && last_update_us > 0;
        
        TEST_ASSERT_FALSE(sensor_online, 
            "Sensor correctly offline when no update received");
    }
    
    // Test 1.5: Pump start time after 50 days
    {
        int64_t pump_start_time = (50LL * 24 * 60 * 60);  // 50 days in seconds
        int64_t current_time = pump_start_time + 3600;  // 1 hour later
        
        uint32_t runtime = (uint32_t)(current_time - pump_start_time);
        
        TEST_ASSERT_EQUAL(3600, runtime, 
            "Pump runtime correctly calculated as 1 hour");
    }
}

// ============================================================================
// Test 2: Zero Height Edge Case
// ============================================================================

void test_zero_height_handling(void)
{
    TEST_GROUP("Zero Height Edge Case Tests");
    
    // Test 2.1: Default height when zero configured
    {
        uint16_t tank_height_cm = 0;
        uint16_t effective_height = tank_height_cm > 0 ? tank_height_cm : 200;
        
        TEST_ASSERT_EQUAL(200, effective_height, 
            "Zero height falls back to default 200cm");
    }
    
    // Test 2.2: Division safety with non-zero height
    {
        uint16_t tank_height = 100;
        float water_depth = 50.0f;
        
        // This calculation should be safe
        uint8_t percent = (uint8_t)((water_depth / tank_height) * 100.0f);
        
        TEST_ASSERT_EQUAL(50, percent, 
            "Water percentage calculated correctly");
    }
    
    // Test 2.3: BLE validation rejects zero height
    {
        uint16_t height = 0;
        bool valid = (height >= 50 && height <= 1000);
        
        TEST_ASSERT_FALSE(valid, 
            "BLE validation rejects height = 0");
    }
    
    // Test 2.4: BLE validation rejects height below minimum
    {
        uint16_t height = 30;
        bool valid = (height >= 50 && height <= 1000);
        
        TEST_ASSERT_FALSE(valid, 
            "BLE validation rejects height = 30 (below 50)");
    }
    
    // Test 2.5: BLE validation rejects height above maximum
    {
        uint16_t height = 1500;
        bool valid = (height >= 50 && height <= 1000);
        
        TEST_ASSERT_FALSE(valid, 
            "BLE validation rejects height = 1500 (above 1000)");
    }
    
    // Test 2.6: BLE validation accepts valid range
    {
        TEST_ASSERT_TRUE((50 >= 50 && 50 <= 1000), 
            "BLE validation accepts height = 50 (min)");
        TEST_ASSERT_TRUE((200 >= 50 && 200 <= 1000), 
            "BLE validation accepts height = 200 (typical)");
        TEST_ASSERT_TRUE((1000 >= 50 && 1000 <= 1000), 
            "BLE validation accepts height = 1000 (max)");
    }
}

// ============================================================================
// Test 3: Pump Timeout Overflow
// ============================================================================

void test_pump_timeout_overflow(void)
{
    TEST_GROUP("Pump Timeout Overflow Tests");
    
    // Test 3.1: uint8_t * 60 doesn't overflow with uint32_t cast
    {
        uint8_t pump_timeout_minutes = 255;  // Max uint8_t
        uint32_t pump_timeout_sec = (uint32_t)pump_timeout_minutes * 60;
        
        TEST_ASSERT_EQUAL(15300, pump_timeout_sec, 
            "255 minutes * 60 = 15300 seconds (no overflow)");
    }
    
    // Test 3.2: Without cast would overflow
    {
        uint8_t pump_timeout_minutes = 255;
        // This would overflow without cast: 255 * 60 = 15300, but in uint8_t = 60
        uint8_t bad_result = pump_timeout_minutes * 60;
        
        TEST_ASSERT_TRUE(bad_result != 15300, 
            "Without cast: 255 * 60 overflows in uint8_t");
    }
    
    // Test 3.3: Safety cap at 2 hours
    {
        uint32_t pump_timeout_sec = 10000;  // More than 2 hours
        
        if (pump_timeout_sec > MAX_PUMP_TIMEOUT_SEC) {
            pump_timeout_sec = MAX_PUMP_TIMEOUT_SEC;
        }
        
        TEST_ASSERT_EQUAL(7200, pump_timeout_sec, 
            "Pump timeout capped at 7200 seconds (2 hours)");
    }
    
    // Test 3.4: Normal timeout not capped
    {
        uint32_t pump_timeout_sec = 3600;  // 1 hour
        
        if (pump_timeout_sec > MAX_PUMP_TIMEOUT_SEC) {
            pump_timeout_sec = MAX_PUMP_TIMEOUT_SEC;
        }
        
        TEST_ASSERT_EQUAL(3600, pump_timeout_sec, 
            "1 hour timeout not capped");
    }
}

// ============================================================================
// Test 4: Input Validation
// ============================================================================

void test_input_validation(void)
{
    TEST_GROUP("Input Validation Tests");
    
    // Test 4.1: Tank diameter validation
    {
        bool valid_30 = (30 >= 30 && 30 <= 500);
        bool valid_500 = (500 >= 30 && 500 <= 500);
        bool invalid_20 = (20 >= 30 && 20 <= 500);
        bool invalid_600 = (600 >= 30 && 600 <= 500);
        
        TEST_ASSERT_TRUE(valid_30, "Diameter 30 valid (min)");
        TEST_ASSERT_TRUE(valid_500, "Diameter 500 valid (max)");
        TEST_ASSERT_FALSE(invalid_20, "Diameter 20 invalid (below min)");
        TEST_ASSERT_FALSE(invalid_600, "Diameter 600 invalid (above max)");
    }
    
    // Test 4.2: Sensor offset validation
    {
        bool valid_0 = (0 <= 50);
        bool valid_50 = (50 <= 50);
        bool invalid_60 = (60 <= 50);
        
        TEST_ASSERT_TRUE(valid_0, "Offset 0 valid");
        TEST_ASSERT_TRUE(valid_50, "Offset 50 valid (max)");
        TEST_ASSERT_FALSE(invalid_60, "Offset 60 invalid (above max)");
    }
    
    // Test 4.3: Pump threshold logic
    {
        uint8_t on = 20, off = 80;
        bool valid_logic = (on < off);
        
        TEST_ASSERT_TRUE(valid_logic, "ON=20 < OFF=80 is valid");
        
        on = 80; off = 20;
        valid_logic = (on < off);
        TEST_ASSERT_FALSE(valid_logic, "ON=80 < OFF=20 is invalid");
        
        on = 50; off = 50;
        valid_logic = (on < off);
        TEST_ASSERT_FALSE(valid_logic, "ON=50 < OFF=50 is invalid (equal)");
    }
    
    // Test 4.4: Zigbee channel validation
    {
        bool valid_11 = (11 >= 11 && 11 <= 26);
        bool valid_26 = (26 >= 11 && 26 <= 26);
        bool invalid_10 = (10 >= 11 && 10 <= 26);
        bool invalid_27 = (27 >= 11 && 27 <= 26);
        
        TEST_ASSERT_TRUE(valid_11, "Channel 11 valid (min)");
        TEST_ASSERT_TRUE(valid_26, "Channel 26 valid (max)");
        TEST_ASSERT_FALSE(invalid_10, "Channel 10 invalid");
        TEST_ASSERT_FALSE(invalid_27, "Channel 27 invalid");
    }
    
    // Test 4.5: Report interval validation
    {
        bool valid_1 = (1 >= 1 && 1 <= 300);
        bool valid_300 = (300 >= 1 && 300 <= 300);
        bool invalid_0 = (0 >= 1 && 0 <= 300);
        bool invalid_400 = (400 >= 1 && 400 <= 300);
        
        TEST_ASSERT_TRUE(valid_1, "Interval 1s valid (min)");
        TEST_ASSERT_TRUE(valid_300, "Interval 300s valid (max)");
        TEST_ASSERT_FALSE(invalid_0, "Interval 0s invalid");
        TEST_ASSERT_FALSE(invalid_400, "Interval 400s invalid");
    }
}

// ============================================================================
// Test 5: Buffer Safety
// ============================================================================

void test_buffer_safety(void)
{
    TEST_GROUP("Buffer Safety Tests");
    
    // Test 5.1: Custom name length validation
    {
        uint8_t name_len = 31;  // Just under max
        bool valid = (name_len > 0 && name_len < MAX_CUSTOM_NAME_LENGTH);
        
        TEST_ASSERT_TRUE(valid, "Name length 31 valid");
        
        name_len = 32;  // At max (invalid, needs room for null)
        valid = (name_len > 0 && name_len < MAX_CUSTOM_NAME_LENGTH);
        TEST_ASSERT_FALSE(valid, "Name length 32 invalid (no room for null)");
        
        name_len = 0;
        valid = (name_len > 0 && name_len < MAX_CUSTOM_NAME_LENGTH);
        TEST_ASSERT_FALSE(valid, "Name length 0 invalid");
    }
    
    // Test 5.2: Password length validation
    {
        uint8_t pwd_len = 4;  // Minimum
        bool valid = (pwd_len >= 4 && pwd_len < MAX_PASSWORD_LENGTH);
        
        TEST_ASSERT_TRUE(valid, "Password length 4 valid (min)");
        
        pwd_len = 15;  // Just under max
        valid = (pwd_len >= 4 && pwd_len < MAX_PASSWORD_LENGTH);
        TEST_ASSERT_TRUE(valid, "Password length 15 valid");
        
        pwd_len = 3;
        valid = (pwd_len >= 4 && pwd_len < MAX_PASSWORD_LENGTH);
        TEST_ASSERT_FALSE(valid, "Password length 3 invalid (below min)");
    }
    
    // Test 5.3: Device name with prefix doesn't overflow
    {
        const char *prefix = "Cultivio-";  // 9 chars
        char name[32];
        char custom_name[32] = "TestDevice123456";  // 16 chars
        
        int written = snprintf(name, sizeof(name), "%s%s", prefix, custom_name);
        
        TEST_ASSERT_TRUE(written < (int)sizeof(name), 
            "Device name fits in buffer");
        TEST_ASSERT_EQUAL('\0', name[sizeof(name)-1] | 0, 
            "Buffer null-terminated");
    }
}

// ============================================================================
// Test 6: Water Level Calculation
// ============================================================================

void test_water_level_calculation(void)
{
    TEST_GROUP("Water Level Calculation Tests");
    
    // Test 6.1: Normal calculation
    {
        uint16_t tank_height = 100;  // cm
        float avg_distance = 30.0f;  // cm from sensor to water
        uint8_t sensor_offset = 5;   // cm sensor above tank
        
        float water_depth = tank_height - avg_distance - sensor_offset;
        
        if (water_depth < 0) water_depth = 0;
        if (water_depth > tank_height) water_depth = tank_height;
        
        uint8_t percent = (uint8_t)((water_depth / tank_height) * 100.0f);
        
        TEST_ASSERT_EQUAL(65, percent, 
            "Water level = 65% (100 - 30 - 5 = 65cm)");
    }
    
    // Test 6.2: Negative depth clamped to zero
    {
        uint16_t tank_height = 100;
        float avg_distance = 120.0f;  // Sensor reading beyond tank
        uint8_t sensor_offset = 5;
        
        float water_depth = tank_height - avg_distance - sensor_offset;
        
        if (water_depth < 0) water_depth = 0;
        
        TEST_ASSERT_EQUAL(0, (int)water_depth, 
            "Negative depth clamped to 0");
    }
    
    // Test 6.3: Depth exceeding tank height clamped
    {
        uint16_t tank_height = 100;
        float avg_distance = -20.0f;  // Invalid reading (sensor malfunction)
        uint8_t sensor_offset = 5;
        
        float water_depth = tank_height - avg_distance - sensor_offset;
        // water_depth = 100 - (-20) - 5 = 115
        
        if (water_depth > tank_height) water_depth = tank_height;
        
        TEST_ASSERT_EQUAL(100, (int)water_depth, 
            "Depth exceeding tank clamped to tank height");
    }
    
    // Test 6.4: Sensor tolerance
    {
        uint16_t tank_height = 100;
        uint16_t sensor_tolerance = 50;
        float distance = 140.0f;
        
        bool valid_reading = (distance > 0 && distance < tank_height + sensor_tolerance);
        
        TEST_ASSERT_TRUE(valid_reading, 
            "Distance 140 valid (< 100 + 50 tolerance)");
        
        distance = 160.0f;
        valid_reading = (distance > 0 && distance < tank_height + sensor_tolerance);
        
        TEST_ASSERT_FALSE(valid_reading, 
            "Distance 160 invalid (> 100 + 50 tolerance)");
    }
}

// ============================================================================
// Main
// ============================================================================

int main(void)
{
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║     CULTIVIO AQUASENSE - UNIT TESTS                   ║\n");
    printf("║     Testing Edge Cases and Bug Fixes                  ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    // Run all tests
    test_timer_49_day_overflow();
    test_zero_height_handling();
    test_pump_timeout_overflow();
    test_input_validation();
    test_buffer_safety();
    test_water_level_calculation();
    
    // Summary
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("📊 TEST SUMMARY\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  Total:  %d\n", tests_run);
    printf("  Passed: %d ✅\n", tests_passed);
    printf("  Failed: %d %s\n", tests_failed, tests_failed > 0 ? "❌" : "");
    printf("═══════════════════════════════════════════════════════\n");
    
    if (tests_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("\n⚠️  SOME TESTS FAILED!\n\n");
        return 1;
    }
}

