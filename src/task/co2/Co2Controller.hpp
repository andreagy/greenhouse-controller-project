#ifndef CO2CONTROLLER_HPP
#define CO2CONTROLLER_HPP

#include "sensor/GMP252.hpp"
#include "storage/Eeprom.hpp"
#include "task/BaseTask.hpp"

#include <memory>

namespace Task
{

namespace Co2
{

class Controller : public BaseTask
{
  public:
    Controller(std::shared_ptr<Sensor::GMP252> co2Sensor,
               TaskHandle_t fanController,
               QueueHandle_t targetQueue,
               std::shared_ptr<Storage::Eeprom> eeprom);
    float getTarget();
    void run() override;

  private:
    const uint m_ValvePin = 27;
    const float m_Co2Min = 200;
    const float m_Co2Max = 1500;
    uint32_t m_Co2Target = 900;
    std::shared_ptr<Sensor::GMP252> m_Co2Sensor;
    TaskHandle_t m_FanControlHandle;
    QueueHandle_t m_TargetQueue;
    std::shared_ptr<Storage::Eeprom> m_Eeprom;
    void setTarget(uint32_t target);
};

} // namespace Co2

} // namespace Task

#endif /* CO2CONTROLLER_HPP */
