/*
 * Cultivio AquaSense - Native Unit Tests
 * Run on PC without ESP32 hardware
 * 
 * Compile: gcc -o test_all test_all.c -I./mocks
 * Run: ./test_all
 */

#include "mocks/mock_esp.h"

static const char *TAG = "TEST";

/* ============================================================================
 * EXTRACTED LOGIC FROM FIRMWARE (for testing)
 * ============================================================================ */

// Simulated config structure
typedef struct {
    uint16_t tank_height_cm;
    uint16_t tank_diameter_cm;
    uint8_t  sensor_offset_cm;
    uint8_t  pump_on_threshold;
    uint8_t  pump_off_threshold;
    uint16_t pump_timeout_minutes;
    uint16_t report_interval_sec;
} test_config_t;

static test_config_t g_config = {
    .tank_height_cm = 200,
    .tank_diameter_cm = 100,
    .sensor_offset_cm = 5,
    .pump_on_threshold = 20,
    .pump_off_threshold = 80,
    .pump_timeout_minutes = 60,
    .report_interval_sec = 5
};

// Simulated globals
static uint8_t  g_water_level_percent = 0;
static uint16_t g_water_level_cm = 0;
static uint8_t  g_sensor_status = 0;
static bool     g_pump_running = false;
static uint32_t g_pump_start_time = 0;
static bool     g_manual_override = false;
static uint32_t g_manual_override_end_time = 0;
static int64_t  g_last_sensor_update_us = 0;

#define SENSOR_TIMEOUT_MS       30000
#define MAX_PUMP_TIMEOUT_SEC    7200
#define SENSOR_TOLERANCE_CM     50

/* ============================================================================
 * WATER LEVEL CALCULATION (from sensor_node.c)
 * ============================================================================ */

typedef struct {
    float distances[5];
    int count;
} mock_samples_t;

static mock_samples_t g_mock_samples = {0};

static void mock_set_distance_samples(float d1, float d2, float d3, float d4, float d5) {
    g_mock_samples.distances[0] = d1;
    g_mock_samples.distances[1] = d2;
    g_mock_samples.distances[2] = d3;
    g_mock_samples.distances[3] = d4;
    g_mock_samples.distances[4] = d5;
    g_mock_samples.count = 0;
}

static float measure_distance_cm_mock(void) {
    if (g_mock_samples.count < 5) {
        return g_mock_samples.distances[g_mock_samples.count++];
    }
    return -1.0f;
}

static void calculate_water_level(void) {
    float total_distance = 0;
    int valid_samples = 0;
    
    uint16_t tank_height = g_config.tank_height_cm > 0 ? g_config.tank_height_cm : 200;
    uint8_t sensor_offset = g_config.sensor_offset_cm;

    g_mock_samples.count = 0;
    for (int i = 0; i < 5; i++) {
        float distance = measure_distance_cm_mock();
        if (distance > 0 && distance < tank_height + SENSOR_TOLERANCE_CM) {
            total_distance += distance;
            valid_samples++;
        }
    }

    if (valid_samples > 0) {
        // Zero height check
        if (tank_height == 0) {
            g_sensor_status = 1;
            g_water_level_percent = 0;
            g_water_level_cm = 0;
            return;
        }
        
        float avg_distance = total_distance / valid_samples;
        float water_depth = tank_height - avg_distance - sensor_offset;
        
        if (water_depth < 0) water_depth = 0;
        if (water_depth > tank_height) water_depth = tank_height;

        g_water_level_cm = (uint16_t)water_depth;
        g_water_level_percent = (uint8_t)((water_depth / tank_height) * 100.0f);
        g_sensor_status = 0;
    } else {
        g_sensor_status = 1;
    }
}

/* ============================================================================
 * PUMP CONTROL LOGIC (from controller_node.c)
 * ============================================================================ */

static void pump_on(void) {
    if (!g_pump_running) {
        g_pump_running = true;
        g_pump_start_time = esp_timer_get_time() / 1000000;
    }
}

static void pump_off(void) {
    if (g_manual_override) {
        g_manual_override = false;
        g_manual_override_end_time = 0;
    }
    g_pump_running = false;
}

