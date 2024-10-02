#include "LocalUI.hpp"
#include "projdefs.h"
#include "hardware/gpio.h"
#include "timer/Timeout.hpp"
#include <iostream> // For debugging output
#include <string>   // For displaying text

namespace Task
{

namespace LocalUI
{

UI::UI(QueueHandle_t rotaryQueue,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       TaskHandle_t co2ControllerHandle)
    : BaseTask{"LocalUI", 256, this, LOW},
      m_RotaryEncoder{rotaryQueue, 256, LOW},
      rotaryQueue{rotaryQueue},
      m_Co2Target{900},
      i2cBus(std::make_shared<I2c::PicoI2C>(1, 400000)),
      display(i2cBus),
      tempRHSensor{modbusClient},
      pressureSensor{i2cBus},
      co2Sensor{modbusClient},
      co2ControllerHandle{co2ControllerHandle}
{
    //readFromEEPROM(); // TODO: implement reading initial settings from EEPROM
    initializeDisplay();
}

void UI::initializeDisplay() {
    display.fill(0);
    display.text("Boot", 0, 0);
    display.show();
}

void UI::run()
{
    while (true)
    {
        updateDisplay();
        handleInput();

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void UI::updateDisplay()
{
    // Retrieve current sensor values
    tempRHSensor.update();
    pressureSensor.update();
    co2Sensor.update();
    float co2Level = co2Sensor.getCo2();
    float humidity = tempRHSensor.getRh();
    float temperature = tempRHSensor.getTemp();
    float pressure = pressureSensor.getPressure();

    display.fill(0); // Clear the display
    display.text("CO2: " + std::to_string(co2Level) + " ppm", 0, 0);
    display.text("Target CO2: " + std::to_string(m_Co2Target) + " ppm", 0, 10);
    display.text("Humidity: " + std::to_string(humidity) + " %", 0, 20);
    display.text("Temp: " + std::to_string(temperature) + " C", 0, 30);
    display.text("Pressure: " + std::to_string(pressure) + " Pa", 0, 40);
    display.show();
}

void UI::handleInput()
{
    GPIO::encoderPin command;
    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (command == GPIO::ROT_A) {
            setCO2Level(co2Sensor.getCo2() + 10); // Increment CO2 level
            updateDisplay();

        } else if (command == GPIO::ROT_B) {
            setCO2Level(co2Sensor.getCo2() - 10); // Decrement CO2 level
            updateDisplay();

        } else if (command == GPIO::ROT_SW) {
            xTaskNotify(co2ControllerHandle, *(uint32_t*)&m_Co2Target, eSetValueWithOverwrite); // TODO: implement in Co2Controller
            //saveToEEPROM(); // TODO: implement saving when the button is pressed
        }
    }
}

void UI::setCO2Level(float level)
{
    // Limit value to valid range
    if (level < 200) level = 200;
    if (level > 1500) level = 1500;

    m_Co2Target = level;
}

/*void readFromEEPROM() {
    // TODO: use EEPROM class
}

void saveToEEPROM() {
    // TODO: use EEPROM class
}*/

} // namespace LocalUI

} // namespace Task
