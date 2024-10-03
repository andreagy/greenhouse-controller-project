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
    "POST /talkbacks/52920/commands/execute.json HTTP/1.1\r\n"                 \
    "Host: api.thingspeak.com\r\n"                                             \
    "User-Agent: PicoW\r\n"                                                    \
    "Accept: */*\r\n"                                                          \
    "Content-Length: 24\r\n"                                                   \
    "Content-Type: application/x-www-form-urlencoded\r\n"                      \
    "\r\n"                                                                     \
    "api_key=GK2UGCNHFLC4ZVWN\r\n"                                             \
    "field1=123"

namespace Task
{
namespace Network
{

Manager::Manager() : BaseTask{"NetworkClient", 256, this, MED}
{
    // TODO: get wifi settings from eeprom
    // If no valid wifi settings, get from environment
    if (m_Ssid.empty()) { m_Ssid = WIFI_SSID; }
    if (m_Password.empty()) { m_Password = WIFI_PASSWORD; }

    gpio_init(9);
    gpio_set_dir(9, GPIO_IN);
    gpio_pull_up(9);
}

void Manager::run()
{
    int err = connect();
    ::Network::Client tlsClient(30);

    if (err == 0) { m_Connected = true; }

    while (true)
    {
        // TODO: handle sending thingspeak

        if (!gpio_get(9))
        {
            while (!gpio_get(9))
            {
                // busy_wait_at_least_cycles(1);
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