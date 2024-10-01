#ifndef BASETASK_HPP
#define BASETASK_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

#include <cstdint>
#include <string>

namespace Task
{

enum priority : unsigned int
{
    IDLE,
    LOW,
    MED,
    HIGH,
    TIMER
};

class BaseTask
{
  public:
    BaseTask(const std::string name,
             uint32_t stackDepth,
             void *const thisPtr,
             priority taskPriority);
    virtual ~BaseTask(){};
    virtual void run() = 0;
    TaskHandle_t getHandle();

  private:
    const std::string m_Name;
    TaskHandle_t m_Handle;
    static void runner(void *params);
};

} // namespace Task

#endif /* BASETASK_HPP */
