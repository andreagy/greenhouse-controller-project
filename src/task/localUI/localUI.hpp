#ifndef LOCALUI_HPP
#define LOCALUI_HPP

#include "task/BaseTask.hpp"
#include "RotaryEncoder.hpp"
#include "ssd1306os.h"
#include "PicoI2C.hpp"
#include "sensor/GMP252.hpp"
#include "sensor/HMP60.hpp"
#include "sensor/SDP600.hpp"
#include "task/Co2Controller.hpp"
#include <queue>
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
       std::shared_ptr<I2c::PicoI2C> i2cBus,
       std::shared_ptr<Modbus::Client> modbusClient,
       TaskHandle_t co2ControllerHandle);
    void run() override;

  private:
    void updateDisplay(); // Update the OLED display with sensor data
    void handleInput();   // Handle user input from rotary encoder and buttons
    void setCO2Level(float level); // Set the CO2 target level
    void saveToEEPROM(); // Save settings to EEPROM
    void readFromEEPROM(); // Read settings from EEPROM

    float m_Co2Target; // Target CO2 level
    GPIO::RotaryEncoder m_RotaryEncoder; // Instance of RotaryEncoder
    QueueHandle_t rotaryQueue; // Queue to receive rotary encoder commands
    const uint32_t CO2_SET_POINT_ADDRESS = 0x00; // EEPROM address for CO2 set point
    ssd1306os display; // Instance for the OLED display
    Sensor::HMP60 tempRHSensor;
    Sensor::SDP600 pressureSensor;
    Sensor::GMP252 co2Sensor;
    TaskHandle_t co2ControllerHandle;
};

} // namespace LocalUI

} // namespace Task

#endif /* LOCALUI_HPP */