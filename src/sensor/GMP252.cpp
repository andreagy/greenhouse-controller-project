#include "GMP252.hpp"

#include "pico/time.h"

namespace Sensor
{

GMP252::GMP252(std::shared_ptr<ModbusClient> modbus) :
    mCO2{0},
    mTemperature{0},
    mCO2RegisterLow{modbus, mModbusAddress, CO2_REGISTER_LOW},
    mCO2RegisterHigh{modbus, mModbusAddress, CO2_REGISTER_HIGH},
    mTemperatureRegisterLow{modbus, mModbusAddress, TEMPERATURE_REGISTER_LOW},
    mTemperatureRegisterHigh{modbus, mModbusAddress, TEMPERATURE_REGISTER_HIGH}
{}

// Returns CO2 value in PPM.
float GMP252::getCO2() { return mCO2.f; }

// Returns temperature value in C.
float GMP252::getTemperature() { return mTemperature.f; }

// Update sensor values.
void GMP252::update()
{
    mCO2.u = mCO2RegisterLow.read();
    sleep_ms(5);
    mCO2.u |= (mCO2RegisterHigh.read() << 16);
    sleep_ms(5);
    mTemperature.u = mTemperatureRegisterLow.read();
    sleep_ms(5);
    mTemperature.u |= (mTemperatureRegisterHigh.read() << 16);
    sleep_ms(5);
}

} // namespace Sensor