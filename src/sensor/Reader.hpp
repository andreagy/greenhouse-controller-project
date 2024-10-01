#ifndef READER_HPP
#define READER_HPP

#include "sensor/BaseSensor.hpp"
#include "task/BaseTask.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace Sensor
{
class Reader : public Task::BaseTask
{
  public:
    Reader(uint32_t msUpdateRate = 250);
    void attach(std::shared_ptr<Sensor::BaseSensor> sensor);
    void run() override;

  private:
    uint32_t m_UpdateRate;
    std::vector<std::shared_ptr<Sensor::BaseSensor>> m_Sensors;
    void update();
};
} // namespace Sensor

#endif /* READER_HPP */
