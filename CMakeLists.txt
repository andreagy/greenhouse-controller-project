cmake_minimum_required(VERSION 3.20)

# Language settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set board type because we are building for PicoW
# set(PICO_BOARD pico_w)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# set(PICO_CYW43_SUPPORTED 1)

# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS-KernelV10.6.2)

# Import those libraries
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# Define project
project(freertos-project LANGUAGES C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Source directory
add_subdirectory(src)

# External libraries/components
add_subdirectory(external)
