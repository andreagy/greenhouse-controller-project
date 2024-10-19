#include "Co2Controller.hpp"

#include "projdefs.h"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"
#include "timer/CounterTimeout.hpp"
#include <hardware/gpio.h>

#include <climits>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace Task
{

namespace Co2
{

enum taskState
{
    NORMAL,
    VALVE_OPEN,
    CRITICAL
};

Controller::Controller(std::shared_ptr<Sensor::GMP252> co2Sensor,
                       TaskHandle_t fanController,
                       QueueHandle_t targetQueue,
                       std::shared_ptr<Storage::Eeprom> eeprom) :
    BaseTask{"CO2Controller", 256, this, HIGH},
    m_Co2Sensor{co2Sensor},
    m_FanControlHandle{fanController},
    m_TargetQueue{targetQueue},
    m_Eeprom{eeprom}
{
    gpio_init(m_ValvePin);
    gpio_set_dir(m_ValvePin, GPIO_OUT);
    gpio_put(m_ValvePin, 0);
}

void Controller::setTarget(uint32_t target)
{
    if (target > m_Co2Max || target < m_Co2Min)
    {
        target = target > m_Co2Max ? m_Co2Max : m_Co2Min;
    }

    m_Co2Target = target;
    // std::vector<uint8_t> co2Target = {static_cast<uint8_t>(m_Co2Target >> 8),
    //                                   static_cast<uint8_t>(m_Co2Target)};

    m_Eeprom->write(Storage::CO2_TARGET_ADDR,
                    std::vector<uint8_t>{static_cast<uint8_t>(m_Co2Target >> 8),
                                         static_cast<uint8_t>(m_Co2Target)});
}

float Controller::getTarget() { return m_Co2Target; }

void Controller::run()
{
    constexpr uint16_t FAN_MIN = 150;
    constexpr uint16_t FAN_MAX = 850; // Set to 1000 - FAN_MIN
    constexpr float CO2_CRITICAL = 2000;
    constexpr float CO2_CRITICAL_DIFF = 1000; // CRITICAL + DIFF => fan at max speed

    taskState state = NORMAL;
    uint32_t fanSpeed = 0;
    uint32_t receivedTarget = 0;
    Timer::CounterTimeout valveTimeout(1500); // Maximum time the valve stays open
    Timer::CounterTimeout retryTimeout(45000); // Minimum time to wait between opening the valve
    Timer::CounterTimeout fanSpeedTimeout(1000); // Minimum time between fan speed adjusts

    std::vector<uint8_t> buffer;

    if (m_Eeprom->read(Storage::CO2_TARGET_ADDR, buffer))
    {
        m_Co2Target = buffer[0] << 8 | buffer[1];
    }
    xQueueOverwrite(m_TargetQueue, &m_Co2Target);

    while (true)
    {
        // Check for a new target
        if (xQueuePeek(m_TargetQueue, &receivedTarget, 0) == pdPASS)
        {
            setTarget(receivedTarget);
        }

        switch (state)
        {
            case NORMAL:
                if (m_Co2Sensor->getCo2() > CO2_CRITICAL) { state = CRITICAL; }
                if (m_Co2Sensor->getCo2() < (m_Co2Target - 50)
                    && retryTimeout()) // TODO: fine tune with real system
                {
                    gpio_put(m_ValvePin, 1);
                    state = VALVE_OPEN;
                    valveTimeout.reset();
                }
                break;
            case VALVE_OPEN:
                if (m_Co2Sensor->getCo2() >= m_Co2Target || valveTimeout())
                {
                    gpio_put(m_ValvePin, 0);
                    state = NORMAL;
                    retryTimeout.reset();
                }
                break;
            case CRITICAL:
                if (fanSpeedTimeout())
                {
                    fanSpeed = (FAN_MAX * ((m_Co2Sensor->getCo2() - CO2_CRITICAL) / CO2_CRITICAL_DIFF))
                               + FAN_MIN; // Calculate fan speed based on CO2 value (15% - 100% at 2000 - 3000)
                    xTaskNotify(m_FanControlHandle, fanSpeed, eSetValueWithOverwrite);
                    fanSpeedTimeout.reset();
                }

                if (m_Co2Sensor->getCo2() < CO2_CRITICAL)
                {
                    xTaskNotify(m_FanControlHandle, 0, eSetValueWithOverwrite);
                    state = NORMAL;
                }
                break;
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

} // namespace Co2

} // namespace Task