#include "TestTask.hpp"

#include "projdefs.h"
#include "task.h"
#include "task/BaseTask.hpp"

#include <vector>

TestTask::TestTask(TaskHandle_t taskHandle) :
    BaseTask{"TestTask", 256, this, Task::MED},
    m_Handle{taskHandle}
{}

void TestTask::run()
{
    std::vector<uint16_t> speeds = {250, 500, 750, 1000, 0};
    uint8_t curr = 0;

    while (true)
    {
        xTaskNotify(m_Handle, speeds[curr], eSetValueWithOverwrite);
        curr = (curr + 1) % speeds.size();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
