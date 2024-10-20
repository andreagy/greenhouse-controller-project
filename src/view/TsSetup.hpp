#ifndef TSSETUP_HPP
#define TSSETUP_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "display/ssd1306os.h"
#include "gpio/GpioInput.hpp"
#include "network/NetData.hpp"
#include "queue.h"

#include <cstdint>
#include <memory>

namespace View
{

enum TsSetupState
{
    SET_API_KEY,
    SET_TALKBACK_KEY,
    SET_TALKBACK_ID,
    TS_EXIT
};

class TsSetup
{
  public:
    TsSetup(std::shared_ptr<ssd1306os> display,
            QueueHandle_t inputQueue,
            QueueHandle_t settingsQueue);
    void display();

  private:
    uint8_t m_CharIndex = 0;
    TsSetupState m_State = SET_API_KEY;
    Gpio::inputPin m_InputPin;
    std::string m_ApiKey;
    std::string m_TalkbackKey;
    std::string m_TalkbackId;
    std::shared_ptr<ssd1306os> m_Display;
    QueueHandle_t m_InputQueue;
    QueueHandle_t m_SettingsQueue;
    Network::Settings m_NetSettings;
    void showPrompt(std::string prompt, std::string &string);
    void handleInput(std::string &string, uint8_t requiredLength);
    void clearStrings();
};

} // namespace View

#endif /* TSSETUP_HPP */
