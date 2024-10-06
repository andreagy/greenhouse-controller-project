#ifndef GPIOINPUT_HPP
#define GPIOINPUT_HPP

#include <pico/types.h>

namespace Gpio
{

enum inputPin : uint
{
    SW2 = 7,
    SW1,
    SW0,
    ROT_A,
    ROT_B,
    ROT_SW
};

} // namespace Gpio

#endif /* GPIOINPUT_HPP */
