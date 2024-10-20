#include "Manager.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "network/NetHeader.hpp"
#include "network/TlsClient.hpp"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"
#include <cyw43_ll.h>
#include <hardware/gpio.h>
#include <mbedtls/debug.h>
#include <pico/cyw43_arch.h>
#include <pico/error.h>

#include <cstdio>
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
    // TODO: move to a function
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

    int err = connect();

    if (err == 0) { m_Connected = true; }
    else
    {
        m_Ssid = WIFI_SSID;
        m_Password = WIFI_PASSWORD;
        m_ApiKey = CHANNEL_API_KEY;
        m_TalkbackKey = TALKBACK_ID;
        m_TalkbackKey = TALKBACK_API_KEY;

        if (connect() == 0) { m_Connected = true; }
    }
    m_TlsClient = std::make_shared<::Network::Client>(30, m_TargetQueue);

    while (true)
    {
        if (m_Connected)
        {
            getTalkback();
            vTaskDelay(5000);
            update();
            sendReport();
            vTaskDelay(5000);
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

    if (err == 0)
    {
        m_Eeprom->write(Storage::SSID_ADDR, m_Ssid);
        m_Eeprom->write(Storage::PASSWORD_ADDR, m_Password);
        m_Eeprom->write(Storage::API_KEY_ADDR, m_ApiKey);
        m_Eeprom->write(Storage::TALKBACK_ID_ADDR, m_TalkbackId);
        m_Eeprom->write(Storage::TALKBACK_KEY_ADDR, m_TalkbackKey);
    }

    return err;
}

int Manager::reconnect(std::string ssid, std::string password)
{
    // TODO: Implement disconnecting and reconnecting while system is up
    return 0;
}

void Manager::sendReport()
{
    if (m_ReportTimeout())
    {
        m_TlsClient->send(createRequest(m_NetworkData));
        m_ReportTimeout.reset();
    }
}

void Manager::getTalkback() { m_TlsClient->send(createRequest()); }

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