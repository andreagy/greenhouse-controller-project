#ifndef FANCONTROLLER_HPP
#define FANCONTROLLER_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "modbus/MbClient.hpp"
#include "modbus/Register.hpp"
#include "queue.h"
#include "sensor/SensorData.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Task
{

namespace Fan
{

class Controller : public BaseTask
{
  public:
    Controller(std::shared_ptr<Modbus::Client> modbus, QueueHandle_t dataQueue);
    Controller(const Controller &) = delete;
    void setSpeed(uint16_t speed);
    void run();

  private:
    uint16_t m_Speed = 0;
    Sensor::SensorData m_SensorData;
    Modbus::Register m_SpeedRegister;
    Modbus::Register m_PulseRegister;
    QueueHandle_t m_DataQueue;
};

} // namespace Fan

} // namespace Task

#endif /* FANCONTROLLER_HPP */
