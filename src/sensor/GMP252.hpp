#ifndef GMP252_HPP
#define GMP252_HPP

#include "modbus/Client.hpp"
#include "modbus/Register.hpp"
#include "sensor/BaseSensor.hpp"

#include <memory>

namespace Sensor
{

class GMP252 : public BaseSensor
{
  public:
    GMP252(std::shared_ptr<Modbus::Client> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    void update();

  private:
    float m_Co2;
    float m_Temp;
    Modbus::Register m_Co2Register;
    Modbus::Register m_TempRegister;
};
} // namespace Sensor

#endif /* GMP252_HPP */
