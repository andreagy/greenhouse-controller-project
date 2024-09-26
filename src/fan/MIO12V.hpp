#ifndef MIO12V_HPP
#define MIO12V_HPP

#include "modbus/Client.hpp"
#include "modbus/Register.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Fan
{

class MIO12V : public Task::BaseTask
{
  public:
    MIO12V(std::shared_ptr<Modbus::Client> modbus,
           uint32_t stackDepth = 256,
           Task::priority taskPriority = Task::MED);
    MIO12V(const MIO12V &) = delete;
    uint16_t getFanRotation();
    uint16_t getFanSpeed();
    void setFanSpeed(int speed);
    void run();

  private:
    uint16_t m_FanSpeed;
    Modbus::Register m_FanSpeedRegister;
    Modbus::Register m_FanRotationRegister;
};

} // namespace Fan

#endif /* MIO12V_HPP */
