#ifndef GMP252_HPP
#define GMP252_HPP

#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Register32bit.hpp"

#include <memory>

namespace Sensor
{

class GMP252
{
  private:
    const int mModbusAddress = 240;
    Register32bit mCO2;
    Register32bit mTemperature;
    ModbusRegister mCO2RegisterLow;
    ModbusRegister mCO2RegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;

    enum modbusRegisterAddress
    {
        CO2_REGISTER_LOW = 0x0000,
        CO2_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0004,
        TEMPERATURE_REGISTER_HIGH
    };

  public:
    GMP252(std::shared_ptr<ModbusClient> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    void update();
};
} // namespace Sensor

#endif /* GMP252_HPP */
