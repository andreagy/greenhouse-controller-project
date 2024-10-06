#include "Input.hpp"

#include "gpio/GpioInput.hpp"
#include "projdefs.h"
#include <hardware/gpio.h>
#include <hardware/irq.h>

#include <cstdint>
#include <cstdio>

namespace Task
{

namespace Gpio
{

QueueHandle_t Input::s_Queue;
TickType_t Input::s_LastInput = 0;

Input::Input(QueueHandle_t queue) :
    Task::BaseTask("GpioInput", 256, this, Task::LOW)
{
    s_Queue = queue;

    constexpr uint INPUT_PIN_MASK = (1 << ::Gpio::SW0) + (1 << ::Gpio::SW1)
                                    + (1 << ::Gpio::SW2) + (1 << ::Gpio::ROT_SW)
                                    + (1 << ::Gpio::ROT_A) + (1 << ::Gpio::ROT_B);

    gpio_init_mask(INPUT_PIN_MASK);
    gpio_set_dir_in_masked(INPUT_PIN_MASK);
    gpio_pull_up(::Gpio::ROT_SW);
    gpio_pull_up(::Gpio::SW0);
    gpio_pull_up(::Gpio::SW1);
    gpio_pull_up(::Gpio::SW2);

    gpio_set_irq_enabled_with_callback(::Gpio::SW0, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(::Gpio::SW1, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(::Gpio::SW2, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(::Gpio::ROT_SW, GPIO_IRQ_EDGE_RISE, true, callback);
    gpio_set_irq_enabled_with_callback(::Gpio::ROT_A, GPIO_IRQ_EDGE_RISE, true, callback);
}

void Input::run()
{
    while (true) { vTaskDelay(portMAX_DELAY); }
}

void Input::callback(uint gpio, uint32_t event_mask)
{
    uint command = gpio;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (command == ::Gpio::ROT_A)
    {
        if (gpio_get(::Gpio::ROT_B)) { command = ::Gpio::ROT_B; }
    }

    if (xTaskGetTickCountFromISR() - s_LastInput >= pdMS_TO_TICKS(250))
    {
        xQueueSendToBackFromISR(Input::s_Queue, &command, &xHigherPriorityTaskWoken);
        s_LastInput = xTaskGetTickCountFromISR();
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

} // namespace Gpio

} // namespace Task