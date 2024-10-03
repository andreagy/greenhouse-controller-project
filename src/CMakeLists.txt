# Source files
add_executable(${PROJECT_NAME}
        main.cpp

        display/framebuf.cpp
        display/mono_vlsb.cpp
        display/ssd1306os.cpp

        gpio/RotaryEncoder.cpp

        i2c/PicoI2C.cpp

        # ipstack/IPStack.cpp
        # ipstack/picow_tls_client.c
        # ipstack/tls_common.c
        modbus/MbClient.cpp
        modbus/Register.cpp

        network/TlsClient.cpp

        semaphore/Binary.cpp
        semaphore/Mutex.cpp

        sensor/GMP252.cpp
        sensor/HMP60.cpp
        sensor/SDP600.cpp

        # storage/Eeprom.cpp
        task/BaseTask.cpp
        task/co2/Co2Controller.cpp
        task/fan/FanController.cpp
        task/network/Manager.cpp

        timer/Timeout.cpp

        uart/PicoOsUart.cpp

        TestTask.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        config
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIFI_SSID=\"$ENV{WIFI_SSID}\"
        WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
        NO_SYS=0 # don't want NO_SYS (generally this would be in your lwipopts.h)
        PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE=CYW43_COUNTRY_FINLAND
)

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)

# Linked libraries
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
        FreeRTOS-Kernel-Heap4
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_mbedtls
        pico_mbedtls
)

# Ignore warnings from lwip code
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
)

pico_add_extra_outputs(${PROJECT_NAME})

# Disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
