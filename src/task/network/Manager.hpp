#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "task/BaseTask.hpp"

namespace Task
{

namespace Network
{

class Manager : public BaseTask
{
  public:
    Manager();
    void run() override;

  private:
    std::string m_Ssid;
    std::string m_Password;
    bool m_Connected = false;
    int connect();
};

} // namespace Network

} // namespace Task

#endif /* MANAGER_HPP */
