#include "WifiSetup.hpp"

#include "CharacterSet.hpp"

#include <cstring>

namespace View
{

constexpr uint8_t MAX_LINE_CHARS = 16;

WifiSetup::WifiSetup(std::shared_ptr<ssd1306os> display,
                     QueueHandle_t inputQueue,
                     QueueHandle_t settingsQueue) :
    m_Display{display},
    m_InputQueue{inputQueue},
    m_SettingsQueue{settingsQueue}
{}

void WifiSetup::display()
{
    constexpr uint8_t SSID_MAX = 31;
    constexpr uint16_t PASSWORD_MAX = 63;

    m_CharIndex = 0;
    m_State = SET_SSID;
    clearStrings();

    while (m_State != WIFI_EXIT)
    {
        m_Display->fill(0);

        switch (m_State)
        {
            case SET_SSID:
                handleInput(m_Ssid, SSID_MAX);
                showPrompt("Enter SSID:", m_Ssid);
                break;
            case SET_PASSWORD:
                handleInput(m_Password, PASSWORD_MAX);
                showPrompt("Enter Password:", m_Password);
                break;
            case WIFI_EXIT:
                break;
            default:
                // We should not be here
                m_State = WIFI_EXIT;
                break;
        }

        // Display current character selection
        m_Display->text("Char: " + std::string(1, characterSet[m_CharIndex]), 0, 20);
        m_Display->text("Press to select", 0, 40);

        m_Display->show();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void WifiSetup::showPrompt(std::string prompt, std::string &string)
{
    m_Display->text(prompt, 0, 0);

    if (string.size() > MAX_LINE_CHARS)
    {
        m_Display->rect(0, 10, 8 * MAX_LINE_CHARS, 8, 0);
        m_Display->text(string.substr(string.size() - MAX_LINE_CHARS), 0, 10);
    }
    else { m_Display->text(string, 0, 10); }
}

void WifiSetup::handleInput(std::string &string, uint8_t maxLength)
{
    constexpr uint16_t PASSWORD_MIN = 8;

    if (xQueueReceive(m_InputQueue, &m_InputPin, pdMS_TO_TICKS(100)) == pdPASS)
    {
        switch (m_InputPin)
        {
            case Gpio::ROT_A:
                m_CharIndex = (m_CharIndex + 1) % characterSet.size();
                break;
            case Gpio::ROT_B:
                m_CharIndex = (m_CharIndex - 1 + characterSet.size())
                              % characterSet.size();
                break;
            case Gpio::ROT_SW:
                if (string.length() < maxLength)
                {
                    string += characterSet[m_CharIndex];
                }
                break;
            case Gpio::SW0:
                clearStrings();
                m_State = WIFI_EXIT;
                break;
            case Gpio::SW1:
                // Delete last character
                if (!string.empty()) { string.pop_back(); }
                break;
            case Gpio::SW2:
                if (m_State == SET_SSID) { m_State = SET_PASSWORD; }
                else if (m_State == SET_PASSWORD && string.size() >= PASSWORD_MIN)
                {
                    strcpy(m_NetSettings.str1, m_Ssid.c_str());
                    strcpy(m_NetSettings.str2, m_Password.c_str());
                    strcpy(m_NetSettings.id, "");
                    xQueueSend(m_SettingsQueue, &m_NetSettings, 0);
                    m_State = WIFI_EXIT;
                }
                break;
            default:
                break;
        }
    }
}

void WifiSetup::clearStrings()
{
    m_Ssid.clear();
    m_Password.clear();
}

} // namespace View