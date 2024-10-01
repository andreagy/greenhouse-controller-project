#ifndef SDP600_HPP
#define SDP600_HPP

#include "i2c/PicoI2C.hpp"
#include "sensor/BaseSensor.hpp"
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include <cstdint>
#include <memory>

namespace Sensor
{

class SDP600 : public BaseSensor
{
  public:
    explicit SDP600(std::shared_ptr<I2c::PicoI2C> picoI2c);
    void update() override;
    int16_t getPressure();

  private:
    int16_t m_Pressure = 0;
    std::shared_ptr<I2c::PicoI2C> m_I2c;
};

} // namespace Sensor

#endif /* SDP600_HPP */
