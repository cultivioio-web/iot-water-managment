/*
 * Mock ESP-IDF Functions for Native Testing
 * Allows running firmware logic on PC without hardware
 */

#ifndef MOCK_ESP_H
#define MOCK_ESP_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================================
 * TYPE DEFINITIONS
 * ============================================================================ */

typedef int esp_err_t;
#define ESP_OK          0
#define ESP_FAIL        -1
#define ESP_ERR_TIMEOUT -2
#define ESP_ERR_NO_MEM  -3

/* ============================================================================
 * MOCK TIME FUNCTIONS
 * ============================================================================ */

static int64_t g_mock_time_us = 0;

static inline int64_t esp_timer_get_time(void) {
    return g_mock_time_us;
}

static inline void mock_set_time_us(int64_t time_us) {
    g_mock_time_us = time_us;
}

static inline void mock_advance_time_sec(uint32_t seconds) {
    g_mock_time_us += (int64_t)seconds * 1000000;
}

static inline void mock_advance_time_ms(uint32_t ms) {
    g_mock_time_us += (int64_t)ms * 1000;
}

/* ============================================================================
 * MOCK LOGGING
 * ============================================================================ */

#define ESP_LOGE(tag, fmt, ...) printf("[ERROR][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) printf("[WARN][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) printf("[INFO][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) /* Debug disabled in tests */

/* ============================================================================
 * MOCK GPIO
 * ============================================================================ */

#define GPIO_NUM_0  0
#define GPIO_NUM_1  1
#define GPIO_NUM_2  2
#define GPIO_NUM_3  3
#define GPIO_NUM_8  8
#define GPIO_NUM_9  9
#define GPIO_NUM_10 10

typedef int gpio_num_t;

static int g_mock_gpio_levels[32] = {0};

static inline void gpio_set_level(gpio_num_t pin, int level) {
    if (pin < 32) g_mock_gpio_levels[pin] = level;
}

static inline int gpio_get_level(gpio_num_t pin) {
    return (pin < 32) ? g_mock_gpio_levels[pin] : 0;
}

static inline void mock_set_gpio(gpio_num_t pin, int level) {
    if (pin < 32) g_mock_gpio_levels[pin] = level;
}

/* ============================================================================
 * MOCK FreeRTOS
 * ============================================================================ */

#define pdMS_TO_TICKS(ms) (ms)
#define portMAX_DELAY 0xFFFFFFFF

typedef void* SemaphoreHandle_t;

static inline void vTaskDelay(uint32_t ticks) {
    /* No-op in tests */
    (void)ticks;
}

static inline void taskYIELD(void) {
    /* No-op in tests */
}

static inline SemaphoreHandle_t xSemaphoreCreateMutex(void) {
    return (SemaphoreHandle_t)1; /* Non-null = success */
}

static inline int xSemaphoreTake(SemaphoreHandle_t sem, uint32_t timeout) {
    (void)sem;
    (void)timeout;
    return 1; /* pdTRUE */
}

static inline void xSemaphoreGive(SemaphoreHandle_t sem) {
    (void)sem;
}

/* ============================================================================
 * MOCK NVS
 * ============================================================================ */

typedef int nvs_handle_t;

#define ESP_ERR_NVS_NO_FREE_PAGES   0x1100
#define ESP_ERR_NVS_NEW_VERSION_FOUND 0x1101

static inline esp_err_t nvs_flash_init(void) {
    return ESP_OK;
}

static inline esp_err_t nvs_flash_erase(void) {
    return ESP_OK;
}

/* ============================================================================
 * TEST ASSERTIONS
 * ============================================================================ */

static int g_test_failures = 0;
static int g_test_passes = 0;

#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        printf("  FAILED: %s (line %d)\n", #condition, __LINE__); \
        g_test_failures++; \
    } else { \
        g_test_passes++; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf("  FAILED: Expected %d, got %d (line %d)\n", (int)(expected), (int)(actual), __LINE__); \
        g_test_failures++; \
    } else { \
        g_test_passes++; \
    } \
} while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define RUN_TEST(test_func) do { \
    printf("  %s... ", #test_func); \
    int failures_before = g_test_failures; \
    test_func(); \
    if (g_test_failures == failures_before) { \
        printf("PASSED\n"); \
    } \
} while(0)

#define TEST_SUMMARY() do { \
    printf("\n================================\n"); \
    printf("Tests: %d passed, %d failed\n", g_test_passes, g_test_failures); \
    if (g_test_failures == 0) { \
        printf("All tests PASSED!\n"); \
    } else { \
        printf("FAILURES detected!\n"); \
    } \
    printf("================================\n"); \
} while(0)

#endif /* MOCK_ESP_H */

