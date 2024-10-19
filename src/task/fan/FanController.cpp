#include "FanController.hpp"

#include "timers.h"

#include <climits>
#include <cstdint>

namespace Task
{

namespace Fan
{

constexpr int MODBUS_ADDR = 1;
constexpr int SPEED_REG_ADDR = 0x0000;
constexpr int ROT_REG_ADDR = 0x0004;
constexpr uint16_t FAN_MIN = 150;
constexpr uint16_t FAN_MAX = 1000;

Controller::Controller(std::shared_ptr<Modbus::Client> modbus) :
    BaseTask("FanController", 256, this, HIGH),
    m_SpeedRegister{modbus, MODBUS_ADDR, SPEED_REG_ADDR},
    m_PulseRegister{modbus, MODBUS_ADDR, ROT_REG_ADDR, false}
{}

uint16_t Controller::getPulse() { return m_PulseRegister.read(); }

uint16_t Controller::getSpeed() { return m_Speed; }

void Controller::setSpeed(uint16_t speed)
{
    if (speed < FAN_MIN) { speed = 0; }
    if (speed > FAN_MAX) { speed = FAN_MAX; }

    m_SpeedRegister.write(speed);
    m_Speed = m_SpeedRegister.read();
}

void Controller::run()
{
    uint32_t speed = 0;

    setSpeed(0);

    while (true)
    {
        xTaskNotifyWait(0, ULONG_MAX, &speed, portMAX_DELAY);
        setSpeed(static_cast<uint16_t>(speed));
    }
}

} // namespace Fan

} // namespace Task