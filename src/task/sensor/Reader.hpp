#ifndef READER_HPP
#define READER_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "i2c/PicoI2C.hpp"
#include "modbus/MbClient.hpp"
#include "queue.h"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "sensor/SensorData.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Task
{

namespace Sensor
{

class Reader : public Task::BaseTask
{
  public:
    Reader(std::shared_ptr<Modbus::Client> modbus,
           std::shared_ptr<I2c::PicoI2C> i2c,
           QueueHandle_t dataQueue);
    void run() override;

  private:
    ::Sensor::SensorData m_SensorData;
    QueueHandle_t m_DataQueue;
    ::Sensor::GMP252 m_Co2Sensor;
    ::Sensor::HMP60 m_RhSensor;
    ::Sensor::SDP600 m_PaSensor;
};

} // namespace Sensor

} // namespace Task

#endif /* READER_HPP */
