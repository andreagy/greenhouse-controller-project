#ifndef TIMEOUT_HPP
#define TIMEOUT_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "timers.h"

#include <cstdint>
#include <string>

namespace Timer
{

class Timeout
{
  public:
    Timeout(uint32_t ms);
    void reset();
    bool hasExpired();

  private:
    const std::string m_Name = "Timeout";
    TimerHandle_t m_Timer;
    static void callback(TimerHandle_t timer);
};

} // namespace Timer

#endif /* TIMEOUT_HPP */
