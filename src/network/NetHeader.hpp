#ifndef NETHEADER_HPP
#define NETHEADER_HPP

#include <string>

namespace Network
{

namespace Request
{

static std::string headerPrefix = {"Host: api.thingspeak.com\r\n"
                                   "Connection: close\r\n"
                                   "User-Agent: PicoW\r\n"
                                   "Accept: */*\r\n"
                                   "Content-Length: "};

static std::string headerSuffix = {
    "\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"};
} // namespace Request

} // namespace Network

#endif /* NETHEADER_HPP */
