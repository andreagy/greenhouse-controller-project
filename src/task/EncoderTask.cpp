#include "EncoderTask.hpp"

#include <hardware/gpio.h>
#include <hardware/irq.h>

#include <cstdio>

namespace Task
{

EncoderTask::EncoderTask(QueueHandle_t queue, uint32_t stackDepth, Task::priority taskPriority) :
    Task::BaseTask("EncoderTask", stackDepth, this, taskPriority)
{
    constexpr uint ROTARY_BIT_MASK = 7 << 10; // TODO: take as parameter?
    queueHandle = queue;

    gpio_set_dir_in_masked(ROTARY_BIT_MASK);
    gpio_pull_up(ROT_SW);
    gpio_set_irq_enabled_with_callback(ROT_A, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(ROT_SW, GPIO_IRQ_EDGE_RISE, true, callback);
}

void EncoderTask::run()
{
    while (true) { vTaskDelay(portMAX_DELAY); }
}

static void callback(uint gpio, uint32_t event_mask)
{
    Task::encoderPin command = Task::ROT_SW;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio == Task::ROT_A)
    {
        command = Task::ROT_A;
        if (gpio_get(Task::ROT_B)) { command = Task::ROT_B; }
    }

    xQueueSendToBackFromISR(EncoderTask::queueHandle, &command, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

} // namespace Task
