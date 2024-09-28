#include "Handler.hpp"

#include "portmacro.h"
#include "projdefs.h"
#include "task/BaseTask.hpp"
#include "timers.h"

#include <memory>

namespace Sensor
{

Handler::Handler(uint32_t updateFrequency) :
    Task::BaseTask{"SensorHandler", 512, this, Task::MED},
    m_UpdateFrequency{updateFrequency}
{}

void Handler::attach(std::shared_ptr<Sensor::BaseSensor> sensor)
{
    m_Sensors.push_back(sensor);
}

void Handler::run()
{
    TickType_t lastUpdate = xTaskGetTickCount();

    while (true)
    {
        update();
        xTaskDelayUntil(&lastUpdate, pdMS_TO_TICKS(m_UpdateFrequency));
    }
}

void Handler::update()
{
    for (auto &sensor : m_Sensors) { sensor->update(); }
}

} // namespace Sensor