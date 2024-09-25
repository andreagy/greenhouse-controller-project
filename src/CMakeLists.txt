add_executable(${PROJECT_NAME}
        main.cpp
        critical_section.cpp

        # Fmutex.cpp
        # blinker.cpp
        ipstack/IPStack.cpp
        ipstack/tls_common.c
        ipstack/picow_tls_client.c

        i2c/PicoI2C.cpp

        uart/PicoOsUart.cpp

        modbus/nanomodbus.c
        modbus/ModbusRegister.cpp
        modbus/ModbusClient.cpp

        display/framebuf.cpp
        display/mono_vlsb.cpp

        # display/ssd1306.cpp
        display/ssd1306os.cpp

        task/BaseTask.cpp
        task/EncoderTask.cpp

        sensor/GMP252.cpp
        sensor/HMP60.cpp
        sensor/SDP600.cpp

        fan/MIO12V.cpp

        # storage/Eeprom.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        modbus
        uart
        i2c
        display
        ipstack
)

target_compile_definitions(${PROJECT_NAME} PRIVATE

        # WIFI_SSID=\"${WIFI_SSID}\"
        # WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        WIFI_SSID=\"SmartIotMQTT\"
        WIFI_PASSWORD=\"SmartIot\"
        NO_SYS=0 # don't want NO_SYS (generally this would be in your lwipopts.h)
        PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE=CYW43_COUNTRY_FINLAND
)

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
