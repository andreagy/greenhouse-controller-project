#ifndef BUTTONHANDLER_HPP
#define BUTTONHANDLER_HPP

#include "FreeRTOS.h"
#include "queue.h"
#include "task/BaseTask.hpp"

namespace GPIO
{

enum buttonPin : uint
{
    SW0_PIN = 9,
    SW1_PIN = 8,
    SW2_PIN = 7
};

class ButtonHandler : public Task::BaseTask
{
  public:
    ButtonHandler(QueueHandle_t queue,
                  uint32_t stackDepth = 256,
                  Task::priority taskPriority = Task::MED);

    static QueueHandle_t buttonQueue;
    void run() override;

  private:
    static void callback(uint gpio, uint32_t event_mask);
};

} // namespace GPIO

#endif /* BUTTONHANDLER_HPP */
