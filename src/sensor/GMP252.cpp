#include "GMP252.hpp"

#include "Register32bit.hpp"

#include <cstdint>

namespace Sensor
{

constexpr int MODBUS_ADDR = 240;
constexpr int CO2_REG_ADDR = 0x0000;
constexpr int TEMP_REG_ADDR = 0x0004;

GMP252::GMP252(std::shared_ptr<Modbus::Client> modbusClient) :
    m_Co2Register(modbusClient, MODBUS_ADDR, CO2_REG_ADDR),
    m_TempRegister(modbusClient, MODBUS_ADDR, TEMP_REG_ADDR)

{}

// Returns CO2 value in PPM.
float GMP252::getCo2() { return m_Co2; }

// Returns temperature value in C.
float GMP252::getTemp() { return m_Temp; }

// Update sensor values.
void GMP252::update()
{
    constexpr uint8_t QUANTITY = 2;
    uint16_t values[QUANTITY] = {0};

    m_Co2Register.read(values, QUANTITY);
    m_Co2 = convertToFloat(values);
    m_TempRegister.read(values, QUANTITY);
    m_Temp = convertToFloat(values);
}

} // namespace Sensor