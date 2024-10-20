#include "TsSetup.hpp"

#include "CharacterSet.hpp"

#include <cstring>

namespace View
{

constexpr uint8_t MAX_LINE_CHARS = 16;

TsSetup::TsSetup(std::shared_ptr<ssd1306os> display,
                 QueueHandle_t inputQueue,
                 QueueHandle_t settingsQueue) :
    m_Display{display},
    m_InputQueue{inputQueue},
    m_SettingsQueue{settingsQueue}
{}

void TsSetup::display()
{
    constexpr uint8_t KEY_LENGTH = 16;
    constexpr uint16_t ID_LENGTH = 6;

    m_CharIndex = 0;
    m_State = SET_API_KEY;
    clearStrings();

    while (m_State != TS_EXIT)
    {
        m_Display->fill(0);

        switch (m_State)
        {
            case SET_API_KEY:
                handleInput(m_ApiKey, KEY_LENGTH);
                showPrompt("Thingspeak key:", m_ApiKey);
                break;
            case SET_TALKBACK_KEY:
                handleInput(m_TalkbackKey, KEY_LENGTH);
                showPrompt("Talkback Key:", m_TalkbackKey);
                break;
            case SET_TALKBACK_ID:
                handleInput(m_TalkbackId, ID_LENGTH);
                showPrompt("Talkback ID:", m_TalkbackId);
                break;
            case TS_EXIT:
                break;
            default:
                // We should not be here
                m_State = TS_EXIT;
                break;
        }

        // Display current character selection
        m_Display->text("Char: " + std::string(1, characterSet[m_CharIndex]), 0, 20);
        m_Display->text("Press to select", 0, 40);

        m_Display->show();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void TsSetup::showPrompt(std::string prompt, std::string &string)
{
    m_Display->text(prompt, 0, 0);

    if (string.size() > MAX_LINE_CHARS)
    {
        m_Display->rect(0, 10, 8 * MAX_LINE_CHARS, 8, 0);
        m_Display->text(string.substr(string.size() - MAX_LINE_CHARS), 0, 10);
    }
    else { m_Display->text(string, 0, 10); }
}

void TsSetup::handleInput(std::string &string, uint8_t requiredLength)
{
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
                if (string.length() < requiredLength)
                {
                    string += characterSet[m_CharIndex];
                }
                break;
            case Gpio::SW0:
                clearStrings();
                m_State = TS_EXIT;
                break;
            case Gpio::SW1:
                // Delete last character
                if (!string.empty()) { string.pop_back(); }
                break;
            case Gpio::SW2:
                if (m_State == SET_API_KEY && string.size() == requiredLength)
                {
                    m_State = SET_TALKBACK_KEY;
                }
                else if (m_State == SET_TALKBACK_KEY && string.size() == requiredLength)
                {
                    m_State = SET_TALKBACK_ID;
                }
                else if (m_State == SET_TALKBACK_ID && string.size() == requiredLength)
                {
                    strcpy(m_NetSettings.str1, m_ApiKey.c_str());
                    strcpy(m_NetSettings.str2, m_TalkbackKey.c_str());
                    strcpy(m_NetSettings.id, m_TalkbackId.c_str());
                    xQueueSend(m_SettingsQueue, &m_NetSettings, 0);
                    m_State = TS_EXIT;
                }
                break;
            default:
                break;
        }
    }
}

void TsSetup::clearStrings()
{
    m_ApiKey.clear();
    m_TalkbackId.clear();
    m_TalkbackKey.clear();
}

} // namespace View