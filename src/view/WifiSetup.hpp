#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "display/ssd1306os.h"
#include "gpio/GpioInput.hpp"
#include "network/NetData.hpp"
#include "queue.h"

#include <cstdint>
#include <memory>

namespace View
{

enum WifiSetupState
{
    SET_SSID,
    SET_PASSWORD,
    WIFI_EXIT
};

class WifiSetup
{
  public:
    WifiSetup(std::shared_ptr<ssd1306os> display,
              QueueHandle_t inputQueue,
              QueueHandle_t settingsQueue);
    void display();

  private:
    uint8_t m_CharIndex = 0;
    WifiSetupState m_State = SET_SSID;
    Gpio::inputPin m_InputPin;
    std::string m_Ssid;
    std::string m_Password;
    std::shared_ptr<ssd1306os> m_Display;
    QueueHandle_t m_InputQueue;
    QueueHandle_t m_SettingsQueue;
    Network::Settings m_NetSettings;
    void showPrompt(std::string prompt, std::string &string);
    void handleInput(std::string &string, uint8_t maxLength);
    void clearStrings();
};

} // namespace View

#endif /* WIFISETUP_HPP */
