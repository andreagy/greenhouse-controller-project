#ifndef EEPROM_HPP
#define EEPROM_HPP

#include "I2CHandler.h"

#include <memory>
#include <string>

#define EEPROM_REG_NETWORK_ID 0x00
#define EEPROM_REG_NETWORK_PW 0x40
#define EEPROM_REG_BROKER_IP 0x80
#define EEPROM_REG_MODE 0xC0
#define EEPROM_REG_TAR_PRES 0x100
#define EEPROM_REG_TAR_FAN 0x140

namespace Storage
{

class Eeprom
{
  private:
    const int mI2CBusNumber = 0;
    const uint8_t mI2CDeviceAddress = 0x50;
    uint8_t mCurrentReadWriteAddress[2];
    std::shared_ptr<I2CHandler> mI2CHandler;
    void mSetReadWriteAddress(const uint16_t readWriteAddress);
    void mWaitUntilReady();

  public:
    Eeprom(std::shared_ptr<I2CHandler> i2cHandler);
    void write(const uint16_t readWriteAddress, const std::string &writeBuffer);
    std::string read(const uint16_t readWriteAddress);
};

} // namespace Storage

#endif /* EEPROM_HPP */
