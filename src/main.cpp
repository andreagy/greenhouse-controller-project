#include "FreeRTOS.h" // IWYU pragma: keep
#include "gpio/Buttons.hpp"
#include "gpio/RotaryEncoder.hpp"
#include "i2c/PicoI2C.hpp"
#include "modbus/Client.hpp"
#include "semphr.h"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task.h"
#include "task/co2/Co2Controller.hpp"
#include "task/fan/FanController.hpp"
#include "task/localUI/LocalUI.hpp"
#include "uart/PicoOsUart.hpp"
#include <hardware/structs/timer.h>
#include <pico/stdio.h>

#include <memory>

extern "C"
{
    uint32_t read_runtime_ctr(void) { return timer_hw->timerawl; }

    // "Syscall stub" to silence linker warning from some versions of arm-none-eabi-gcc
    int getentropy(void *buffer, size_t length) { return -ENOSYS; }
}

int main()
{
    // Set system variables

    stdio_init_all();
    printf("\nBoot\n");

    // Create system objects
    auto picoI2c0 = std::make_shared<I2c::PicoI2C>(I2c::BUS_0);
    auto picoI2c1 = std::make_shared<I2c::PicoI2C>(I2c::BUS_1);
    auto uart = std::make_shared<Uart::PicoOsUart>(1, 4, 5, 9600); // TODO: Add enums for accepted values
    auto modbusClient = std::make_shared<Modbus::Client>(uart);

    // Create sensor objects
    auto co2Sensor = std::make_shared<Sensor::GMP252>(modbusClient);
    auto rhSensor = std::make_shared<Sensor::HMP60>(modbusClient);
    auto paSensor = std::make_shared<Sensor::SDP600>(picoI2c1);

    // Create queue for rotary encoder and buttons
    QueueHandle_t rotaryQueue = xQueueCreate(5, sizeof(GPIO::encoderPin));
    QueueHandle_t buttonQueue = xQueueCreate(5, sizeof(GPIO::buttonPin));

    // Create task objects
    auto rotary = new GPIO::RotaryEncoder(rotaryQueue);
    auto button = new GPIO::ButtonHandler(buttonQueue);
    auto fanController = new Task::Fan::Controller(modbusClient);
    auto co2Controller = std::make_shared<Task::Co2::Controller>(co2Sensor, fanController->getHandle());
    auto localUI = new Task::LocalUI::UI(rotaryQueue, buttonQueue, modbusClient, co2Controller, picoI2c1, co2Sensor, rhSensor, paSensor);

    // Start scheduler
    vTaskStartScheduler();

    while (true) {};

    // Delete task objects
    delete rotary;
    delete button;
    delete fanController;
    delete localUI;

    return 0;
}
