set(ZIGBEE_SHARED_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/../../zigbee_water_management/zigbee_water_management.ino"
    "${CMAKE_CURRENT_LIST_DIR}/../../zigbee_water_management/zigbee_config.cpp"
)

set_source_files_properties(${ZIGBEE_SHARED_SRCS} PROPERTIES LANGUAGE CXX)

idf_component_register(
    SRCS "main.cpp" ${ZIGBEE_SHARED_SRCS}
    INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}/../../zigbee_water_management"
    REQUIRES arduino
)

target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)

