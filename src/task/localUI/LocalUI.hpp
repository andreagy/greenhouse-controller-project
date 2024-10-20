#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "display/ssd1306os.h"
#include "gpio/GpioInput.hpp"
#include "i2c/PicoI2C.hpp"
#include "network/NetData.hpp"
#include "queue.h"
#include "task/BaseTask.hpp"
#include "timers.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace Task
{

namespace LocalUI
{

enum MenuState : uint8_t
{
    SENSOR_VALUES,
    WIFI_SETTINGS,
    THINGSPEAK_SETTINGS,
    MAIN_MENU
};

class UI : public BaseTask
{
  public:
    UI(const std::shared_ptr<I2c::PicoI2C> &i2c,
       QueueHandle_t inputQueue,
       QueueHandle_t dataQueue,
       QueueHandle_t targetQueue,
       QueueHandle_t settingsQueue);
    static bool updateDisplayFlag;
    void run() override;

  private:
    uint8_t m_CharIndex = 0;
    ::Gpio::inputPin m_InputPin;
    MenuState m_State = MAIN_MENU;
    const std::vector<std::string> m_MenuOptions;
    std::shared_ptr<I2c::PicoI2C> m_I2cBus;
    std::shared_ptr<ssd1306os> m_Display;
    QueueHandle_t m_InputQueue;
    QueueHandle_t m_DataQueue;
    QueueHandle_t m_TargetQueue;
    QueueHandle_t m_SettingsQueue;
    Network::Settings m_NetSettings;
    void initializeDisplay();
    void displayMenu();
    void handleInput();
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */
