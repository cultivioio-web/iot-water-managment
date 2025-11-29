set(_zigbee_fw_candidates
    "${CMAKE_CURRENT_LIST_DIR}/../zigbee_water_management"
    "${CMAKE_CURRENT_LIST_DIR}/../firmware/zigbee_water_management"
    "${CMAKE_CURRENT_LIST_DIR}/../../zigbee_water_management"
)

foreach(_candidate IN LISTS _zigbee_fw_candidates)
    if(EXISTS "${_candidate}/zigbee_water_management.ino")
        set(ZIGBEE_SHARED_DIR "${_candidate}")
        break()
    endif()
endforeach()

if(NOT DEFINED ZIGBEE_SHARED_DIR)
    list(JOIN _zigbee_fw_candidates "\n    * " _zigbee_search_paths)
    message(FATAL_ERROR
        "Unable to locate zigbee firmware sources. Expected zigbee_water_management.ino in one of:\n"
        "    * ${_zigbee_search_paths}\n"
        "Please sync the shared firmware next to this project (recommended: ../zigbee_water_management).")
endif()

# Check if target supports native Zigbee (802.15.4 hardware)
if(CONFIG_IDF_TARGET_ESP32C6 OR CONFIG_IDF_TARGET_ESP32H2)
    set(ZIGBEE_SUPPORTED TRUE)
    set(EXTRA_SRCS "${ZIGBEE_SHARED_DIR}/zigbee_config.cpp")
    set(EXTRA_REQUIRES esp-zigbee-lib)
    message(STATUS "=== Building with ZIGBEE support for ${CONFIG_IDF_TARGET} ===")
else()
    set(ZIGBEE_SUPPORTED FALSE)
    set(EXTRA_SRCS "")
    set(EXTRA_REQUIRES "")
    message(STATUS "=== Building WIFI-ONLY mode for ${CONFIG_IDF_TARGET} (no native Zigbee) ===")
endif()

idf_component_register(
    SRCS 
        "main.cpp"
        "zigbee_ino_wrapper.cpp"
        ${EXTRA_SRCS}
    INCLUDE_DIRS
        "${ZIGBEE_SHARED_DIR}"
    REQUIRES arduino-esp32 ${EXTRA_REQUIRES}
)

target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)

# Define macro for conditional compilation in C/C++ code
if(ZIGBEE_SUPPORTED)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC ZIGBEE_ENABLED=1)
else()
    target_compile_definitions(${COMPONENT_LIB} PUBLIC ZIGBEE_ENABLED=0)
endif()
