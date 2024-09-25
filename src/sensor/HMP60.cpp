#include "HMP60.hpp"

#include "pico/time.h"

namespace Sensor
{

HMP60::HMP60(std::shared_ptr<ModbusClient> modbus) :
    mRelativeHumidity{0},
    mTemperature{0},
    mHumidityRegisterLow{modbus, mModbusAddress, RH_REGISTER_LOW},
    mHumidityRegisterHigh{modbus, mModbusAddress, RH_REGISTER_HIGH},
    mTemperatureRegisterLow{modbus, mModbusAddress, TEMPERATURE_REGISTER_LOW},
    mTemperatureRegisterHigh{modbus, mModbusAddress, TEMPERATURE_REGISTER_HIGH}
{}

float HMP60::getRelativeHumidity() { return mRelativeHumidity.f; };

float HMP60::getTemperature() { return mTemperature.f; }

void HMP60::update()
{
    mRelativeHumidity.u = mHumidityRegisterLow.read();
    sleep_ms(5);
    mRelativeHumidity.u |= (mHumidityRegisterHigh.read() << 16);
    sleep_ms(5);
    mTemperature.u = mTemperatureRegisterLow.read();
    sleep_ms(5);
    mTemperature.u |= (mTemperatureRegisterHigh.read() << 16);
    sleep_ms(5);
}

} // namespace Sensor