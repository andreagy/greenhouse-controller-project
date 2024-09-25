#include "Mutex.hpp"

namespace Semaphore
{

Mutex::Mutex() { m_Semaphore = xSemaphoreCreateMutex(); }

Mutex::~Mutex()
{
    /* delete semaphore */
    /* (not needed if object lifetime is known
     * to be infinite) */
}

void Mutex::lock() { xSemaphoreTake(m_Semaphore, portMAX_DELAY); }

bool Mutex::try_lock() { return xSemaphoreTake(m_Semaphore, 0); }

void Mutex::unlock() { xSemaphoreGive(m_Semaphore); }

} // namespace Semaphore