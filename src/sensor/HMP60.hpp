#ifndef HMP60_HPP
#define HMP60_HPP

#include "modbus/MbClient.hpp"
#include "modbus/Register.hpp"
#include "sensor/BaseSensor.hpp"

#include <memory>

namespace Sensor
{

class HMP60 : public BaseSensor
{
  public:
    HMP60(std::shared_ptr<Modbus::Client> modbus);
    HMP60(const HMP60 &) = delete;
    float getRh();
    float getTemp();
    void update() override;

  private:
    float m_Rh;
    float m_Temp;
    Modbus::Register m_RhRegister;
    Modbus::Register m_TempRegister;
};

} // namespace Sensor

#endif /* HMP60_HPP */
