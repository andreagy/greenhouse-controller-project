#include "LocalUI.hpp"
#include "projdefs.h"
#include "hardware/gpio.h"
#include "timer/Timeout.hpp"
#include "timers.h"
#include <iostream>
#include <string>
#include "gpio/RotaryEncoder.hpp"
#include "gpio/Buttons.hpp"
#include <iomanip>
#include <sstream>
#include <vector>

bool Task::LocalUI::UI::updateDisplayFlag = false;
namespace Task
{

namespace LocalUI
{

MenuState currentState = MAIN_MENU;

UI::UI(QueueHandle_t rotaryQueue,
       QueueHandle_t buttonQueue,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       const std::shared_ptr<Task::Co2::Controller>& co2Controller,
       const std::shared_ptr<I2c::PicoI2C>& i2c,
       const std::shared_ptr<Sensor::GMP252>& co2Sensor,
       const std::shared_ptr<Sensor::HMP60>& tempRhSensor,
       const std::shared_ptr<Sensor::SDP600>& paSensor)
    : BaseTask{"LocalUI", 256, this, LOW},
      m_Co2Target{900},
      rotaryQueue{rotaryQueue},
      buttonQueue{buttonQueue},
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
    currentState = MAIN_MENU;
    display = std::make_shared<ssd1306os>(i2cBus);
    initializeDisplay();
    displayMenu();

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
            displayMenu();
            switch (currentState)
            {
                case MAIN_MENU:
                    displayMenu();
                    break;
                case SENSOR_VALUES:
                    displaySensorValues();
                    setCO2Target();
                    break;
                case WIFI_SETTINGS:
                    displayWiFiSettings();
                    break;
                case THINGSPEAK_SETTINGS:
                    displayThingSpeakSettings();
                    break;
            }
            UI::updateDisplayFlag = false;
        }
    }
}

void UI::displayRefreshCallback(TimerHandle_t xTimer)
{
    UI::updateDisplayFlag = true;
}

void UI::displayMenu()
{
    const std::vector<std::string> menuOptions = {
        "Sensor Values",
        "Set Network",
        "Set ThingSpeak"
    };

    static int selectedIndex = 0;

    display->fill(0);

    // Display each menu option
    for (int i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedIndex) {
            display->text("> " + menuOptions[i], 0, i * 10);
        } else {
            display->text("  " + menuOptions[i], 0, i * 10);
        }
    }
    display->text(" " + std::to_string(rand()%1000), 0, 30);
    display->show();

    GPIO::encoderPin command;
    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) {
        if (command == GPIO::ROT_A) {
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
        } else if (command == GPIO::ROT_B) {
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
        } else if (command == GPIO::ROT_SW) {
            if (selectedIndex == 0) {
                displaySensorValues();
            } else if (selectedIndex == 1) {
                displayWiFiSettings();
            } else if (selectedIndex == 2) {
                displayThingSpeakSettings();
            }
        }
    }

    // Handle Back button (SW0_PIN)
    GPIO::buttonPin buttonCommand;
    if (xQueueReceive(buttonQueue, &buttonCommand, 0) == pdPASS) {
        if (buttonCommand == GPIO::SW0_PIN) {
            // Exit back to main menu or previous state
            currentState = MAIN_MENU;
            return;  // Exit the current menu loop
        }
    }
}


void UI::displaySensorValues()
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

void UI::displayWiFiSettings()
{
    std::vector<char> characterSet = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                      'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                      '4', '5', '6', '7', '8', '9', '-', '_'};

    int charIndex = 0;
    const int maxSSIDSize = 10;
    const int maxPasswordSize = 10;
    std::string ssid, password;
    bool isSSIDInput = true; // Flag for SSID or password input

    while (true)
    {
        display->fill(0); // Clear the display

        if (isSSIDInput)
        {
            display->text("Enter SSID:", 0, 0);
            display->text(ssid, 0, 10);
        }
        else
        {
            display->text("Enter Password:", 0, 0);
            display->text(password, 0, 10);
        }

        // Display current character selection
        display->text("Char: " + std::string(1, characterSet[charIndex]), 0, 20);
        display->text("Press to select", 0, 40);
        display->show();

        GPIO::encoderPin command;
        if (xQueueReceive(rotaryQueue, &command, pdMS_TO_TICKS(100)) == pdPASS) {
            if (command == GPIO::ROT_A) {
                charIndex = (charIndex + 1) % characterSet.size();
            } else if (command == GPIO::ROT_B) {
                charIndex = (charIndex - 1 + characterSet.size()) % characterSet.size();
            } else if (command == GPIO::ROT_SW) {
                if (isSSIDInput && ssid.length() < maxSSIDSize) {
                    ssid += characterSet[charIndex];
                } else if (!isSSIDInput && password.length() < maxPasswordSize) {
                    password += characterSet[charIndex];
                }
            }
        }

        // Handle Delete button (SW1_PIN)
        GPIO::buttonPin buttonCommand;
        if (xQueueReceive(buttonQueue, &buttonCommand, pdMS_TO_TICKS(100)) == pdPASS) {
            if (buttonCommand == GPIO::SW1_PIN) {
                // Delete last character from SSID or Password
                if (isSSIDInput && !ssid.empty()) {
                    ssid.pop_back();
                } else if (!isSSIDInput && !password.empty()) {
                    password.pop_back();
                }
            }
        }

        // Switch from SSID to Password entry after max SSID length
        if (ssid.length() >= maxSSIDSize && isSSIDInput) {
            // TODO: handle sending SSID to network task
            isSSIDInput = false;
        }

        // Break out of the loop when the password is fully entered
        if (!isSSIDInput && password.length() >= maxPasswordSize) {
            // TODO: handle sending password to network task
            break;
        }
    }

    // After entering both SSID and password, display sensor values
    displaySensorValues();
}


void UI::displayThingSpeakSettings() {
    // TODO: implement
    display->fill(0);
    display->text("Work in progress", 0, 20);
    display->show();

    vTaskDelay(3000);

}

void UI::setCO2Target()
{
    GPIO::encoderPin command;

    if (xQueueReceive(rotaryQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (command == GPIO::ROT_A) {
            m_Co2Target += 10; // Increment CO2 level

        } else if (command == GPIO::ROT_B) {
            m_Co2Target -= 10; // Decrement CO2 level

        } else if (command == GPIO::ROT_SW) {
            xTaskNotify(co2Controller->getHandle(), *(uint32_t*)&m_Co2Target, eSetValueWithOverwrite);
            //saveToEEPROM(); // TODO: implement saving when the button is pressed
        }
    }


}

/*void UI::readFromEEPROM() {
    // TODO: use EEPROM class
}

void UI::saveToEEPROM() {
    // TODO: use EEPROM class
}*/

} // namespace LocalUI

} // namespace Task
