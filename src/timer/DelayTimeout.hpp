#ifndef TIMEOUT_HPP
#define TIMEOUT_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "portmacro.h"
#include "timers.h"

#include <cstdint>

namespace Timer
{

class DelayTimeout
{
  public:
    DelayTimeout(uint32_t ms);
    void reset();
    bool operator()();

  private:
    uint32_t m_Timeout;
    TickType_t m_LastTrigger;
};

} // namespace Timer

#endif /* TIMEOUT_HPP */
