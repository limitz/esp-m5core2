# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
list(APPEND EXTRA_COMPONENT_DIRS ..)
list(APPEND EXTRA_COMPONENT_DIRS ../components/lvgl)
list(APPEND EXTRA_COMPONENT_DIRS ../components/lvgl_esp32_drivers)
list(APPEND EXTRA_COMPONENT_DIRS ../components/lvgl_esp32_drivers/lvgl_tft)

project(m5core2_example)
