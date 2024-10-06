#include "Reader.hpp"

#include "task/BaseTask.hpp"
#include "timer/DelayTimeout.hpp"
#include "timers.h"

#include <memory>

namespace Task
{

namespace Sensor
{

Reader::Reader() : Task::BaseTask{"SensorReader", 256, this, HIGH} {}

void Reader::attach(std::shared_ptr<::Sensor::BaseSensor> sensor)
{
    m_Sensors.push_back(sensor);
}

void Reader::run()
{
    Timer::DelayTimeout updateTimeout(100);

    while (true)
    {
        update();
        updateTimeout();
    }
}

void Reader::update()
{
    for (auto sensor : m_Sensors) { sensor->update(); }
}

} // namespace Sensor

} // namespace Task