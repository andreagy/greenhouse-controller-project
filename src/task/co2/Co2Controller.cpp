#include "Co2Controller.hpp"

#include "portmacro.h"
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
    VALVE_OPEN
};

// TODO: handle eeprom elsewhere?

Controller::Controller(QueueHandle_t targetQueue,
                       QueueHandle_t dataQueue,
                       std::shared_ptr<Storage::Eeprom> eeprom) :
    BaseTask{"CO2Controller", 256, this, HIGH},
    m_TargetQueue{targetQueue},
    m_DataQueue{dataQueue},
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

    m_Eeprom->write(Storage::CO2_TARGET_ADDR,
                    std::vector<uint8_t>{static_cast<uint8_t>(m_Co2Target >> 8),
                                         static_cast<uint8_t>(m_Co2Target)});
}

float Controller::getTarget() { return m_Co2Target; }

void Controller::run()
{
    taskState state = NORMAL;
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
        xQueuePeek(m_DataQueue, &m_SensorData, portMAX_DELAY);

        if (xQueuePeek(m_TargetQueue, &receivedTarget, 0) == pdPASS)
        {
            setTarget(receivedTarget);
        }

        switch (state)
        {
            case NORMAL:
                if (m_SensorData.co2 < (m_Co2Target - 50) && retryTimeout())
                {
                    gpio_put(m_ValvePin, 1);
                    state = VALVE_OPEN;
                    valveTimeout.reset();
                }
                break;
            case VALVE_OPEN:
                if (m_SensorData.co2 >= m_Co2Target || valveTimeout())
                {
                    gpio_put(m_ValvePin, 0);
                    state = NORMAL;
                    retryTimeout.reset();
                }
                break;
            default:
                // We should never get here
                state = NORMAL;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

} // namespace Co2

} // namespace Task