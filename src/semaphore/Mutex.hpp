#ifndef MUTEX_HPP
#define MUTEX_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "semphr.h"

namespace Semaphore
{
class Mutex
{
  public:
    Mutex();
    virtual ~Mutex();
    void lock();
    bool try_lock();
    void unlock();

  private:
    SemaphoreHandle_t m_Semaphore;
};

} // namespace Semaphore

#endif /* MUTEX_HPP */
