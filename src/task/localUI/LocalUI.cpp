#include "LocalUI.hpp"

#include "MenuVariables.hpp"
#include "gpio/GpioInput.hpp"
#include "projdefs.h"
#include "timer/DelayTimeout.hpp"
#include "timers.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool Task::LocalUI::UI::updateDisplayFlag = false;

namespace Task
{

namespace LocalUI
{

// MenuState currentState = MAIN_MENU;

UI::UI(QueueHandle_t inputQueue,
       TaskHandle_t co2Controller,
       const std::shared_ptr<Modbus::Client> &modbusClient,
       const std::shared_ptr<I2c::PicoI2C> &i2c,
       const std::shared_ptr<Sensor::GMP252> &co2Sensor,
       const std::shared_ptr<Sensor::HMP60> &tempRhSensor,
       const std::shared_ptr<Sensor::SDP600> &paSensor) :
    BaseTask{"UserInterface", 512, this, MED},
    m_InputQueue{inputQueue},
    m_Co2Controller{co2Controller},
    i2cBus{i2c},
    m_Co2Sensor{co2Sensor},
    m_RhSensor{tempRhSensor},
    m_PaSensor{paSensor}
{
    // readFromEEPROM(); // TODO: implement reading initial settings from EEPROM
    UI::updateDisplayFlag = false;
}

void UI::initializeDisplay()
{
    display->fill(0);
    display->text("Boot", 0, 0);
    display->show();
}

void UI::run()
{
    Timer::DelayTimeout updateTimeout(40);
    display = std::make_shared<ssd1306os>(i2cBus);
    initializeDisplay();

    while (true)
    {
        switch (m_State)
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

        updateTimeout();
    }
}

void UI::displayRefreshCallback(TimerHandle_t xTimer)
{
    UI::updateDisplayFlag = true;
}

void UI::displayMenu()
{
    static size_t selectedIndex = 0;

    display->fill(0);

    // Display each menu option
    for (size_t i = 0; i < menuOptions.size(); ++i)
    {
        if (i == selectedIndex)
        {
            display->text("> " + menuOptions[i], 0, i * 10);
        }
        else { display->text("  " + menuOptions[i], 0, i * 10); }
    }
    display->show();

    Gpio::inputPin command;

    if (xQueueReceive(m_InputQueue, &command, 0) == pdPASS)
    {
        switch (command)
        {
            case Gpio::ROT_A:
                selectedIndex = (selectedIndex + 1) % menuOptions.size();
                break;
            case Gpio::ROT_B:
                selectedIndex = (selectedIndex - 1 + menuOptions.size())
                                % menuOptions.size();
                break;
            case Gpio::ROT_SW:
                if (selectedIndex == 0) { m_State = SENSOR_VALUES; }
                else if (selectedIndex == 1) { m_State = WIFI_SETTINGS; }
                else if (selectedIndex == 2) { m_State = THINGSPEAK_SETTINGS; }
                break;
            case Gpio::SW0:
            case Gpio::SW1:
            case Gpio::SW2:
            default:
                break;
        }
    }
}

void UI::displaySensorValues()
{
    constexpr uint8_t FONT_SIZE = 8;

    // Format values
    std::ostringstream co2Stream, tempStream, humidityStream, pressureStream;
    co2Stream << std::fixed << std::setprecision(2) << m_Co2Sensor->getCo2();
    tempStream << std::fixed << std::setprecision(1) << m_RhSensor->getTemp();
    humidityStream << std::fixed << std::setprecision(1) << m_RhSensor->getRh();
    pressureStream << std::fixed << std::setprecision(1) << m_PaSensor->getPressure();

    std::string co2TargetText = "Set CO2: ";
    std::string co2TargetNumber = std::to_string(m_Co2Target);

    display->fill(0); // Clear the display
    display->text(co2TargetText, 0, 0);
    if (m_Co2SetEnabled)
    {
        display->rect(co2TargetText.size() * FONT_SIZE,
                      0,
                      co2TargetNumber.size() * FONT_SIZE,
                      FONT_SIZE,
                      1,
                      true);
        display->text(co2TargetNumber, co2TargetText.size() * FONT_SIZE, 0, 0);
    }
    else
    {
        display->text(co2TargetNumber, co2TargetText.size() * FONT_SIZE, 0);
    }
    display->text("ppm", (co2TargetText.size() + co2TargetNumber.size()) * FONT_SIZE, 0);
    display->text("CO2: " + co2Stream.str() + "ppm", 0, 20);
    display->text("Humidity: " + humidityStream.str() + "%", 0, 30);
    display->text("Temp: " + tempStream.str() + "C", 0, 40);
    display->text("Pressure: " + pressureStream.str() + "Pa", 0, 50);
    display->show();
}

void UI::displayWiFiSettings()
{
    constexpr uint8_t MAX_LINE_CHARS = 16;
    constexpr uint8_t SSID_MAX = 31;
    constexpr uint16_t PASSWORD_MIN = 8;
    constexpr uint16_t PASSWORD_MAX = 63;

    int charIndex = 0;

    std::string ssid, password;
    bool isSSIDInput = true; // Flag for SSID or password input

    while (m_State == WIFI_SETTINGS)
    {
        display->fill(0); // Clear the display

        if (isSSIDInput)
        {
            display->text("Enter SSID:", 0, 0);

            if (ssid.size() > MAX_LINE_CHARS)
            {
                display->rect(0, 10, 8 * MAX_LINE_CHARS, 8, 0);
                display->text(ssid.substr(ssid.size() - MAX_LINE_CHARS), 0, 10);
            }
            else { display->text(ssid, 0, 10); }
        }
        else
        {
            display->text("Enter Password:", 0, 0);
            if (password.size() > MAX_LINE_CHARS)
            {
                display->rect(0, 10, 8 * MAX_LINE_CHARS, 8, 0);
                display->text(password.substr(password.size() - MAX_LINE_CHARS), 0, 10);
            }
            else { display->text(password, 0, 10); }
        }

        // Display current character selection
        display->text("Char: " + std::string(1, characterSet[charIndex]), 0, 20);
        display->text("Press to select", 0, 40);
        display->show();

        Gpio::inputPin command;

        if (xQueueReceive(m_InputQueue, &command, pdMS_TO_TICKS(100)) == pdPASS)
        {
            if (command == Gpio::ROT_A)
            {
                charIndex = (charIndex + 1) % characterSet.size();
            }
            else if (command == Gpio::ROT_B)
            {
                charIndex = (charIndex - 1 + characterSet.size())
                            % characterSet.size();
            }
            else if (command == Gpio::ROT_SW)
            {
                if (isSSIDInput && ssid.length() < SSID_MAX)
                {
                    ssid += characterSet[charIndex];
                }
                else if (!isSSIDInput && password.length() < PASSWORD_MAX)
                {
                    password += characterSet[charIndex];
                }
            }
            else if (command == Gpio::SW1)
            {
                // Delete last character from SSID or Password
                if (isSSIDInput && !ssid.empty()) { ssid.pop_back(); }
                else if (!isSSIDInput && !password.empty())
                {
                    password.pop_back();
                }
            }

            else if (command == Gpio::SW2)
            {
                if (isSSIDInput) { isSSIDInput = false; }
                else if (password.size() >= PASSWORD_MIN)
                {
                    // TODO: handle sending wifi credentials to network task
                    m_State = MAIN_MENU;
                }
            }
            else if (command == Gpio::SW0)
            {
                ssid.clear();
                password.clear();
                m_State = MAIN_MENU;
            }
        }
    }
}

void UI::displayThingSpeakSettings()
{
    // TODO: implement
    display->fill(0);
    display->text("Work in progress", 0, 20);
    display->show();

    vTaskDelay(3000);
}

void UI::setCO2Target()
{
    Gpio::inputPin command;

    if (xQueueReceive(m_InputQueue, &command, 0) == pdPASS) // Non-blocking receive
    {
        if (m_Co2SetEnabled)
        {
            if (command == Gpio::ROT_A)
            {
                m_Co2Target += 10; // Increment CO2 level
            }
            else if (command == Gpio::ROT_B)
            {
                m_Co2Target -= 10; // Decrement CO2 level
            }
            else if (command == Gpio::ROT_SW)
            {
                xTaskNotify(m_Co2Controller, *(uint32_t *)&m_Co2Target, eSetValueWithOverwrite);
                // saveToEEPROM(); // TODO: implement saving when the button is pressed
                m_Co2SetEnabled = false;
            }
        }
        else if (command == Gpio::ROT_SW) { m_Co2SetEnabled = true; }
        else if (command == Gpio::SW0) { m_State = MAIN_MENU; }
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
