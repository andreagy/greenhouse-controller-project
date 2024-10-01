#ifndef CLIENT_HPP
#define CLIENT_HPP

//
// Created by Keijo Länsikunnas on 14.2.2024.
//

#include "FreeRTOS.h" // IWYU pragma: keep
#include "nanomodbus/nanomodbus.h"
#include "timer/Timeout.hpp"
#include "uart/PicoOsUart.hpp"

#include <memory>

namespace Modbus
{

// Wrapper class does not implement full nanomodbus API
// addresses are wire addresses (numbering starts from zero)
class Client
{
  public:
    explicit Client(std::shared_ptr<Uart::PicoOsUart> uart_);
    void set_destination_rtu_address(uint8_t address);
    nmbs_error read_coils(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out);
    nmbs_error read_discrete_inputs(uint16_t address,
                                    uint16_t quantity,
                                    nmbs_bitfield inputs_out);
    nmbs_error read_holding_registers(uint16_t address,
                                      uint16_t quantity,
                                      uint16_t *registers_out);
    nmbs_error read_input_registers(uint16_t address,
                                    uint16_t quantity,
                                    uint16_t *registers_out);
    nmbs_error write_single_coil(uint16_t address, bool value);
    nmbs_error write_single_register(uint16_t address, uint16_t value);
    nmbs_error write_multiple_coils(uint16_t address,
                                    uint16_t quantity,
                                    const nmbs_bitfield coils);
    nmbs_error write_multiple_registers(uint16_t address,
                                        uint16_t quantity,
                                        const uint16_t *registers);

  private:
    static int32_t uart_transport_write(const uint8_t *buf,
                                        uint16_t count,
                                        int32_t byte_timeout_ms,
                                        void *arg);
    static int32_t uart_transport_read(uint8_t *buf,
                                       uint16_t count,
                                       int32_t byte_timeout_ms,
                                       void *arg);

    std::shared_ptr<Uart::PicoOsUart> uart;
    nmbs_platform_conf platform_conf;
    nmbs_t nmbs;
    inline static Timer::Timeout s_RequestDelay{5};
};

} // namespace Modbus

#endif /* CLIENT_HPP */
