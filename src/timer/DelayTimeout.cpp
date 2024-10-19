#include "DelayTimeout.hpp"

#include "projdefs.h"

namespace Timer
{

DelayTimeout::DelayTimeout(uint32_t ms) :
    m_Timeout{ms},
    m_LastTrigger{xTaskGetTickCount()}
{}

void DelayTimeout::reset() { m_LastTrigger = xTaskGetTickCount(); }

bool DelayTimeout::operator()()
{
    return xTaskDelayUntil(&m_LastTrigger, pdMS_TO_TICKS(m_Timeout));
}

} // namespace Timer