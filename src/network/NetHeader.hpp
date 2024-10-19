#ifndef NETHEADER_HPP
#define NETHEADER_HPP

#include <string>

namespace Network
{

namespace Request
{

static std::string updateHeader = {"POST /update HTTP/1.1\r\n"
                                   "Host: api.thingspeak.com\r\n"
                                   "Connection: close\r\n"
                                   "User-Agent: PicoW\r\n"
                                   "Accept: */*\r\n"
                                   "Content-Length: "};

static std::string executeHeader = {
    "POST /talkbacks/53284/commands/execute HTTP/1.1\r\n"
    "Host: api.thingspeak.com\r\n"
    "Connection: close\r\n"
    "User-Agent: PicoW\r\n"
    "Accept: */*\r\n"
    "Content-Length: "};

static std::string headerEnd = {
    "\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"};
} // namespace Request

} // namespace Network

#endif /* NETHEADER_HPP */
