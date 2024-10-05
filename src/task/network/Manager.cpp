#include "Manager.hpp"

#include "network/TlsClient.hpp"
#include "task/BaseTask.hpp"
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <mbedtls/debug.h>
#include <pico/cyw43_arch.h>
#include <pico/error.h>

#include <cstdio>

// Testing purposes
#define TLS_CLIENT_SERVER "api.thingspeak.com"
#define TLS_CLIENT_HTTP_REQUEST                                                \
    "POST /update.json HTTP/1.1\r\n"                                           \
    "Host: api.thingspeak.com\r\n"                                             \
    "User-Agent: PicoW\r\n"                                                    \
    "Accept: */*\r\n"                                                          \
    "Content-Length: 35\r\n"                                                   \
    "Content-Type: application/x-www-form-urlencoded\r\n"                      \
    "\r\n"                                                                     \
    "field1=123&api_key="

namespace Task
{
namespace Network
{

Manager::Manager() : BaseTask{"NetworkClient", 1024, this, MED} {}

void Manager::run()
{
    // TODO: get wifi settings from eeprom or environment
    if (m_Ssid.empty()) { m_Ssid = WIFI_SSID; }
    if (m_Password.empty()) { m_Password = WIFI_PASSWORD; }

    int err = connect();
    ::Network::Client tlsClient(30);

    if (err == 0) { m_Connected = true; }

    while (true)
    {
        // TODO: handle sending thingspeak
        if (m_Connected) {}

        if (!gpio_get(9))
        {
            while (!gpio_get(9))
            {
                vTaskDelay(1);
                /* Loop while button is pressed */
            }

            // Send thingspeak message
            if (m_Connected)
            {
                if (tlsClient.open(TLS_CLIENT_SERVER, TLS_CLIENT_HTTP_REQUEST))
                {
                    printf("OK\n");
                }
            }
        }
    }
}

int Manager::connect()
{
    int err = cyw43_arch_init();

    if (err == 0)
    {
        cyw43_arch_enable_sta_mode();

        err = cyw43_arch_wifi_connect_timeout_ms(m_Ssid.c_str(),
                                                 m_Password.c_str(),
                                                 CYW43_AUTH_WPA2_AES_PSK,
                                                 30000);

        if (err != 0)
        {
            printf("failed to connect - error %d\n", err);
            cyw43_arch_deinit();
        }
    }
    else { printf("failed to initialise - error %d\n", err); }

    return err;
}

} // namespace Network

} // namespace Task