#ifndef BASESENSOR_HPP
#define BASESENSOR_HPP

namespace Sensor
{

class BaseSensor
{
  public:
    BaseSensor(){};
    virtual ~BaseSensor(){};
    virtual void update() = 0;
};

} // namespace Sensor

#endif /* BASESENSOR_HPP */
