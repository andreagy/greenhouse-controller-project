#include "Manager.hpp"

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

Manager::Manager(std::shared_ptr<Sensor::GMP252> co2Sensor,
                 std::shared_ptr<Sensor::HMP60> rhSensor,
                 std::shared_ptr<Task::Co2::Controller> co2Controller,
                 std::shared_ptr<Task::Fan::Controller> fanController,
                 std::shared_ptr<Storage::Eeprom> eeprom,
                 QueueHandle_t targetQueue,
                 QueueHandle_t settingsQueue) :
    BaseTask{"NetworkClient", 1024, this, MED},
    m_Co2Sensor{co2Sensor},
    m_RhSensor{rhSensor},
    m_Co2Controller{co2Controller},
    m_FanController{fanController},
    m_Eeprom{eeprom},
    m_ReportTimeout{15000},
    m_TargetQueue{targetQueue},
    m_SettingsQueue{settingsQueue}
{
    update();
}

void Manager::run()
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
    if (!m_Eeprom->read(Storage::TALKBACK_KEY_ADDR, m_TalkbackKey)
        || m_TalkbackKey.empty())
    {
        m_TalkbackKey = TALKBACK_API_KEY;
    }

    int err = connect();

    if (err == 0) { m_Connected = true; }
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
        m_TlsClient->send(createRequest(m_Data));
        m_ReportTimeout.reset();
    }
}

void Manager::getTalkback() { m_TlsClient->send(createRequest()); }

std::string Manager::createRequest(const ::Network::Data &data)
{
    std::string request = ::Network::Request::updateHeader;
    std::string content = "api_key=" + m_ApiKey
                          + "&field1=" + std::to_string(data.co2)
                          + "&field2=" + std::to_string(data.rh)
                          + "&field3=" + std::to_string(data.temp)
                          + "&field4=" + std::to_string(data.speed)
                          + "&field5=" + std::to_string(data.target);
    request += std::to_string(content.size()) + ::Network::Request::headerEnd + content;

    return request;
}

std::string Manager::createRequest()
{
    std::string request = ::Network::Request::executeHeader;
    std::string content = "api_key=" + m_TalkbackKey;
    request += std::to_string(content.size()) + ::Network::Request::headerEnd + content;

    return request;
}

void Manager::update()
{
    m_Data.co2 = m_Co2Sensor->getCo2();
    m_Data.rh = m_RhSensor->getRh();
    m_Data.temp = m_RhSensor->getTemp();
    m_Data.target = m_Co2Controller->getTarget();
    m_Data.speed = m_FanController->getSpeed();
}

} // namespace Network

} // namespace Task