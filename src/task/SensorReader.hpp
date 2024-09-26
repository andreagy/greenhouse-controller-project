#ifndef SENSORREADER_HPP
#define SENSORREADER_HPP

// TODO: Trigger reading from modbus and update sensor values to relevant classes
#include "sensor/BaseSensor.hpp"
#include "task/BaseTask.hpp"

#include <memory>
#include <vector>

namespace Task
{
class SensorReader : public BaseTask
{
  public:
    SensorReader();
    void attach(std::shared_ptr<Sensor::BaseSensor>);
    void run();

  private:
    std::vector<std::shared_ptr<Sensor::BaseSensor>> m_Sensors;
    void update();
};
} // namespace Task

#endif /* SENSORREADER_HPP */
