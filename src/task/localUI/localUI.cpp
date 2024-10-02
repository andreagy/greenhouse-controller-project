#include "LocalUI.hpp"
#include "projdefs.h"
#include "hardware/gpio.h"
#include "timer/Timeout.hpp"
#include "timers.h"
#include <iostream> // For debugging output
#include <string>   // For displaying text
#include "gpio/RotaryEncoder.hpp"
#include <iomanip>
#include <sstream>

bool Task::LocalUI::UI::updateDisplayFlag = false;
namespace Task
{

namespace LocalUI
{

UI::UI(QueueHandle_t rotaryQueue,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       const std::shared_ptr<Task::Co2::Controller>& co2Controller,
       const std::shared_ptr<I2c::PicoI2C>& i2c,
       const std::shared_ptr<Sensor::GMP252>& co2Sensor,
       const std::shared_ptr<Sensor::HMP60>& tempRhSensor,
       const std::shared_ptr<Sensor::SDP600>& paSensor)
    : BaseTask{"LocalUI", 256, this, LOW},
      m_Co2Target{900},
      rotaryQueue{rotaryQueue},
      i2cBus{i2c},
      co2Controller{co2Controller},
      co2Sensor{co2Sensor},
      tempRhSensor{tempRhSensor},
      paSensor{paSensor}
{
    //readFromEEPROM(); // TODO: implement reading initial settings from EEPROM
    UI::updateDisplayFlag = false;
}

void UI::initializeDisplay() {
    display->fill(0);
    display->text("Boot", 0, 0);
    display->show();
}

void UI::run()
{
    display = std::make_shared<ssd1306os>(i2cBus);
    initializeDisplay();

    // Create a periodic display refresh timer (runs every 50ms)
    TimerHandle_t displayRefreshTimer = xTimerCreate("DisplayRefreshTimer",
                                                     pdMS_TO_TICKS(50),
                                                     pdTRUE,  // Auto-reload
                                                     this,    // Pass the UI object as timer ID
                                                     displayRefreshCallback);

    if (displayRefreshTimer != NULL) {
        xTimerStart(displayRefreshTimer, 0);
    }

    while (true)
    {
        if (UI::updateDisplayFlag) {
            updateDisplay();
            UI::updateDisplayFlag = false;
        }
        handleInput();
    }
}

void UI::displayRefreshCallback(TimerHandle_t xTimer)
{
    UI::updateDisplayFlag = true;
}


void UI::updateDisplay()
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

void UI::handleInput()
{
    GPIO::encoderPin command;

    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (command == GPIO::ROT_A) {
            m_Co2Target += 10; // Increment CO2 level
            //updateDisplay(display);

        } else if (command == GPIO::ROT_B) {
            m_Co2Target -= 10; // Decrement CO2 level
            //updateDisplay(display);

        } else if (command == GPIO::ROT_SW) {
            xTaskNotify(co2Controller->getHandle(), *(uint32_t*)&m_Co2Target, eSetValueWithOverwrite);
            //saveToEEPROM(); // TODO: implement saving when the button is pressed
        }
    }
}

/*void readFromEEPROM() {
    // TODO: use EEPROM class
}

void saveToEEPROM() {
    // TODO: use EEPROM class
}*/

} // namespace LocalUI

} // namespace Task
