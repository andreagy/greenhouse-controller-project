#include "RotaryEncoder.hpp"

#include <hardware/gpio.h>
#include <hardware/irq.h>

#include <cstdio>

namespace GPIO
{
QueueHandle_t RotaryEncoder::queueHandle;

RotaryEncoder::RotaryEncoder(QueueHandle_t queue,
                             uint32_t stackDepth,
                             Task::priority taskPriority) :
    Task::BaseTask("RotaryEncoder", stackDepth, this, taskPriority)
{
    constexpr uint ROTARY_BIT_MASK = 7 << 10; // TODO: take as parameter?
    queueHandle = queue;

    gpio_set_dir_in_masked(ROTARY_BIT_MASK);
    gpio_pull_up(ROT_SW);
    gpio_set_irq_enabled_with_callback(ROT_A, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(ROT_SW, GPIO_IRQ_EDGE_RISE, true, callback);
}

void RotaryEncoder::run()
{
    while (true) { vTaskDelay(portMAX_DELAY); }
}

void RotaryEncoder::callback(uint gpio, uint32_t event_mask)
{
    encoderPin command = ROT_SW;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio == ROT_A)
    {
        command = ROT_A;
        if (gpio_get(ROT_B)) { command = ROT_B; }
    }

    xQueueSendToBackFromISR(RotaryEncoder::queueHandle, &command, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

} // namespace GPIO
