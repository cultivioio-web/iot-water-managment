/*
 * Radio Spinel Stub for ESP32
 * 
 * ESP32 doesn't have native 802.15.4 hardware. This stub allows compilation
 * but Zigbee won't function without an external radio co-processor (RCP).
 * 
 * For actual Zigbee functionality, use ESP32-H2 or ESP32-C6 which have
 * native 802.15.4 support, or connect an external RCP module.
 */

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "radio_spinel_stub";

// Stub for esp_radio_spinel_uart_interface_enable
// This function is expected by the Zigbee library for UART-based radio
esp_err_t esp_radio_spinel_uart_interface_enable(void *config, void *init_func, void *deinit_func, int idx)
{
    ESP_LOGW(TAG, "Radio spinel UART interface is a stub. Zigbee requires ESP32-H2/C6 or external RCP.");
    return ESP_ERR_NOT_SUPPORTED;
}

