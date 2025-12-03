idf_component_register(
    SRCS "test_main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        nvs_flash
        bt
        esp_timer
        driver
)

