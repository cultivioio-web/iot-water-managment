# Cultivio AquaSense - Router Node
# Zigbee Range Extender

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS 
    "${CMAKE_CURRENT_SOURCE_DIR}/../shared"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(router_node)

