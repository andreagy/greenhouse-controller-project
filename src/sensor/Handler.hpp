#ifndef HANDLER_HPP
#define HANDLER_HPP

// TODO: Trigger reading from modbus and update sensor values to relevant classes
#include "sensor/BaseSensor.hpp"
#include "task/BaseTask.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace Sensor
{
class Handler : public Task::BaseTask
{
  public:
    Handler(uint32_t updateFrequency);
    void attach(std::shared_ptr<Sensor::BaseSensor> sensor);
    void run() override;

  private:
    uint32_t m_UpdateFrequency;
    std::vector<std::shared_ptr<Sensor::BaseSensor>> m_Sensors;
    void update();
};
} // namespace Sensor

#endif /* HANDLER_HPP */
