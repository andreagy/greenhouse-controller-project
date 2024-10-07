#include "FreeRTOS.h" // IWYU pragma: keep
#include "gpio/GpioInput.hpp"
#include "i2c/PicoI2C.hpp"
#include "modbus/MbClient.hpp"
#include "queue.h"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task.h"
#include "task/co2/Co2Controller.hpp"
#include "task/fan/FanController.hpp"
#include "task/gpio/Input.hpp"
#include "task/localUI/LocalUI.hpp"
#include "task/network/Manager.hpp"
#include "task/sensor/Reader.hpp"
#include "uart/PicoOsUart.hpp"
#include <hardware/structs/timer.h>
#include <pico/stdio.h>

#include <cstdio>
#include <memory>

extern "C"
{
    uint32_t read_runtime_ctr(void) { return timer_hw->timerawl; }

    // "Syscall stub" to silence linker warning from some versions of arm-none-eabi-gcc
    int getentropy(void *buffer, size_t length) { return -ENOSYS; }
}

int main()
{
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

    // Create queue for GPIO inputs
    QueueHandle_t inputQueue = xQueueCreate(5, sizeof(Gpio::inputPin));

    // Create task objects
    auto gpioInput = new Task::Gpio::Input(inputQueue);
    auto sensorReader = new Task::Sensor::Reader();
    auto fanController = new Task::Fan::Controller(modbusClient);
    auto co2Controller = new Task::Co2::Controller(co2Sensor,
                                                   fanController->getHandle());
    auto localUI = new Task::LocalUI::UI(inputQueue,
                                         co2Controller->getHandle(),
                                         modbusClient,
                                         picoI2c1,
                                         co2Sensor,
                                         rhSensor,
                                         paSensor);
    auto netManager = new Task::Network::Manager();

    // Attach sensors to the reader
    sensorReader->attach(co2Sensor);
    sensorReader->attach(rhSensor);
    sensorReader->attach(paSensor);

    // Start scheduler
    vTaskStartScheduler();

    while (true) {};

    // Delete task objects
    delete gpioInput;
    delete sensorReader;
    delete fanController;
    delete localUI;
    delete co2Controller;
    delete netManager;

    return 0;
}
