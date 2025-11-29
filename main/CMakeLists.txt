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

set(ZIGBEE_SHARED_SRCS
    "${ZIGBEE_SHARED_DIR}/zigbee_water_management.ino"
    "${ZIGBEE_SHARED_DIR}/zigbee_config.cpp"
)

if(NOT CMAKE_SCRIPT_MODE_FILE)
    set_source_files_properties(${ZIGBEE_SHARED_SRCS} PROPERTIES LANGUAGE CXX)
endif()

idf_component_register(
    SRCS "main.cpp" ${ZIGBEE_SHARED_SRCS}
    INCLUDE_DIRS
        "${ZIGBEE_SHARED_DIR}"
    REQUIRES arduino-esp32 esp-zigbee-lib
)

target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)

