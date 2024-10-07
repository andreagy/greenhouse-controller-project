#ifndef EEPROM_HPP
#define EEPROM_HPP

#include "i2c/PicoI2C.hpp"
#include "semaphore/Mutex.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Storage
{

enum eepromAddress : uint16_t
{
    CO2_TARGET_ADDR = 0x00,
    API_KEY_ADDR = 0x40,
    TALKBACK_KEY_ADDR = 0x80,
    SSID_ADDR = 0xC0,
    PASSWORD_ADDR = 0x100,
};

class Eeprom
{
  public:
    Eeprom(std::shared_ptr<I2c::PicoI2C> i2c);
    bool write(eepromAddress address, const std::vector<uint8_t> &buffer);
    bool write(eepromAddress address, const std::string &buffer);
    bool read(eepromAddress address, std::vector<uint8_t> &buffer);
    bool read(eepromAddress address, std::string &buffer);

  private:
    const uint8_t m_DeviceAddress = 0x50;
    Semaphore::Mutex access;
    std::shared_ptr<I2c::PicoI2C> m_I2c;
    std::vector<uint16_t> m_CrcTable;
    void initCrc();
    uint16_t calcCrc(const std::vector<uint8_t> &message);
    bool checkCrc(const std::vector<uint8_t> &message, uint16_t crc);
    void writeCrc(eepromAddress address, uint16_t crc);
    uint16_t readCrc(eepromAddress address);
    void eepromWait();
};

} // namespace Storage

#endif /* EEPROM_HPP */
