idf_component_register(
    SRCS "unified_main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        esp-zigbee-lib
        esp-zboss-lib
        nvs_flash
        driver
        freertos
        esp_timer
        log
        ble_provision
)

