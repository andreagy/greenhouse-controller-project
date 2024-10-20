#include "SensorView.hpp"

#include "projdefs.h"

#include <cstddef>
#include <cstdio>

namespace View
{

constexpr size_t STRING_MAX = 32;

SensorView::SensorView(std::shared_ptr<ssd1306os> display,
                       QueueHandle_t inputQueue,
                       QueueHandle_t dataQueue,
                       QueueHandle_t targetQueue) :
    m_Display{display},
    m_InputQueue{inputQueue},
    m_DataQueue{dataQueue},
    m_TargetQueue{targetQueue}
{
    m_TargetText.reserve(STRING_MAX);
    m_Co2Text.reserve(STRING_MAX);
    m_RhText.reserve(STRING_MAX);
    m_TempText.reserve(STRING_MAX);
    m_PaText.reserve(STRING_MAX);
}

void SensorView::display()
{
    constexpr uint8_t FONT_SIZE = 8;
    constexpr uint8_t MAX_LINE_CHARS = 16;

    m_Exit = false;

    while (!m_Exit)
    {
        xQueuePeek(m_DataQueue, &m_SensorData, portMAX_DELAY);
        handleInput();

        m_TargetText = "Set CO2: ";
        m_TargetNumberText = std::to_string(m_Co2Target);

        m_Display->fill(0); // Clear the display
        m_Display->text(m_TargetText, 0, 0);
        if (m_Co2SetEnabled)
        {
            m_Display->rect((MAX_LINE_CHARS - m_TargetNumberText.size() - 3) * FONT_SIZE,
                            0,
                            m_TargetNumberText.size() * FONT_SIZE,
                            FONT_SIZE,
                            1,
                            true);
            m_Display->text(m_TargetNumberText,
                            (MAX_LINE_CHARS - m_TargetNumberText.size() - 3) * FONT_SIZE,
                            0,
                            0);
        }
        else
        {
            xQueuePeek(m_TargetQueue, &m_Co2Target, 0);
            m_Display->text(m_TargetNumberText,
                            (MAX_LINE_CHARS - m_TargetNumberText.size() - 3) * FONT_SIZE,
                            0);
        }
        m_Display->text("ppm", (MAX_LINE_CHARS - 3) * FONT_SIZE, 0);

        snprintf(m_Co2Text.data(), STRING_MAX, "CO2: %8.2fppm", m_SensorData.co2);
        snprintf(m_RhText.data(), STRING_MAX, "Humidity: %5.1f%%", m_SensorData.rh);
        snprintf(m_TempText.data(), STRING_MAX, "Temp: %9.1fC", m_SensorData.temp);
        snprintf(m_PaText.data(), STRING_MAX, "Pressure: %4dPa", m_SensorData.pa);
        m_Display->text(m_Co2Text, 0, 20);
        m_Display->text(m_RhText, 0, 30);
        m_Display->text(m_TempText, 0, 40);
        m_Display->text(m_PaText, 0, 50);

        m_Display->show();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void SensorView::handleInput()
{
    if (xQueueReceive(m_InputQueue, &m_InputPin, 0) == pdPASS) // Non-blocking receive
    {
        if (m_Co2SetEnabled)
        {
            switch (m_InputPin)
            {
                case Gpio::ROT_A:
                    if ((m_Co2Target + 10) <= 1500)
                    {
                        m_Co2Target += 10; // Increment CO2 level
                    }
                    else { m_Co2Target = 1500; }
                    break;
                case Gpio::ROT_B:
                    if ((m_Co2Target - 10) >= 0)
                    {
                        m_Co2Target -= 10; // Decrement CO2 level
                    }
                    else { m_Co2Target = 0; }
                    break;
                case Gpio::ROT_SW:
                    xQueueOverwrite(m_TargetQueue, &m_Co2Target);
                    m_Co2SetEnabled = false;
                    break;
                case Gpio::SW0:
                    m_Co2SetEnabled = false;
                    m_Exit = true;
                case Gpio::SW1:
                case Gpio::SW2:
                default:
                    break;
            }
        }
        else if (m_InputPin == Gpio::ROT_SW) { m_Co2SetEnabled = true; }
        else if (m_InputPin == Gpio::SW0) { m_Exit = true; }
    }
}

} // namespace View