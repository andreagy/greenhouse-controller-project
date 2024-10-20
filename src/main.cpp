#include "FreeRTOS.h" // IWYU pragma: keep
#include "gpio/GpioInput.hpp"
#include "i2c/PicoI2C.hpp"
#include "modbus/MbClient.hpp"
#include "queue.h"
#include "sensor/SensorData.hpp"
#include "storage/Eeprom.hpp"
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

    // Create shared resources
    auto picoI2c0 = std::make_shared<I2c::PicoI2C>(I2c::BUS_0);
    auto picoI2c1 = std::make_shared<I2c::PicoI2C>(I2c::BUS_1);
    auto uart = std::make_shared<Uart::PicoOsUart>(1, 4, 5, 9600);
    auto modbusClient = std::make_shared<Modbus::Client>(uart);
    auto eeprom = std::make_shared<Storage::Eeprom>(picoI2c0);

    // Create queues
    QueueHandle_t inputQueue = xQueueCreate(3, sizeof(Gpio::inputPin));
    QueueHandle_t dataQueue = xQueueCreate(1, sizeof(Sensor::SensorData));
    QueueHandle_t targetQueue = xQueueCreate(1, sizeof(uint32_t));
    QueueHandle_t settingsQueue = xQueueCreate(1, sizeof(Network::Settings));

    // TODO: clean up task dependencies, use more queues for task-to-task communication

    // Create task objects
    auto gpioInput = new Task::Gpio::Input(inputQueue);
    auto sensorReader = new Task::Sensor::Reader(modbusClient, picoI2c1, dataQueue);
    auto fanController = new Task::Fan::Controller(modbusClient, dataQueue);
    auto co2Controller = new Task::Co2::Controller(eeprom, dataQueue, targetQueue);
    auto localUI = new Task::LocalUI::UI(picoI2c1, inputQueue, dataQueue, targetQueue, settingsQueue);
    auto netManager = new Task::Network::Manager(eeprom, dataQueue, targetQueue, settingsQueue);

    // Start scheduler
    vTaskStartScheduler();

    while (true) {};

    // Delete task objects
    delete gpioInput;
    delete sensorReader;
    delete fanController;
    delete co2Controller;
    delete localUI;
    delete netManager;

    return 0;
}
