#ifndef CO2CONTROLLER_HPP
#define CO2CONTROLLER_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "queue.h"
#include "sensor/SensorData.hpp"
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
    Controller(std::shared_ptr<Storage::Eeprom> eeprom,
               QueueHandle_t dataQueue,
               QueueHandle_t targetQueue);
    float getTarget();
    void run() override;

  private:
    const uint m_ValvePin = 27;
    const float m_Co2Min = 200;
    const float m_Co2Max = 1500;
    uint32_t m_Co2Target = 900;
    Sensor::SensorData m_SensorData;
    QueueHandle_t m_DataQueue;
    QueueHandle_t m_TargetQueue;
    std::shared_ptr<Storage::Eeprom> m_Eeprom;
    void setTarget(uint32_t target);
};

} // namespace Co2

} // namespace Task

#endif /* CO2CONTROLLER_HPP */
