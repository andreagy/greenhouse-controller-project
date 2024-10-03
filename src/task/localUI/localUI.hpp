#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "task/BaseTask.hpp"
#include "display/ssd1306os.h"
#include "i2c/PicoI2C.hpp"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task/co2/Co2Controller.hpp"
#include "queue.h"
#include <cstdint>
#include <memory>

namespace Task
{

namespace LocalUI
{

enum MenuState {
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
    UI(QueueHandle_t rotaryQueue,
       QueueHandle_t buttonQueue,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       const std::shared_ptr<Task::Co2::Controller>& co2Controller,
       const std::shared_ptr<I2c::PicoI2C>& i2c,
       const std::shared_ptr<Sensor::GMP252>& co2Sensor,
       const std::shared_ptr<Sensor::HMP60>& tempRhSensor,
       const std::shared_ptr<Sensor::SDP600>& paSensor);
    void run() override;
    static bool updateDisplayFlag;

  private:
    void initializeDisplay();
    void setCO2Target();
    void displayMenu();
    void displaySensorValues();
    void displayWiFiSettings();
    void displayThingSpeakSettings();
    static void displayRefreshCallback(TimerHandle_t xTimer);
    float m_Co2Target;
    QueueHandle_t rotaryQueue;
    QueueHandle_t buttonQueue;
    std::shared_ptr<I2c::PicoI2C> i2cBus;
    std::shared_ptr<ssd1306os> display;
    std::shared_ptr<Co2::Controller> co2Controller;
    std::shared_ptr<Sensor::GMP252> co2Sensor;
    std::shared_ptr<Sensor::HMP60> tempRhSensor;
    std::shared_ptr<Sensor::SDP600> paSensor;
    std::string ssid;
    std::string password;
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */
