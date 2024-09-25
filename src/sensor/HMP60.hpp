#ifndef HMP60_HPP
#define HMP60_HPP

#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Register32bit.hpp"

#include <memory>

namespace Sensor
{

class HMP60
{
  private:
    const int mModbusAddress = 241;
    Register32bit mRelativeHumidity;
    Register32bit mTemperature;
    ModbusRegister mHumidityRegisterLow;
    ModbusRegister mHumidityRegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;

    enum modbusRegisterAddress
    {
        RH_REGISTER_LOW = 0x0000,
        RH_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0002,
        TEMPERATURE_REGISTER_HIGH
    };

  public:
    HMP60(std::shared_ptr<ModbusClient> modbus);
    HMP60(const HMP60 &) = delete;
    float getRelativeHumidity();
    float getTemperature();
    void update();
};

} // namespace Sensor

#endif /* HMP60_HPP */
