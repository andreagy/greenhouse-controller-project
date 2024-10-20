#ifndef NETDATA_HPP
#define NETDATA_HPP

#include <cstdint>

namespace Network
{

struct Data
{
    float co2;
    float rh;
    float temp;
    uint32_t target;
    uint16_t speed;
};

struct Settings
{
    char str1[64];
    char str2[64];
    char id[8];
};

} // namespace Network

#endif /* NETDATA_HPP */