static void pump_control_logic(void) {
    int64_t now_us = esp_timer_get_time();
    uint32_t now_sec = (uint32_t)(now_us / 1000000);
    
    int64_t sensor_elapsed_ms = (now_us - g_last_sensor_update_us) / 1000;
    bool sensor_online = sensor_elapsed_ms < SENSOR_TIMEOUT_MS && g_last_sensor_update_us > 0;
    
    uint8_t pump_on_threshold = g_config.pump_on_threshold > 0 ? g_config.pump_on_threshold : 20;
    uint8_t pump_off_threshold = g_config.pump_off_threshold > 0 ? g_config.pump_off_threshold : 80;
    
    uint32_t pump_timeout_sec = g_config.pump_timeout_minutes > 0 ? 
                                ((uint32_t)g_config.pump_timeout_minutes * 60) : 3600;
    
    if (pump_timeout_sec > MAX_PUMP_TIMEOUT_SEC) {
        pump_timeout_sec = MAX_PUMP_TIMEOUT_SEC;
    }
    
    // Manual override mode
    if (g_manual_override) {
        if (now_sec >= g_manual_override_end_time) {
            g_manual_override = false;
            g_manual_override_end_time = 0;
            pump_off();
            return;
        }
        if (!g_pump_running) {
            pump_on();
        }
        return;
    }
    
    // Sensor offline check
    if (!sensor_online) {
        if (g_pump_running) {
            pump_off();
        }
        return;
    }

    // Pump timeout check
    if (g_pump_running) {
        uint32_t runtime = now_sec - g_pump_start_time;
        if (runtime >= pump_timeout_sec) {
            pump_off();
            return;
        }
    }

    // Auto control logic
    if (g_water_level_percent <= pump_on_threshold && !g_pump_running) {
        pump_on();
    }
    else if (g_water_level_percent >= pump_off_threshold && g_pump_running) {
        pump_off();
    }
}

/* ============================================================================
 * INPUT VALIDATION (from ble_provision.c)
 * ============================================================================ */

static bool validate_tank_height(uint16_t height) {
    return (height >= 50 && height <= 1000);
}

static bool validate_tank_diameter(uint16_t diameter) {
    return (diameter >= 30 && diameter <= 500);
}

static bool validate_sensor_offset(uint8_t offset) {
    return (offset <= 50);
}

static bool validate_pump_thresholds(uint8_t on_threshold, uint8_t off_threshold) {
    return (on_threshold <= 100 && off_threshold <= 100 && on_threshold < off_threshold);
}

static bool validate_pump_timeout(uint16_t timeout) {
    return (timeout >= 1 && timeout <= 120);
}

static bool validate_zigbee_channel(uint8_t channel) {
    return (channel >= 11 && channel <= 26);
}

static bool validate_report_interval(uint16_t interval) {
    return (interval >= 1 && interval <= 300);
}

/* ============================================================================
 * TEST: WATER LEVEL CALCULATION
 * ============================================================================ */

