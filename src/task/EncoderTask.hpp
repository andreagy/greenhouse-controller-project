#ifndef ENCODERTASK_HPP
#define ENCODERTASK_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "queue.h"
#include "task/BaseTask.hpp"

// TODO: move encoder as separate class under GPIO namespace, have task create object

namespace Task
{

enum encoderPin : uint
{
    ROT_A = 10,
    ROT_B,
    ROT_SW
};

class EncoderTask : public Task::BaseTask
{
  public:
    EncoderTask(QueueHandle_t queue,
                uint32_t stackDepth = 256,
                Task::priority taskPriority = Task::MED);
    static QueueHandle_t queueHandle;
    void run() override;

  private:
    static void callback(uint gpio, uint32_t event_mask);
};

} // namespace Task

#endif /* ENCODERTASK_HPP */
