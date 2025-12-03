# Cultivio AquaSense - Test Firmware for ESP32-WROOM
# For testing BLE provisioning and controller logic without Zigbee

cmake_minimum_required(VERSION 3.16)

# Include shared components
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../shared")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cultivio_test_esp32)

