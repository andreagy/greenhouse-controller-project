#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "display/ssd1306os.h"
#include "i2c/PicoI2C.hpp"
#include "network/NetData.hpp"
#include "queue.h"
#include "sensor/SensorData.hpp"
#include "task/BaseTask.hpp"
#include "timers.h"

#include <cstdint>
#include <memory>

namespace Task
{

namespace LocalUI
{

enum MenuState
{
    MAIN_MENU,
    SENSOR_VALUES,
    WIFI_SETTINGS,
    THINGSPEAK_SETTINGS
};

// Declare currentState as extern
extern MenuState currentState;

class UI : public BaseTask
{
  public:
    UI(const std::shared_ptr<I2c::PicoI2C> &i2c,
       QueueHandle_t inputQueue,
       QueueHandle_t targetQueue,
       QueueHandle_t dataQueue,
       QueueHandle_t settingsQueue);
    static bool updateDisplayFlag;
    void run() override;

  private:
    uint32_t m_Co2Target = 0;
    MenuState m_State = MAIN_MENU;
    bool m_Co2SetEnabled = false;
    std::shared_ptr<I2c::PicoI2C> i2cBus;
    std::shared_ptr<ssd1306os> display;
    QueueHandle_t m_InputQueue;
    QueueHandle_t m_TargetQueue;
    QueueHandle_t m_DataQueue;
    QueueHandle_t m_SettingsQueue;
    Sensor::SensorData m_SensorData;
    Network::Settings m_NetSettings;
    void initializeDisplay();
    void setCO2Target();
    void displayMenu();
    void displaySensorValues();
    void displayWiFiSettings();
    void displayThingSpeakSettings();
    static void displayRefreshCallback(TimerHandle_t xTimer);
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */
