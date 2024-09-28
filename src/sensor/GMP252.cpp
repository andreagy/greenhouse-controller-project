#include "GMP252.hpp"

#include "Register32bit.hpp"
#include "projdefs.h"

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
    constexpr uint8_t FIRST = 0;
    constexpr uint8_t SECOND = 1;

    uint16_t values[QUANTITY] = {0};
    Register32bit converter = {0};

    m_Co2Register.read(values, QUANTITY);
    converter.u = values[FIRST] | values[SECOND];
    m_Co2 = converter.f;
    vTaskDelay(pdMS_TO_TICKS(5)); // TODO: figure better delay system

    m_TempRegister.read(values, QUANTITY);
    converter.u = values[FIRST] | values[SECOND];
    m_Temp = converter.f;
    vTaskDelay(pdMS_TO_TICKS(5));
}

} // namespace Sensor