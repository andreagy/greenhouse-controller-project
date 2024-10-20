#include "Manager.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "network/NetHeader.hpp"
#include "network/TlsClient.hpp"
#include "projdefs.h"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <mbedtls/debug.h>
#include <pico/cyw43_arch.h>
#include <pico/error.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

namespace Task
{
namespace Network
{

Manager::Manager(std::shared_ptr<Storage::Eeprom> eeprom,
                 QueueHandle_t dataQueue,
                 QueueHandle_t fanQueue,
                 QueueHandle_t targetQueue,
                 QueueHandle_t settingsQueue) :
    BaseTask{"NetworkClient", 1024, this, MED},
    m_Eeprom{eeprom},
    m_ReportTimeout{25000},
    m_DataQueue{dataQueue},
    m_FanQueue{fanQueue},
    m_TargetQueue{targetQueue},
    m_SettingsQueue{settingsQueue}
{
    update();
}

void Manager::run()
{
    getEeprom();

    connect(m_Ssid, m_Password);

    if (!m_Connected)
    {
        // Retry with settings from ENV
        if (connect(WIFI_SSID, WIFI_PASSWORD) == 0) { m_Connected = true; }
        else { printf("Check WiFi settings.\n"); }
    }

    if (m_Connected)
    {
        m_TlsClient = std::make_shared<::Network::Client>(30, m_TargetQueue);
        testTls();
    }

    while (true)
    {
        if (xQueueReceive(m_SettingsQueue, &m_NetSettings, 0) == pdTRUE)
        {
            if (strcmp(m_NetSettings.id, "") == 0)
            {
                if (reconnect(m_NetSettings.str1, m_NetSettings.str2) == 0)
                {
                    m_TlsClient = std::make_shared<::Network::Client>(30, m_TargetQueue);
                    testTls();
                }
            }
            else
            {
                m_ApiKey = m_NetSettings.str1;
                m_TalkbackKey = m_NetSettings.str2;
                m_TalkbackId = m_NetSettings.id;
                testTls();
            }
        }

        if (m_Connected)
        {
            getTalkback();
            vTaskDelay(pdMS_TO_TICKS(5000));
            update();
            sendReport();
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        else { vTaskDelay(pdMS_TO_TICKS(50)); }
    }
}

int Manager::connect(std::string ssid, std::string password)
{
    int err = cyw43_arch_init();

    if (err == 0)
    {
        cyw43_arch_enable_sta_mode();

        err = cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(),
                                                 password.c_str(),
                                                 CYW43_AUTH_WPA2_AES_PSK,
                                                 30000);

        if (err != 0)
        {
            printf("failed to connect - error %d\n", err);
            cyw43_arch_deinit();
        }
    }
    else { printf("failed to initialise - error %d\n", err); }

    if (err == 0)
    {
        m_Ssid = ssid;
        m_Password = password;
        m_Eeprom->write(Storage::SSID_ADDR, m_Ssid);
        m_Eeprom->write(Storage::PASSWORD_ADDR, m_Password);

        m_Connected = true;
    }

    return err;
}

int Manager::reconnect(std::string ssid, std::string password)
{
    if (m_Connected)
    {
        m_TlsClient.reset();
        cyw43_arch_deinit();
        m_Connected = false;
    }

    return connect(ssid, password);
}

void Manager::testTls()
{
    int result = 0;
    result = sendReport();
    if (result == 200) { m_Eeprom->write(Storage::API_KEY_ADDR, m_ApiKey); }
    else { printf("Invalid API key.\n"); }

    result = 0;
    result = getTalkback();
    if (result == 200)
    {
        m_Eeprom->write(Storage::TALKBACK_ID_ADDR, m_TalkbackId);
        m_Eeprom->write(Storage::TALKBACK_KEY_ADDR, m_TalkbackKey);
    }
    else { printf("Invalid Talkback ID or API key.\n"); }
}

void Manager::getEeprom()
{
    if (!m_Eeprom->read(Storage::SSID_ADDR, m_Ssid) || m_Ssid.empty())
    {
        m_Ssid = WIFI_SSID;
    }
    if (!m_Eeprom->read(Storage::PASSWORD_ADDR, m_Password) || m_Password.empty())
    {
        m_Password = WIFI_PASSWORD;
    }
    if (!m_Eeprom->read(Storage::API_KEY_ADDR, m_ApiKey) || m_ApiKey.empty())
    {
        m_ApiKey = CHANNEL_API_KEY;
    }
    if (!m_Eeprom->read(Storage::TALKBACK_ID_ADDR, m_TalkbackId)
        || m_TalkbackId.empty())
    {
        m_TalkbackKey = TALKBACK_ID;
    }
    if (!m_Eeprom->read(Storage::TALKBACK_KEY_ADDR, m_TalkbackKey)
        || m_TalkbackKey.empty())
    {
        m_TalkbackKey = TALKBACK_API_KEY;
    }
}

int Manager::sendReport()
{
    int result = 0;

    if (m_ReportTimeout())
    {
        result = m_TlsClient->send(createRequest(m_NetworkData));
        m_ReportTimeout.reset();
    }

    return result;
}

int Manager::getTalkback() { return m_TlsClient->send(createRequest()); }

std::string Manager::createRequest(const ::Network::Data &data)
{
    std::string request = "POST /update HTTP/1.1\r\n" + ::Network::Request::headerPrefix;
    std::string content = "api_key=" + m_ApiKey
                          + "&field1=" + std::to_string(data.co2)
                          + "&field2=" + std::to_string(data.rh)
                          + "&field3=" + std::to_string(data.temp)
                          + "&field4=" + std::to_string(data.speed / 10)
                          + "&field5=" + std::to_string(data.target);
    request += std::to_string(content.size()) + ::Network::Request::headerSuffix + content;

    return request;
}

std::string Manager::createRequest()
{
    std::string request = "POST /talkbacks/" + m_TalkbackId + "/commands/execute HTTP/1.1\r\n"
                          + ::Network::Request::headerPrefix;
    std::string content = "api_key=" + m_TalkbackKey;
    request += std::to_string(content.size()) + ::Network::Request::headerSuffix + content;

    return request;
}

void Manager::update()
{
    xQueuePeek(m_DataQueue, &m_SensorData, 0);
    xQueuePeek(m_TargetQueue, &m_NetworkData.target, 0);
    xQueuePeek(m_FanQueue, &m_NetworkData.speed, 0);

    m_NetworkData.co2 = m_SensorData.co2;
    m_NetworkData.rh = m_SensorData.rh;
    m_NetworkData.temp = m_SensorData.temp;
}

} // namespace Network

} // namespace Task