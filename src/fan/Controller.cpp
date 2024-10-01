#include "Controller.hpp"

#include "pico/time.h"
#include "projdefs.h"
#include "task/BaseTask.hpp"

#include <cstdint>

namespace Fan
{

constexpr int MODBUS_ADDR = 1;
constexpr int SPEED_REG_ADDR = 0x0000;
constexpr int ROT_REG_ADDR = 0x0004;

Controller::Controller(std::shared_ptr<Modbus::Client> modbus,
                       uint32_t stackDepth,
                       Task::priority taskPriority) :
    Task::BaseTask("FanController", stackDepth, this, taskPriority),
    m_SpeedRegister{modbus, MODBUS_ADDR, SPEED_REG_ADDR},
    m_PulseRegister{modbus, MODBUS_ADDR, ROT_REG_ADDR, false}
{}

uint16_t Controller::getPulse() { return m_PulseRegister.read(); }

uint16_t Controller::getSpeed() { return m_Speed; }

void Controller::setFanSpeed(int speed)
{
    if (speed < 0 || speed > 1000) { speed = speed < 1000 ? 0 : 1000; }

    m_SpeedRegister.write(speed);
    m_Speed = m_SpeedRegister.read();
}

void Controller::run()
{
    uint16_t newSpeed = 0;

    while (true)
    {
        // TODO: block on queue waiting for new fan speed setting
        if (true) { setFanSpeed(newSpeed); }
    }
}

} // namespace Fan