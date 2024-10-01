#include "Reader.hpp"

#include "portmacro.h"
#include "projdefs.h"
#include "task/BaseTask.hpp"
#include "timers.h"

#include <memory>

// TODO: Implement as display updater

namespace Sensor
{

Reader::Reader(uint32_t msUpdateRate) :
    Task::BaseTask{"SensorReader", 256, this, Task::MED},
    m_UpdateRate{msUpdateRate}
{}

void Reader::attach(std::shared_ptr<Sensor::BaseSensor> sensor)
{
    m_Sensors.push_back(sensor);
}

void Reader::run()
{
    TickType_t lastUpdate = xTaskGetTickCount();

    while (true)
    {
        update();
        xTaskDelayUntil(&lastUpdate, pdMS_TO_TICKS(m_UpdateRate));
    }
}

void Reader::update()
{
    for (auto &sensor : m_Sensors) { sensor->update(); }
}

} // namespace Sensor