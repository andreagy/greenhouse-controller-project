#ifndef MIO12V_HPP
#define MIO12V_HPP

#include "ModbusClient.h"
#include "ModbusRegister.h"

#include <memory>

namespace Fan
{

class MIO12V
{
  private:
    const int mModbusAddress = 1;
    const int mFanSpeedRegisterAddress = 0x0000;
    const int mFanRotationRegisterAddress = 0x0004;
    uint16_t mFanSpeed;
    ModbusRegister mFanSpeedRegister;
    ModbusRegister mFanRotationRegister;

  public:
    MIO12V(std::shared_ptr<ModbusClient> modbus);
    MIO12V(const MIO12V &) = delete;
    uint16_t getFanRotation();
    uint16_t getFanSpeed();
    void setFanSpeed(int speed);
};

} // namespace Fan

#endif /* MIO12V_HPP */
