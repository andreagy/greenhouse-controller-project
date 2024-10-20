#include "Binary.hpp"

namespace Semaphore
{

Binary::Binary() { m_Semaphore = xSemaphoreCreateBinary(); }

Binary::~Binary()
{
    /* delete semaphore */
    /* (not needed if object lifetime is known
     * to be infinite) */
}

void Binary::take() { xSemaphoreTake(m_Semaphore, portMAX_DELAY); }

void Binary::give() { xSemaphoreGive(m_Semaphore); }

} // namespace Semaphore