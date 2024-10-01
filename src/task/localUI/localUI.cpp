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
       std::shared_ptr<I2c::PicoI2C> i2cBus,
       std::shared_ptr<Modbus::Client> modbusClient,
       TaskHandle_t co2ControllerHandle)
    : BaseTask{"LocalUI", 256, this, LOW},
      m_RotaryEncoder{rotaryQueue, 256, LOW},
      rotaryQueue{rotaryQueue},
      m_Co2Target{900},
      display{i2cBus}, // Initialize the OLED display with I2C bus
      tempRHSensor{modbusClient}, // Initialize HMP60 temp sensor with Modbus client
      pressureSensor{i2cBus},
      co2Sensor{modbusClient},
      co2ControllerHandle{co2ControllerHandle}
{
    readFromEEPROM(); // TODO: implement reading initial settings from EEPROM
    display.fill(0); // Clear the display
    display.show(); // Show the cleared display
}

void UI::run()
{
    while (true)
    {
        updateDisplay(); // Update the OLED with current values
        handleInput();   // Check for user input

        vTaskDelay(pdMS_TO_TICKS(500)); // Update every 500ms
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
    display.text("Humidity: " + std::to_string(humidity) + " %", 0, 10);
    display.text("Temp: " + std::to_string(temperature) + " C", 0, 20);
    display.text("Pressure: " + std::to_string(pressure) + " Pa", 0, 30);
    display.show(); // Refresh the display with new content
}

void UI::handleInput()
{
    GPIO::encoderPin command;
    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (command == GPIO::ROT_A) {
            setCO2Level(co2Sensor.getCo2() + 10); // Increment CO2 level
        } else if (command == GPIO::ROT_B) {
            setCO2Level(co2Sensor.getCo2() - 10); // Decrement CO2 level
        } else if (command == GPIO::ROT_SW) {
            // TODO: handle button press (confirm setting)
            xTaskNotify(co2ControllerHandle, *(uint32_t*)&m_Co2Target, eSetValueWithOverwrite); // TODO: implement in Co2Controller
            saveToEEPROM(); // TODO: implement saving when the button is pressed
        }
    }
}

void UI::setCO2Level(float level)
{
    // Limit value to valid range
    if (level < 200) level = 200;
    if (level > 1500) level = 1500;

    m_Co2Target = level; // Set the target CO2 level
}

} // namespace LocalUI

} // namespace Task
