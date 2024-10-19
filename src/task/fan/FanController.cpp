#include "FanController.hpp"

#include "portmacro.h"
#include "timer/CounterTimeout.hpp"
#include "timers.h"

#include <climits>
#include <cstdint>

namespace Task
{

namespace Fan
{

constexpr int MODBUS_ADDR = 1;
constexpr int SPEED_REG_ADDR = 0x0000;
constexpr int ROT_REG_ADDR = 0x0004;
constexpr uint16_t FAN_MIN = 150;
constexpr uint16_t FAN_MAX = 1000;

Controller::Controller(std::shared_ptr<Modbus::Client> modbus, QueueHandle_t dataQueue) :
    BaseTask("FanController", 256, this, HIGH),
    m_SpeedRegister{modbus, MODBUS_ADDR, SPEED_REG_ADDR},
    m_PulseRegister{modbus, MODBUS_ADDR, ROT_REG_ADDR, false},
    m_DataQueue{dataQueue}
{}

void Controller::setSpeed(uint16_t speed)
{
    if (speed < FAN_MIN) { speed = 0; }
    if (speed > FAN_MAX) { speed = FAN_MAX; }

    m_SpeedRegister.write(speed);
    m_Speed = m_SpeedRegister.read();
}

void Controller::run()
{
    constexpr uint16_t CRITICAL_FAN_MAX = FAN_MAX - FAN_MIN;
    constexpr float CO2_CRITICAL = 2000;
    constexpr float CO2_CRITICAL_DIFF = 1000; // CRITICAL + DIFF => fan at max speed

    uint16_t speed = 0;
    Timer::CounterTimeout adjustInterval(1000); // Minimum time between fan speed adjustments

    setSpeed(0);

    while (true)
    {
        xQueuePeek(m_DataQueue, &m_SensorData, portMAX_DELAY);

        if (m_SensorData.co2 >= CO2_CRITICAL && adjustInterval())
        {
            speed = (CRITICAL_FAN_MAX
                     * ((m_SensorData.co2 - CO2_CRITICAL) / CO2_CRITICAL_DIFF))
                    + FAN_MIN; // Calculate fan speed based on how far above critical level we are (from 15% at 2000ppm to 100% at 3000ppm)
            setSpeed(speed);
            adjustInterval.reset();
        }
        else if (m_SensorData.co2 < CO2_CRITICAL && m_Speed != 0)
        {
            setSpeed(0);
        }

        vTaskDelay(250);
    }
}

} // namespace Fan

} // namespace Task