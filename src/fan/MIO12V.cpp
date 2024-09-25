#include "MIO12V.hpp"

#include "pico/time.h"

namespace Fan
{

MIO12V::MIO12V(std::shared_ptr<ModbusClient> modbus) :
    mFanSpeed{0},
    mFanSpeedRegister{modbus, mModbusAddress, mFanSpeedRegisterAddress},
    mFanRotationRegister{modbus, mModbusAddress, mFanRotationRegisterAddress, false}
{
    mFanSpeed = mFanSpeedRegister.read();
    sleep_ms(5);
}

uint16_t MIO12V::getFanRotation()
{
    return mFanRotationRegister.read();
    sleep_ms(5);
}

uint16_t MIO12V::getFanSpeed() { return mFanSpeed; }

void MIO12V::setFanSpeed(int speed)
{
    if (speed < 0 || speed > 1000) { speed = speed < 1000 ? 0 : 1000; }

    mFanSpeedRegister.write(speed);
    sleep_ms(5);
    mFanSpeed = mFanSpeedRegister.read();
    sleep_ms(5);
}

} // namespace Fan