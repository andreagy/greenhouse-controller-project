#ifndef READER_HPP
#define READER_HPP

#include "sensor/BaseSensor.hpp"
#include "task/BaseTask.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace Task
{

namespace Sensor
{

class Reader : public Task::BaseTask
{
  public:
    Reader();
    void attach(std::shared_ptr<::Sensor::BaseSensor> sensor);
    void run() override;

  private:
    uint32_t m_EventBits = 0;
    std::vector<std::shared_ptr<::Sensor::BaseSensor>> m_Sensors;
    void update();
};

} // namespace Sensor

} // namespace Task

#endif /* READER_HPP */
