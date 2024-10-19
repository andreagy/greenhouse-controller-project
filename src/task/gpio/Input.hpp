#ifndef INPUT_HPP
#define INPUT_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "portmacro.h"
#include "queue.h"
#include "task/BaseTask.hpp"

namespace Task
{

namespace Gpio
{

class Input : public BaseTask
{
  public:
    Input(QueueHandle_t queue);
    void run() override;

  private:
    static TickType_t s_LastInput;
    static QueueHandle_t s_Queue;
    static void callback(uint gpio, uint32_t event_mask);
};

} // namespace Gpio

} // namespace Task

#endif /* INPUT_HPP */
