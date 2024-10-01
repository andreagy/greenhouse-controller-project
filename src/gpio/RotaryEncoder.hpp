#ifndef ROTARYENCODER_HPP
#define ROTARYENCODER_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "queue.h"
#include "task/BaseTask.hpp"

namespace GPIO
{

// TODO: add CW/CCW enum for rotation direction, hide pin enum
enum encoderPin : uint
{
    ROT_A = 10,
    ROT_B,
    ROT_SW
};

class RotaryEncoder : public Task::BaseTask
{
  public:
    RotaryEncoder(QueueHandle_t queue,
                  uint32_t stackDepth = 256,
                  Task::priority taskPriority = Task::MED);
    static QueueHandle_t queueHandle;
    void run() override;

  private:
    static void callback(uint gpio, uint32_t event_mask);
};

} // namespace GPIO

#endif /* ROTARYENCODER_HPP */
