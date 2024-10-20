#ifndef SENSORVIEW_HPP
#define SENSORVIEW_HPP

#include "FreeRTOS.h" // IWYU pragma: keep
#include "display/ssd1306os.h"
#include "gpio/GpioInput.hpp"
#include "queue.h"
#include "sensor/SensorData.hpp"

#include <memory>
#include <string>

namespace View
{

class SensorView
{
  public:
    SensorView(std::shared_ptr<ssd1306os> display,
               QueueHandle_t inputQueue,
               QueueHandle_t dataQueue,
               QueueHandle_t targetQueue);
    void display();

  private:
    bool m_Exit = false;
    bool m_Co2SetEnabled = false;
    uint32_t m_Co2Target = 0;
    Gpio::inputPin m_InputPin;
    Sensor::SensorData m_SensorData;
    std::string m_TargetText;
    std::string m_TargetNumberText;
    std::string m_Co2Text;
    std::string m_RhText;
    std::string m_TempText;
    std::string m_PaText;
    std::shared_ptr<ssd1306os> m_Display;
    QueueHandle_t m_InputQueue;
    QueueHandle_t m_DataQueue;
    QueueHandle_t m_TargetQueue;
    void handleInput();
};

} // namespace View

#endif /* SENSORVIEW_HPP */
