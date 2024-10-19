#ifndef SENSORDATA_HPP
#define SENSORDATA_HPP

#include <cstdint>

namespace Sensor
{
struct SensorData
{
    float co2;
    float rh;
    float temp;
    int16_t pa;
};

} // namespace Sensor

#endif /* SENSORDATA_HPP */
