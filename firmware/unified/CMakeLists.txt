# Cultivio AquaSense - Unified Firmware
# Single firmware for Sensor, Controller, and Router roles
# Role is selected during BLE provisioning

cmake_minimum_required(VERSION 3.16)

# Add shared components
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../shared")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cultivio_unified)

