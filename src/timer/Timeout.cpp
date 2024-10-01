#include "Timeout.hpp"

#include "projdefs.h"

namespace Timer
{

Timeout::Timeout(uint32_t ms) :
    m_Timer{xTimerCreate(m_Name.c_str(), pdMS_TO_TICKS(ms), false, NULL, callback)}
{}

void Timeout::reset() { xTimerReset(m_Timer, 0); }

bool Timeout::hasExpired() { return !xTimerIsTimerActive(m_Timer); }

void Timer::Timeout::callback(TimerHandle_t timer) {}

} // namespace Timer