void test_water_level_normal_reading(void) {
    g_config.tank_height_cm = 200;
    g_config.sensor_offset_cm = 0;
    
    // Distance 50cm from top = 150cm water depth = 75%
    mock_set_distance_samples(50.0f, 50.0f, 50.0f, 50.0f, 50.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(0, g_sensor_status);
    TEST_ASSERT_EQUAL(75, g_water_level_percent);
    TEST_ASSERT_EQUAL(150, g_water_level_cm);
}

void test_water_level_zero_height(void) {
    g_config.tank_height_cm = 0;  // Invalid!
    
    mock_set_distance_samples(50.0f, 50.0f, 50.0f, 50.0f, 50.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(1, g_sensor_status);  // Error status
    TEST_ASSERT_EQUAL(0, g_water_level_percent);
}

void test_water_level_full_tank(void) {
    g_config.tank_height_cm = 200;
    g_config.sensor_offset_cm = 0;
    
    // Distance 0 = full tank
    mock_set_distance_samples(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(0, g_sensor_status);
    TEST_ASSERT_EQUAL(100, g_water_level_percent);
}

void test_water_level_empty_tank(void) {
    g_config.tank_height_cm = 200;
    g_config.sensor_offset_cm = 0;
    
    // Distance 200 = empty tank
    mock_set_distance_samples(200.0f, 200.0f, 200.0f, 200.0f, 200.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(0, g_sensor_status);
    TEST_ASSERT_EQUAL(0, g_water_level_percent);
}

void test_water_level_invalid_samples(void) {
    g_config.tank_height_cm = 200;
    
    // All invalid readings
    mock_set_distance_samples(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(1, g_sensor_status);  // Error
}

void test_water_level_partial_samples(void) {
    g_config.tank_height_cm = 200;
    g_config.sensor_offset_cm = 0;
    
    // 3 valid, 2 invalid -> should use avg of 3
    mock_set_distance_samples(100.0f, 100.0f, 100.0f, -1.0f, -1.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(0, g_sensor_status);
    TEST_ASSERT_EQUAL(50, g_water_level_percent);  // 100cm depth
}

void test_water_level_with_offset(void) {
    g_config.tank_height_cm = 200;
    g_config.sensor_offset_cm = 10;
    
    // Distance 50cm, offset 10cm -> water depth = 200 - 50 - 10 = 140cm = 70%
    mock_set_distance_samples(50.0f, 50.0f, 50.0f, 50.0f, 50.0f);
    calculate_water_level();
    
    TEST_ASSERT_EQUAL(0, g_sensor_status);
    TEST_ASSERT_EQUAL(70, g_water_level_percent);
}

/* ============================================================================
 * TEST: PUMP CONTROL LOGIC
 * ============================================================================ */

void test_pump_low_water_trigger(void) {
    // Reset state
    g_pump_running = false;
    g_manual_override = false;
    g_config.pump_on_threshold = 20;
    g_config.pump_off_threshold = 80;
    g_water_level_percent = 15;  // Below threshold
    
    // Set sensor as online
    mock_set_time_us(10000000);  // 10 seconds
    g_last_sensor_update_us = 9000000;  // 1 second ago
    
    pump_control_logic();
    
    TEST_ASSERT_TRUE(g_pump_running);
}

void test_pump_high_water_stop(void) {
    // Setup: pump running, water high
    g_pump_running = true;
    g_pump_start_time = 0;
    g_manual_override = false;
    g_config.pump_on_threshold = 20;
    g_config.pump_off_threshold = 80;
    g_water_level_percent = 85;  // Above threshold
    
    mock_set_time_us(10000000);
    g_last_sensor_update_us = 9000000;
    
    pump_control_logic();
    
    TEST_ASSERT_FALSE(g_pump_running);
}

void test_pump_hysteresis(void) {
    // Water at 50% - no action should be taken
    g_pump_running = false;
    g_manual_override = false;
    g_config.pump_on_threshold = 20;
    g_config.pump_off_threshold = 80;
    g_water_level_percent = 50;
    
    mock_set_time_us(10000000);
    g_last_sensor_update_us = 9000000;
    
    pump_control_logic();
    
    TEST_ASSERT_FALSE(g_pump_running);  // Should stay off
}

void test_pump_timeout(void) {
    // Pump running for too long
    g_pump_running = true;
    g_pump_start_time = 0;  // Started at time 0
    g_manual_override = false;
    g_config.pump_timeout_minutes = 60;  // 60 min = 3600 sec
    g_water_level_percent = 50;
    
    // Time is now 3601 seconds (past timeout)
    mock_set_time_us(3601000000LL);
    g_last_sensor_update_us = 3600000000LL;
    
    pump_control_logic();
    
    TEST_ASSERT_FALSE(g_pump_running);  // Should be off due to timeout
}

void test_pump_manual_override(void) {
    // Manual override active
    g_pump_running = false;
    g_manual_override = true;
    g_manual_override_end_time = 1000;  // Ends at second 1000
    g_water_level_percent = 90;  // High water (would normally stop pump)
    
    mock_set_time_us(500000000LL);  // 500 seconds (before end)
    g_last_sensor_update_us = 499000000LL;
    
    pump_control_logic();
    
    TEST_ASSERT_TRUE(g_pump_running);  // Manual override keeps it on
    TEST_ASSERT_TRUE(g_manual_override);
}

void test_pump_manual_override_expire(void) {
    // Manual override expired
    g_pump_running = true;
    g_manual_override = true;
    g_manual_override_end_time = 1000;
    
    mock_set_time_us(1001000000LL);  // 1001 seconds (after end)
    g_last_sensor_update_us = 1000000000LL;
    
    pump_control_logic();
    
    TEST_ASSERT_FALSE(g_pump_running);
    TEST_ASSERT_FALSE(g_manual_override);
}

void test_pump_sensor_offline(void) {
    // Sensor offline (no updates for 30+ seconds)
    g_pump_running = true;
    g_manual_override = false;
    g_water_level_percent = 50;
    
    mock_set_time_us(60000000LL);  // 60 seconds
    g_last_sensor_update_us = 10000000LL;  // Last update at 10 sec (50 sec ago)
    
    pump_control_logic();
    
    TEST_ASSERT_FALSE(g_pump_running);  // Should stop due to sensor offline
}

/* ============================================================================
 * TEST: TIME CALCULATIONS (49-day overflow)
 * ============================================================================ */

void test_time_49_day_overflow(void) {
    // Simulate 50 days in microseconds
    // 50 days = 50 * 24 * 60 * 60 * 1000000 = 4,320,000,000,000 us
    int64_t fifty_days_us = 50LL * 24 * 60 * 60 * 1000000;
    
    mock_set_time_us(fifty_days_us);
    g_last_sensor_update_us = fifty_days_us - 1000000;  // 1 second ago
    
    // This should NOT overflow
    int64_t elapsed_ms = (esp_timer_get_time() - g_last_sensor_update_us) / 1000;
    
    TEST_ASSERT_EQUAL(1000, elapsed_ms);  // Should be 1000ms, not negative
}

void test_time_manual_override_50_days(void) {
    // Start manual override at day 50
    int64_t fifty_days_us = 50LL * 24 * 60 * 60 * 1000000;
    uint32_t fifty_days_sec = (uint32_t)(fifty_days_us / 1000000);
    
    mock_set_time_us(fifty_days_us);
    
    // Set 10 minute override
    g_manual_override = true;
    g_manual_override_end_time = fifty_days_sec + 600;  // +10 min
    g_pump_running = false;
    
    pump_control_logic();
    
    TEST_ASSERT_TRUE(g_pump_running);  // Should still work
}

/* ============================================================================
 * TEST: INPUT VALIDATION
 * ============================================================================ */

void test_validate_tank_height(void) {
    TEST_ASSERT_FALSE(validate_tank_height(0));
    TEST_ASSERT_FALSE(validate_tank_height(49));
    TEST_ASSERT_TRUE(validate_tank_height(50));
    TEST_ASSERT_TRUE(validate_tank_height(200));
    TEST_ASSERT_TRUE(validate_tank_height(1000));
    TEST_ASSERT_FALSE(validate_tank_height(1001));
}

void test_validate_tank_diameter(void) {
    TEST_ASSERT_FALSE(validate_tank_diameter(0));
    TEST_ASSERT_FALSE(validate_tank_diameter(29));
    TEST_ASSERT_TRUE(validate_tank_diameter(30));
    TEST_ASSERT_TRUE(validate_tank_diameter(100));
    TEST_ASSERT_TRUE(validate_tank_diameter(500));
    TEST_ASSERT_FALSE(validate_tank_diameter(501));
}

void test_validate_pump_thresholds(void) {
    TEST_ASSERT_TRUE(validate_pump_thresholds(20, 80));
    TEST_ASSERT_TRUE(validate_pump_thresholds(10, 90));
    TEST_ASSERT_FALSE(validate_pump_thresholds(80, 20));  // on > off
    TEST_ASSERT_FALSE(validate_pump_thresholds(50, 50));  // on == off
    TEST_ASSERT_FALSE(validate_pump_thresholds(101, 80)); // on > 100
}

void test_validate_zigbee_channel(void) {
    TEST_ASSERT_FALSE(validate_zigbee_channel(10));
    TEST_ASSERT_TRUE(validate_zigbee_channel(11));
    TEST_ASSERT_TRUE(validate_zigbee_channel(15));
    TEST_ASSERT_TRUE(validate_zigbee_channel(26));
    TEST_ASSERT_FALSE(validate_zigbee_channel(27));
}

void test_validate_report_interval(void) {
    TEST_ASSERT_FALSE(validate_report_interval(0));
    TEST_ASSERT_TRUE(validate_report_interval(1));
    TEST_ASSERT_TRUE(validate_report_interval(5));
    TEST_ASSERT_TRUE(validate_report_interval(300));
    TEST_ASSERT_FALSE(validate_report_interval(301));
}

/* ============================================================================
 * MAIN TEST RUNNER
 * ============================================================================ */

int main(void) {
    printf("\n========================================\n");
    printf("Cultivio AquaSense - Unit Tests\n");
    printf("========================================\n\n");
    
    // Water Level Tests
    printf("Water Level Calculation Tests:\n");
    RUN_TEST(test_water_level_normal_reading);
    RUN_TEST(test_water_level_zero_height);
    RUN_TEST(test_water_level_full_tank);
    RUN_TEST(test_water_level_empty_tank);
    RUN_TEST(test_water_level_invalid_samples);
    RUN_TEST(test_water_level_partial_samples);
    RUN_TEST(test_water_level_with_offset);
    
    // Pump Control Tests
    printf("\nPump Control Logic Tests:\n");
    RUN_TEST(test_pump_low_water_trigger);
    RUN_TEST(test_pump_high_water_stop);
    RUN_TEST(test_pump_hysteresis);
    RUN_TEST(test_pump_timeout);
    RUN_TEST(test_pump_manual_override);
    RUN_TEST(test_pump_manual_override_expire);
    RUN_TEST(test_pump_sensor_offline);
    
    // Time Calculation Tests
    printf("\nTime Calculation Tests (49-day overflow):\n");
    RUN_TEST(test_time_49_day_overflow);
    RUN_TEST(test_time_manual_override_50_days);
    
    // Input Validation Tests
    printf("\nInput Validation Tests:\n");
    RUN_TEST(test_validate_tank_height);
    RUN_TEST(test_validate_tank_diameter);
    RUN_TEST(test_validate_pump_thresholds);
    RUN_TEST(test_validate_zigbee_channel);
    RUN_TEST(test_validate_report_interval);
    
    TEST_SUMMARY();
    
    return g_test_failures > 0 ? 1 : 0;
}

