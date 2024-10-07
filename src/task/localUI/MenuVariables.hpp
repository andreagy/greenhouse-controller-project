#ifndef MENUVARIABLES_HPP
#define MENUVARIABLES_HPP

#include <string>
#include <vector>

namespace Task
{

namespace LocalUI
{

static const std::vector<std::string> menuOptions = {"Sensor Values",
                                                     "Set Network",
                                                     "Set ThingSpeak"};

static const std::vector<char> characterSet =
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
     'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
     'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};

} // namespace LocalUI

} // namespace Task

#endif /* MENUVARIABLES_HPP */
