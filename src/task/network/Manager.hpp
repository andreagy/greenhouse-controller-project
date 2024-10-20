#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "network/NetData.hpp"
#include "network/TlsClient.hpp"
#include "sensor/SensorData.hpp"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"
#include "timer/CounterTimeout.hpp"

#include <memory>

namespace Task
{

namespace Network
{

class Manager : public BaseTask
{
  public:
    Manager(std::shared_ptr<Storage::Eeprom> eeprom,
            QueueHandle_t dataQueue,
            QueueHandle_t fanQueue,
            QueueHandle_t targetQueue,
            QueueHandle_t settingsQueue);
    void run() override;

  private:
    std::string m_Ssid;
    std::string m_Password;
    std::string m_ApiKey;
    std::string m_TalkbackId;
    std::string m_TalkbackKey;
    ::Network::Data m_NetworkData;
    Sensor::SensorData m_SensorData;
    std::shared_ptr<::Network::Client> m_TlsClient;
    std::shared_ptr<Storage::Eeprom> m_Eeprom;
    Timer::CounterTimeout m_ReportTimeout;
    QueueHandle_t m_DataQueue;
    QueueHandle_t m_FanQueue;
    QueueHandle_t m_TargetQueue;
    QueueHandle_t m_SettingsQueue;
    bool m_Connected = false;
    int connect();
    int reconnect(std::string ssid, std::string password);
    void sendReport();
    void getTalkback();
    std::string createRequest(const ::Network::Data &data);
    std::string createRequest();
    void update();
};

} // namespace Network

} // namespace Task

#endif /* MANAGER_HPP */
