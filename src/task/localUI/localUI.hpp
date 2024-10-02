#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "task/BaseTask.hpp"
#include "RotaryEncoder.hpp"
#include "ssd1306os.h"
#include "PicoI2C.hpp"
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
       const std::shared_ptr<I2c::PicoI2C>& i2cBus,
       const std::shared_ptr<Modbus::Client>& modbusClient,
       TaskHandle_t co2ControllerHandle);
    void run() override;

  private:
    void initializeDisplay();
    void updateDisplay();
    void handleInput();
    void setCO2Level(float level);
    void saveToEEPROM();
    void readFromEEPROM();

    float m_Co2Target;
    GPIO::RotaryEncoder m_RotaryEncoder;
    QueueHandle_t rotaryQueue;
    const uint32_t CO2_SET_POINT_ADDRESS = 0x00; // EEPROM address for CO2 set point
    ssd1306os display;
    Sensor::HMP60 tempRHSensor;
    Sensor::SDP600 pressureSensor;
    Sensor::GMP252 co2Sensor;
    TaskHandle_t co2ControllerHandle;
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */