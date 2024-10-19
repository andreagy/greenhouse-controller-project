#ifndef REGISTER32BIT_HPP
#define REGISTER32BIT_HPP

#include <cstdint>

namespace Sensor
{

// Used for "type punning" the 32-bit unsigned integer to a floating point number.
union Register32bit
{
    uint32_t u;
    float f;
};

// Converts 2x 16-bit unsigned integers into a floating point number using a union.
static float convertToFloat(uint16_t values[2])
{
    constexpr uint8_t LSB = 0;
    constexpr uint8_t MSB = 1;

    Register32bit tmp{static_cast<uint32_t>(values[LSB] | (values[MSB] << 16))};
    return tmp.f;
}

} // namespace Sensor

#endif /* REGISTER32BIT_HPP */
