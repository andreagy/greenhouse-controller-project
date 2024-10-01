//
// Created by Keijo Länsikunnas on 14.2.2024.
//

#include "Register.hpp"

#include "semaphore/Mutex.hpp"

#include <mutex>

namespace Modbus
{

Register::Register(std::shared_ptr<Client> client_,
                   int server_address,
                   int register_address,
                   bool holding_register) :
    client(client_),
    server(server_address),
    reg_addr(register_address),
    hr(holding_register)
{}

uint16_t Register::read()
{
    uint16_t value = 0;
    read(&value, 1);
    return value;
}

void Register::read(uint16_t *values, uint16_t quantity)
{
    std::lock_guard<Semaphore::Mutex> exclusive(s_Access);

    client->set_destination_rtu_address(server);

    if (hr) { client->read_holding_registers(reg_addr, quantity, values); }
    else { client->read_input_registers(reg_addr, quantity, values); }
}

void Register::read(std::vector<uint16_t> &values, uint16_t quantity)
{
    values.reserve(quantity);
    read(values.data(), quantity);
}

void Register::write(uint16_t value)
{
    std::lock_guard<Semaphore::Mutex> exclusive(s_Access);

    // only holding register is writable
    if (hr)
    {
        // With RTU one client handles all devices (servers) on the same bus
        // so we need to set the server address
        client->set_destination_rtu_address(server);
        client->write_single_register(reg_addr, value);
    }
}

void Register::write(const uint16_t *values, uint16_t quantity)
{
    std::lock_guard<Semaphore::Mutex> exclusive(s_Access);

    // only holding register is writable
    if (hr)
    {
        // With RTU one client handles all devices (servers) on the same bus
        // so we need to set the server address
        client->set_destination_rtu_address(server);
        client->write_multiple_registers(reg_addr, quantity, values);
    }
}

void Register::write(const std::vector<uint16_t> values, uint16_t quantity)
{
    write(values.data(), quantity);
}

} // namespace Modbus