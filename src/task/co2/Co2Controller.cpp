#include "Co2Controller.hpp"

#include "projdefs.h"
#include "task/BaseTask.hpp"
#include "timer/Timeout.hpp"
#include <hardware/gpio.h>

#include <cstdint>

namespace Task
{

namespace Co2
{

enum taskState
{
    VALVE_OPEN,
    VALVE_CLOSED,
    CO2_CRITICAL
};

Controller::Controller(std::shared_ptr<Sensor::GMP252> co2Sensor,
                       TaskHandle_t fanController) :
    BaseTask{"CO2Controller", 256, this, HIGH},
    m_Co2Sensor{co2Sensor},
    m_FanControlHandle{fanController}
{
    gpio_init(m_ValvePin);
    gpio_set_dir(m_ValvePin, GPIO_OUT);
    gpio_put(m_ValvePin, 0);
}

void Controller::setTarget(float target)
{
    if (target > m_Co2Max || target < m_Co2Min)
    {
        target = target > m_Co2Max ? m_Co2Max : m_Co2Min;
    }

    m_Co2Target = target;
}

float Controller::getTarget() { return m_Co2Target; }

void Controller::run()
{
    constexpr uint16_t FAN_MIN = 150;
    constexpr uint16_t FAN_MAX = 850;
    constexpr float CRITICAL = 2000;
    constexpr float CRITICAL_CONV = 500;

    taskState state = VALVE_CLOSED;
    uint16_t pollInterval = 250;
    uint32_t fanSpeed = 0;

    setTarget(900);

    while (true)
    {
        pollSensor(pollInterval);

        switch (state)
        {
            case VALVE_CLOSED:
                if (m_Co2Sensor->getCo2() > CRITICAL) { state = CO2_CRITICAL; }
                if (m_Co2Sensor->getCo2() < (m_Co2Target - 25))
                {
                    gpio_put(m_ValvePin, 1);
                    pollInterval = 25;
                    state = VALVE_OPEN;
                }
                break;
            case VALVE_OPEN:
                if (m_Co2Sensor->getCo2() >= m_Co2Target)
                {
                    gpio_put(m_ValvePin, 0);
                    pollInterval = 250;
                    state = VALVE_CLOSED;
                }
                break;
            case CO2_CRITICAL:
                fanSpeed = (FAN_MAX * ((m_Co2Sensor->getCo2() - CRITICAL) / CRITICAL_CONV))
                           + FAN_MIN; // Calculate fan speed based on CO2 value (15% - 100% at 2000 - 2500)
                xTaskNotify(m_FanControlHandle, fanSpeed, eSetValueWithOverwrite);

                if (m_Co2Sensor->getCo2() < CRITICAL)
                {
                    xTaskNotify(m_FanControlHandle, 0, eSetValueWithOverwrite);
                    state = VALVE_CLOSED;
                }
                break;
            default:
                break;
        }
    }
}

void Controller::pollSensor(uint16_t interval)
{
    m_Co2Sensor->update();
    vTaskDelay(pdMS_TO_TICKS(interval));
}

} // namespace Co2

} // namespace Task