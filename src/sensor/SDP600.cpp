#include "SDP600.hpp"

#include <cstdint>
#include <vector>

namespace Sensor
{

constexpr uint8_t I2C_ADDR = 0x40;

union typePun
{
    uint16_t u;
    int16_t i;
};

SDP600::SDP600(std::shared_ptr<I2c::PicoI2C> picoI2c) : m_I2c{picoI2c} {}

void SDP600::update()
{
    constexpr uint WRITE_LEN = 1;
    constexpr uint READ_LEN = 2;

    std::vector<uint8_t> buffer = {0xF1, 0x00};
    typePun temp;

    uint result = m_I2c->transaction(I2C_ADDR, buffer.data(), WRITE_LEN, buffer.data(), READ_LEN);
    temp.u = (buffer[0] << 8) | buffer[1];

    if (result == WRITE_LEN + READ_LEN) { m_Pressure = temp.i / 240; }
}

int16_t SDP600::getPressure() { return m_Pressure; }

} // namespace Sensor