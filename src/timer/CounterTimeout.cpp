#include "CounterTimeout.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "projdefs.h"
#include "task.h"

#include <cstdint>

namespace Timer
{

CounterTimeout::CounterTimeout(uint32_t ms) :
    m_Timeout{ms},
    m_Timestamp{xTaskGetTickCount()}
{}

void CounterTimeout::reset() { m_Timestamp = xTaskGetTickCount(); }

bool CounterTimeout::operator()()
{
    return (xTaskGetTickCount() - m_Timestamp) > pdMS_TO_TICKS(m_Timeout);
}

} // namespace Timer