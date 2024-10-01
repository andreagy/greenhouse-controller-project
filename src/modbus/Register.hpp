//
// Created by Keijo Länsikunnas on 14.2.2024.
//

#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "Client.hpp"
#include "semaphore/Mutex.hpp"

#include <memory>
#include <vector>

namespace Modbus
{

class Register
{
  public:
    Register(std::shared_ptr<Client> client_,
             int server_address,
             int register_address,
             bool holding_register = true);
    uint16_t read();
    void read(uint16_t *values, uint16_t quantity);
    void read(std::vector<uint16_t> &values, uint16_t quantity);
    void write(uint16_t value);
    void write(const uint16_t *values, uint16_t quantity);
    void write(const std::vector<uint16_t> values, uint16_t quantity);

  private:
    std::shared_ptr<Client> client;
    int server;
    int reg_addr;
    bool hr;
    inline static Semaphore::Mutex s_Access;
};

} // namespace Modbus

#endif /* REGISTER_HPP */
