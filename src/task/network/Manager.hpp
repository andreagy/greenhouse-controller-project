#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "network/NetData.hpp"
#include "network/TlsClient.hpp"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"
#include "task/co2/Co2Controller.hpp"
#include "task/fan/FanController.hpp"
#include "timer/CounterTimeout.hpp"

#include <memory>

namespace Task
{

namespace Network
{

class Manager : public BaseTask
{
  public:
    Manager(std::shared_ptr<Sensor::GMP252> co2Sensor,
            std::shared_ptr<Sensor::HMP60> rhSensor,
            std::shared_ptr<Task::Co2::Controller> co2Controller,
            std::shared_ptr<Task::Fan::Controller> fanController,
            std::shared_ptr<Storage::Eeprom> eeprom,
            QueueHandle_t targetQueue,
            QueueHandle_t settingsQueue);
    void run() override;

  private:
    std::string m_Ssid;
    std::string m_Password;
    std::string m_ApiKey;
    std::string m_TalkbackKey;
    ::Network::Data m_Data;
    std::shared_ptr<::Network::Client> m_TlsClient;
    std::shared_ptr<Sensor::GMP252> m_Co2Sensor;
    std::shared_ptr<Sensor::HMP60> m_RhSensor;
    std::shared_ptr<Task::Co2::Controller> m_Co2Controller;
    std::shared_ptr<Task::Fan::Controller> m_FanController;
    std::shared_ptr<Storage::Eeprom> m_Eeprom;
    Timer::CounterTimeout m_ReportTimeout;
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
