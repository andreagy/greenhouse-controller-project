#include "Eeprom.hpp"

#include "projdefs.h"
#include "semaphore/Mutex.hpp"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <vector>

namespace Storage
{

Eeprom::Eeprom(std::shared_ptr<I2c::PicoI2C> i2c) : m_I2c{i2c} { initCrc(); }

bool Eeprom::write(eepromAddress address, const std::vector<uint8_t> &buffer)
{
    std::lock_guard<Semaphore::Mutex> exclusive(access);
    bool result = false;
    uint count = 0;
    std::vector<uint8_t> tempBuffer;
    uint16_t crc = calcCrc(buffer);

    tempBuffer.push_back(static_cast<uint8_t>(address >> 8));
    tempBuffer.push_back(static_cast<uint8_t>(address));
    tempBuffer.insert(tempBuffer.end(), buffer.begin(), buffer.end());
    tempBuffer.push_back('\0'); // Add an end marker to find the end of the string on read

    eepromWait();

    count = m_I2c->write(m_DeviceAddress, tempBuffer.data(), tempBuffer.size());
    result = count == tempBuffer.size() && checkCrc(buffer, crc);

    writeCrc(address, crc);

    return result;
}

// Write data to EEPROM.
bool Eeprom::write(eepromAddress address, const std::string &buffer)
{
    std::vector<uint8_t> temp(buffer.begin(), buffer.end());
    return write(address, temp);
}

int Eeprom::read(eepromAddress address, std::vector<uint8_t> &buffer)
{
    std::lock_guard<Semaphore::Mutex> exclusive(access);

    constexpr uint READ_COUNT = 64;

    bool result = false;
    uint count = 0;
    std::vector<uint8_t> readAddress;
    uint16_t crc = readCrc(address);

    buffer.reserve(READ_COUNT);
    readAddress.push_back(static_cast<uint8_t>(address >> 8));
    readAddress.push_back(static_cast<uint8_t>(address));

    eepromWait();

    count = m_I2c->transaction(m_DeviceAddress, readAddress.data(), 2, buffer.data(), READ_COUNT);

    auto endMark = std::find(buffer.begin(), buffer.end(), '\0'); // Find the end marker
    buffer.erase(endMark, buffer.end()); // Erase all extra data starting from the end marker

    result = count == READ_COUNT + 2 && checkCrc(buffer, crc);

    return result;
}

int Eeprom::read(eepromAddress address, std::string &buffer)
{
    std::vector<uint8_t> temp;

    if (!read(address, temp)) { return false; }

    buffer.insert(buffer.begin(), temp.begin(), temp.end());
    return true;
}

void Eeprom::initCrc()
{
    constexpr uint16_t POLYNOMIAL = 0x1021;
    constexpr uint16_t WIDTH = 8 * sizeof(uint16_t);
    constexpr uint16_t TOP_BIT = 1 << (WIDTH - 1);

    uint16_t remainder = 0;

    for (int i = 0; i < 256; ++i)
    {
        remainder = i << (WIDTH - 8);

        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (remainder & TOP_BIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else { remainder = (remainder << 1); }
        }

        m_CrcTable.push_back(remainder);
    }
}

uint16_t Eeprom::calcCrc(const std::vector<uint8_t> &message)
{
    constexpr uint16_t WIDTH = 8 * sizeof(uint16_t);

    uint8_t data = 0;
    uint16_t remainder = 0xFFFF;

    for (auto &byte : message)
    {
        data = byte ^ (remainder >> (WIDTH - 8));
        remainder = m_CrcTable.at(data) ^ (remainder << 8);
    }

    return remainder;
}

bool Eeprom::checkCrc(const std::vector<uint8_t> &message, uint16_t crc)
{
    uint16_t temp = calcCrc(message);

    return temp == crc;
}

void Eeprom::writeCrc(eepromAddress address, uint16_t crc)
{
    constexpr uint16_t CRC_OFFSET = 0x1000;

    std::vector<uint8_t> tempBuffer;

    tempBuffer.push_back(static_cast<uint8_t>((CRC_OFFSET + address) >> 8));
    tempBuffer.push_back(static_cast<uint8_t>(CRC_OFFSET + address));
    tempBuffer.push_back(static_cast<uint8_t>(crc >> 8));
    tempBuffer.push_back(static_cast<uint8_t>(crc));

    eepromWait();

    m_I2c->write(m_DeviceAddress, tempBuffer.data(), tempBuffer.size());
}

uint16_t Eeprom::readCrc(eepromAddress address)
{
    constexpr uint16_t CRC_OFFSET = 0x1000;

    std::vector<uint8_t> readAddress;
    std::vector<uint8_t> crcValues(2);

    readAddress.push_back(static_cast<uint8_t>((CRC_OFFSET + address) >> 8));
    readAddress.push_back(static_cast<uint8_t>(CRC_OFFSET + address));

    m_I2c->transaction(m_DeviceAddress, readAddress.data(), 2, crcValues.data(), 2);

    return static_cast<uint16_t>(crcValues[0] << 8 | crcValues[1]);
}

// Waits until the EEPROM is readable/writable.
void Eeprom::eepromWait()
{
    std::vector<uint8_t> temp = {0};

    while (m_I2c->read(m_DeviceAddress, temp.data(), 1) != 1)
    {
        // Loop while EEPROM is unreadable.
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

} // namespace Storage