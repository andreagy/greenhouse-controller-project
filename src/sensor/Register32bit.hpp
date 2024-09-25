#ifndef REGISTER32BIT_HPP
#define REGISTER32BIT_HPP

#include <cstdint>

namespace Sensor
{

// Used for type punning the sensor data from integer to float.
union Register32bit
{
    uint32_t u;
    float f;
};

} // namespace Sensor

#endif /* REGISTER32BIT_HPP */
