#ifndef TESTTASK_HPP
#define TESTTASK_HPP

#include "task/BaseTask.hpp"

class TestTask : public Task::BaseTask
{
  public:
    TestTask(TaskHandle_t taskHandle);
    void run() override;

  private:
    TaskHandle_t m_Handle;
};

#endif /* TESTTASK_HPP */
