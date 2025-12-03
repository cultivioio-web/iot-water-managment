idf_component_register(
    SRCS "router_node.c"
    INCLUDE_DIRS "." "${CMAKE_CURRENT_SOURCE_DIR}/../../shared"
    PRIV_REQUIRES nvs_flash driver esp_timer
)

