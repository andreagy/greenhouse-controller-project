#ifndef CO2CONTROLLER_HPP
#define CO2CONTROLLER_HPP

#include "sensor/GMP252.hpp"
#include "task/BaseTask.hpp"
#include "timer/Timeout.hpp"

#include <cstdint>
#include <memory>

namespace Task
{

namespace Co2
{

class Controller : public BaseTask
{
  public:
    Controller(std::shared_ptr<Sensor::GMP252> co2Sensor, TaskHandle_t fanController);
    void setTarget(float target);
    float getTarget();
    void run() override;

  private:
    const uint m_ValvePin = 27;
    const float m_Co2Min = 200;
    const float m_Co2Max = 1500;
    float m_Co2Target;
    std::shared_ptr<Sensor::GMP252> m_Co2Sensor;
    TaskHandle_t m_FanControlHandle;
    void pollSensor(uint16_t interval);
};

} // namespace Co2

} // namespace Task

#endif /* CO2CONTROLLER_HPP */
