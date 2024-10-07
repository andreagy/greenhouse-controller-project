#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "display/ssd1306os.h"
#include "i2c/PicoI2C.hpp"
#include "queue.h"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task/BaseTask.hpp"

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
    UI(QueueHandle_t inputQueue,
       TaskHandle_t co2Controller,
       const std::shared_ptr<Modbus::Client> &modbusClient,
       const std::shared_ptr<I2c::PicoI2C> &i2c,
       const std::shared_ptr<Sensor::GMP252> &co2Sensor,
       const std::shared_ptr<Sensor::HMP60> &tempRhSensor,
       const std::shared_ptr<Sensor::SDP600> &paSensor);
    static bool updateDisplayFlag;
    void run() override;

  private:
    uint32_t m_Co2Target = 900;
    MenuState m_State = MAIN_MENU;
    bool m_Co2SetEnabled = false;
    QueueHandle_t m_InputQueue;
    TaskHandle_t m_Co2Controller;
    std::shared_ptr<I2c::PicoI2C> i2cBus;
    std::shared_ptr<ssd1306os> display;
    std::shared_ptr<Sensor::GMP252> m_Co2Sensor;
    std::shared_ptr<Sensor::HMP60> m_RhSensor;
    std::shared_ptr<Sensor::SDP600> m_PaSensor;
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
