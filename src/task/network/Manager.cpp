#include "Manager.hpp"

#include "network/TlsClient.hpp"
#include "task/BaseTask.hpp"
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <mbedtls/debug.h>
#include <pico/cyw43_arch.h>
#include <pico/error.h>

#include <cstdio>
#include <string>

// Testing purposes
const std::string TLS_CLIENT_HTTP_REQUEST =
    "POST /update HTTP/1.1\r\n"
    "Host: api.thingspeak.com\r\n"
    "User-Agent: PicoW\r\n"
    "Accept: */*\r\n"
    "Content-Length: 35\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "api_key=" CHANNEL_API_KEY "talkback_key=" TALKBACK_API_KEY "field1="
    + std::to_string(12)              /* co2 (ppm) */
    + "field2=" + std::to_string(12)  /* rh (%) */
    + "field3=" + std::to_string(12)  /* temp (C) */
    + "field4=" + std::to_string(12)  /* speed (%) */
    + "field5=" + std::to_string(12); /* target (ppm) */

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