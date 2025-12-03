idf_component_register(
    SRCS "ble_provision.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        nvs_flash
        bt
        freertos
        log
)

