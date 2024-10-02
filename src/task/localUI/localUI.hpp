#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "task/BaseTask.hpp"
//#include "display/ssd1306.h"
#include "display/ssd1306os.h"
#include "i2c/PicoI2C.hpp"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task/co2/Co2Controller.hpp"
#include "queue.h"
#include <cstdint>
#include <memory> // Include for std::shared_ptr

namespace Task
{

namespace LocalUI
{

class UI : public BaseTask
{
  public:
    UI(QueueHandle_t rotaryQueue,
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
    void handleInput();
    void updateDisplay();
    static void displayRefreshCallback(TimerHandle_t xTimer);
    //void saveToEEPROM();
    //void readFromEEPROM();
    float m_Co2Target;
    QueueHandle_t rotaryQueue;
    const uint32_t CO2_SET_POINT_ADDRESS = 0x00; // EEPROM address for CO2 set point
    std::shared_ptr<I2c::PicoI2C> i2cBus;
    std::shared_ptr<ssd1306os> display;
    std::shared_ptr<Co2::Controller> co2Controller;
    std::shared_ptr<Sensor::GMP252> co2Sensor;
    std::shared_ptr<Sensor::HMP60> tempRhSensor;
    std::shared_ptr<Sensor::SDP600> paSensor;
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */