cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Suppress warnings from Arduino's headers
add_compile_options("-Wno-switch")

project(zigbee_water_management)
