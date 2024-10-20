#include "Reader.hpp"

#include "task/BaseTask.hpp"
#include "timer/DelayTimeout.hpp"
#include "timers.h"

#include <memory>

namespace Task
{

namespace Sensor
{

Reader::Reader(std::shared_ptr<Modbus::Client> modbus,
               std::shared_ptr<I2c::PicoI2C> i2c,
               QueueHandle_t dataQueue) :
    Task::BaseTask{"SensorReader", 256, this, MED},
    m_DataQueue{dataQueue},
    m_Co2Sensor{modbus},
    m_RhSensor{modbus},
    m_PaSensor{i2c}
{}

void Reader::run()
{
    Timer::DelayTimeout updateTimeout(250);

    while (true)
    {
        m_Co2Sensor.update();
        m_SensorData.co2 = m_Co2Sensor.getCo2();
        m_RhSensor.update();
        m_SensorData.rh = m_RhSensor.getRh();
        m_SensorData.temp = m_RhSensor.getTemp();
        m_PaSensor.update();
        m_SensorData.pa = m_PaSensor.getPressure();

        xQueueOverwrite(m_DataQueue, &m_SensorData);

        updateTimeout();
    }
}

} // namespace Sensor

} // namespace Task