#include "LocalUI.hpp"

#include "projdefs.h"
#include "view/SensorView.hpp"
#include "view/TsSetup.hpp"
#include "view/WifiSetup.hpp"

#include <cstring>
#include <string>

bool Task::LocalUI::UI::updateDisplayFlag = false;

namespace Task
{

namespace LocalUI
{

UI::UI(const std::shared_ptr<I2c::PicoI2C> &i2c,
       QueueHandle_t inputQueue,
       QueueHandle_t dataQueue,
       QueueHandle_t targetQueue,
       QueueHandle_t settingsQueue) :
    BaseTask{"UserInterface", 512, this, MED},
    m_MenuOptions{"Sensor Values", "Set Network", "Set ThingSpeak"},
    m_I2cBus{i2c},
    m_InputQueue{inputQueue},
    m_DataQueue{dataQueue},
    m_TargetQueue{targetQueue},
    m_SettingsQueue{settingsQueue}
{
    UI::updateDisplayFlag = false;
}

void UI::initializeDisplay()
{
    m_Display->fill(0);
    m_Display->text("Boot", 0, 0);
    m_Display->show();
}

void UI::run()
{
    m_Display = std::make_shared<ssd1306os>(m_I2cBus);

    // Create view objects
    View::SensorView m_SensorView(m_Display, m_InputQueue, m_DataQueue, m_TargetQueue);
    View::WifiSetup m_WifiSetup(m_Display, m_InputQueue, m_SettingsQueue);
    View::TsSetup m_ThingspeakSetup(m_Display, m_InputQueue, m_SettingsQueue);

    initializeDisplay();

    while (true)
    {
        switch (m_State)
        {
            case MAIN_MENU:
                displayMenu();
                break;
            case SENSOR_VALUES:
                m_SensorView.display();
                m_State = MAIN_MENU;
                break;
            case WIFI_SETTINGS:
                m_WifiSetup.display();
                m_State = MAIN_MENU;
                break;
            case THINGSPEAK_SETTINGS:
                m_ThingspeakSetup.display();
                m_State = MAIN_MENU;
                break;
        }
    }
}

void UI::displayMenu()
{
    handleInput();
    m_Display->fill(0);

    // Display each menu option
    for (uint8_t i = 0; i < m_MenuOptions.size(); ++i)
    {
        if (i == m_CharIndex)
        {
            m_Display->text("> " + m_MenuOptions[i], 0, i * 10);
        }
        else { m_Display->text("  " + m_MenuOptions[i], 0, i * 10); }
    }
    m_Display->show();
    vTaskDelay(pdMS_TO_TICKS(40));
}

void UI::handleInput()
{
    if (xQueueReceive(m_InputQueue, &m_InputPin, 0) == pdPASS)
    {
        switch (m_InputPin)
        {
            case Gpio::ROT_A:
                m_CharIndex = (m_CharIndex + 1) % m_MenuOptions.size();
                break;
            case Gpio::ROT_B:
                m_CharIndex = (m_CharIndex - 1 + m_MenuOptions.size())
                              % m_MenuOptions.size();
                break;
            case Gpio::ROT_SW:
                if (m_CharIndex <= m_MenuOptions.size())
                {
                    m_State = static_cast<MenuState>(m_CharIndex);
                }
                break;
            case Gpio::SW0:
            case Gpio::SW1:
            case Gpio::SW2:
            default:
                // We should never be here
                break;
        }
    }
}

} // namespace LocalUI

} // namespace Task
