#include "HMP60.hpp"

#include "Register32bit.hpp"

namespace Sensor
{

constexpr int MODBUS_ADDR = 241;
constexpr int RH_REG_ADDR = 0x0000;
constexpr int TEMP_REG_ADDR = 0x0002;

HMP60::HMP60(std::shared_ptr<Modbus::Client> modbus) :
    m_RhRegister(modbus, MODBUS_ADDR, RH_REG_ADDR),
    m_TempRegister(modbus, MODBUS_ADDR, TEMP_REG_ADDR)
{}

float HMP60::getRelativeHumidity() { return m_Rh; };

float HMP60::getTemperature() { return m_Temp; }

void HMP60::update()
{
    constexpr uint8_t QUANTITY = 2;
    constexpr uint8_t FIRST = 0;
    constexpr uint8_t SECOND = 1;

    uint16_t values[QUANTITY] = {0};
    Register32bit converter = {0};

    m_RhRegister.read(values, QUANTITY);
    converter.u = values[FIRST] | (values[SECOND] << 16);
    m_Rh = converter.f;
    vTaskDelay(pdMS_TO_TICKS(5)); // TODO: figure better delay system

    m_TempRegister.read(values, QUANTITY);
    converter.u = values[FIRST] | (values[SECOND] << 16);
    m_Temp = converter.f;
    vTaskDelay(pdMS_TO_TICKS(5));
}

} // namespace Sensor