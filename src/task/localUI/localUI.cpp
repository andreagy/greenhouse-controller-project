#include "LocalUI.hpp"
#include "projdefs.h"
#include "hardware/gpio.h"
#include "timer/Timeout.hpp"
#include <iostream> // For debugging output
#include <string>   // For displaying text
#include "gpio/RotaryEncoder.hpp"
#include <iomanip>
#include <sstream>

namespace Task
{

namespace LocalUI
{

UI::UI(QueueHandle_t rotaryQueue,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       TaskHandle_t co2ControllerHandle,
       const std::shared_ptr<I2c::PicoI2C>& i2c,
       const std::shared_ptr<Sensor::GMP252>& co2Sensor,
       const std::shared_ptr<Sensor::HMP60>& tempRhSensor,
       const std::shared_ptr<Sensor::SDP600>& paSensor)
    : BaseTask{"LocalUI", 256, this, LOW},
      m_Co2Target{900},
      rotaryQueue{rotaryQueue},
      i2cBus(i2c),
      //display(i2c),
      co2ControllerHandle{co2ControllerHandle},
      co2Sensor{co2Sensor},
      tempRhSensor{tempRhSensor},
      paSensor{paSensor}
{
    //readFromEEPROM(); // TODO: implement reading initial settings from EEPROM
}

void UI::initializeDisplay(std::shared_ptr<ssd1306os> display) {
    display->fill(0);
    display->text("Boot", 0, 0);
    display->show();
}

void UI::run()
{
    auto display = std::make_shared<ssd1306os>(i2cBus);
    initializeDisplay(display);

    while (true)
    {
        updateDisplay(display);
        handleInput(display);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void UI::updateDisplay(std::shared_ptr<ssd1306os> display)
{
    // Retrieve current sensor values
    co2Sensor->update();
    tempRhSensor->update();
    paSensor->update();
    float co2Level = co2Sensor->getCo2();
    float humidity = tempRhSensor->getRh();
    float temperature = tempRhSensor->getTemp();
    float pressure = paSensor->getPressure();

    // Format values
    std::ostringstream co2Stream, tempStream, humidityStream, pressureStream;
    co2Stream << std::fixed << std::setprecision(2) << co2Level;
    tempStream << std::fixed << std::setprecision(1) << temperature;
    humidityStream << std::fixed << std::setprecision(1) << humidity;
    pressureStream << std::fixed << std::setprecision(1) << pressure;

    display->fill(0); // Clear the display
    display->text("Set CO2: " + std::to_string(m_Co2Target) + "ppm", 0, 0);
    display->text("CO2: " + co2Stream.str() + "ppm", 0, 20);
    display->text("Humidity: " + humidityStream.str() + "%", 0, 30);
    display->text("Temp: " + tempStream.str() + "C", 0, 40);
    display->text("Pressure: " + pressureStream.str() + "Pa", 0, 50);
    display->show();
}

void UI::handleInput(std::shared_ptr<ssd1306os> display)
{
    GPIO::encoderPin command;

    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (command == GPIO::ROT_A) {
            m_Co2Target += 10; // Increment CO2 level
            vTaskDelay(pdMS_TO_TICKS(500));
            updateDisplay(display);

        } else if (command == GPIO::ROT_B) {
            m_Co2Target -= 10; // Decrement CO2 level
            vTaskDelay(pdMS_TO_TICKS(500));
            updateDisplay(display);

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
