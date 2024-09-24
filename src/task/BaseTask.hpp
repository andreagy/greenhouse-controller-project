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

  private:
    const std::string m_Name;
};

} // namespace Task

#endif /* BASETASK_HPP */
