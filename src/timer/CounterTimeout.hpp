#ifndef COUNTERTIMEOUT_HPP
#define COUNTERTIMEOUT_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "portmacro.h"

#include <cstdint>

namespace Timer
{

class CounterTimeout
{
  public:
    CounterTimeout(uint32_t ms);
    void reset();
    bool operator()();

  private:
    uint32_t m_Timeout;
    TickType_t m_Timestamp;
    
};

} // namespace Timer

#endif /* COUNTERTIMEOUT_HPP */
