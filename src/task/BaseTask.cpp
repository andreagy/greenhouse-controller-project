#include "BaseTask.hpp"

namespace Task
{

BaseTask::BaseTask(const std::string name,
                   uint32_t stackDepth,
                   void *const thisPtr,
                   priority taskPriority) :
    m_Name{name}
{
    xTaskCreate(runner, m_Name.c_str(), stackDepth, thisPtr, tskIDLE_PRIORITY + taskPriority, NULL);
}

static void runner(void *params)
{
    BaseTask *task = static_cast<BaseTask *>(params);
    task->run();
}

} // namespace Task