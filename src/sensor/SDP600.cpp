#include "SDP600.hpp"

namespace Sensor
{

SDP600::SDP600(i2c_inst *i2c, uint SDA_pin, uint SCL_pin, uint8_t devAddr) :
    mSensor_i2c(i2c),
    mDevAddr(devAddr)
{
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
    mPressure.mInt16 = 0;
}

void SDP600::update()
{
    uint8_t buf[2] = {0xF1};
    i2c_write_blocking(mSensor_i2c, mDevAddr, buf, 1, true);
    i2c_read_blocking(mSensor_i2c, mDevAddr, buf, 2, false);
    mPressure.mUint16 = (buf[0] << 8) + (buf[1]);
}

int16_t SDP600::getPressure() const { return mPressure.mInt16; }

} // namespace Sensor