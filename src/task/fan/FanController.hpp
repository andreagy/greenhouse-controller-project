#ifndef FANCONTROLLER_HPP
#define FANCONTROLLER_HPP

#include "modbus/MbClient.hpp"
#include "modbus/Register.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Task
{

namespace Fan
{

class Controller : public BaseTask
{
  public:
    Controller(std::shared_ptr<Modbus::Client> modbus,
               uint32_t stackDepth = 256,
               priority taskPriority = Task::HIGH);
    Controller(const Controller &) = delete;
    uint16_t getPulse();
    uint16_t getSpeed();
    void setSpeed(uint16_t speed);
    void run();

  private:
    uint16_t m_Speed = 0;
    Modbus::Register m_SpeedRegister;
    Modbus::Register m_PulseRegister;
};

} // namespace Fan

} // namespace Task

#endif /* FANCONTROLLER_HPP */
