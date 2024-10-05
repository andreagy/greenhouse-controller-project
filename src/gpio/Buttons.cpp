#include "Buttons.hpp"

#include <hardware/gpio.h>
#include <hardware/irq.h>

namespace GPIO
{
QueueHandle_t ButtonHandler::buttonQueue;

ButtonHandler::ButtonHandler(QueueHandle_t queue,
                             uint32_t stackDepth,
                             Task::priority taskPriority) :
    Task::BaseTask("ButtonHandler", stackDepth, this, taskPriority)
{
    buttonQueue = queue;

    gpio_set_dir(SW0_PIN, GPIO_IN);
    gpio_pull_up(SW0_PIN);
    gpio_set_irq_enabled_with_callback(SW0_PIN, GPIO_IRQ_EDGE_FALL, true, callback);

    gpio_set_dir(SW1_PIN, GPIO_IN);
    gpio_pull_up(SW1_PIN);
    gpio_set_irq_enabled_with_callback(SW1_PIN, GPIO_IRQ_EDGE_FALL, true, callback);

    gpio_set_dir(SW2_PIN, GPIO_IN);
    gpio_pull_up(SW2_PIN);
    gpio_set_irq_enabled_with_callback(SW2_PIN, GPIO_IRQ_EDGE_FALL, true, callback);
}

void ButtonHandler::run()
{
    while (true) {
        vTaskDelay(portMAX_DELAY);
    }
}

void ButtonHandler::callback(uint gpio, uint32_t event_mask)
{
    buttonPin command = SW0_PIN; // Default to SW0_PIN
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio == SW0_PIN) {
        command = SW0_PIN;
    } else if (gpio == SW1_PIN) {
        command = SW1_PIN;
    } else if (gpio == SW2_PIN) {
        command = SW2_PIN;
    }

    xQueueSendToBackFromISR(ButtonHandler::buttonQueue, &command, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

} // namespace GPIO
