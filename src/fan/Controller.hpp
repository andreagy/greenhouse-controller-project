#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "modbus/Client.hpp"
#include "modbus/Register.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Fan
{

class Controller : public Task::BaseTask
{
  public:
    Controller(std::shared_ptr<Modbus::Client> modbus,
               uint32_t stackDepth = 256,
               Task::priority taskPriority = Task::HIGH);
    Controller(const Controller &) = delete;
    uint16_t getPulse();
    uint16_t getSpeed();
    void setFanSpeed(int speed);
    void run();

  private:
    uint16_t m_FanSpeed;
    Modbus::Register m_SpeedRegister;
    Modbus::Register m_PulseRegister;
};

} // namespace Fan

#endif /* CONTROLLER_HPP */
