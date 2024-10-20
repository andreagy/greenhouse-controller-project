# Source files
add_executable(${PROJECT_NAME}
        main.cpp

        display/framebuf.cpp
        display/mono_vlsb.cpp
        display/ssd1306os.cpp

        i2c/PicoI2C.cpp

        modbus/MbClient.cpp
        modbus/Register.cpp

        network/TlsClient.cpp

        semaphore/Binary.cpp
        semaphore/Mutex.cpp

        sensor/GMP252.cpp
        sensor/HMP60.cpp
        sensor/SDP600.cpp

        storage/Eeprom.cpp
        task/BaseTask.cpp
        task/co2/Co2Controller.cpp
        task/fan/FanController.cpp
        task/gpio/Input.cpp
        task/localUI/LocalUI.cpp
        task/network/Manager.cpp
        task/sensor/Reader.cpp

        timer/CounterTimeout.cpp
        timer/DelayTimeout.cpp

        uart/PicoOsUart.cpp

        view/SensorView.cpp
        view/TsSetup.cpp
        view/WifiSetup.cpp
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
        CHANNEL_API_KEY=\"$ENV{CHANNEL_API_KEY}\"
        TALKBACK_ID=\"$ENV{TALKBACK_ID}\"
        TALKBACK_API_KEY=\"$ENV{TALKBACK_API_KEY}\"
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
