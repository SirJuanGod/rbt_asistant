#include "drivers/neopixel_driver.h"

NeoPixelDriver::NeoPixelDriver()
    : m_strip(SystemConfig::Pins::NUM_LEDS, SystemConfig::Pins::LED_PIN, NEO_GRB + NEO_KHZ800)
    , m_pulseBrightness(20)
    , m_pulseDirection(5)
{
}

void NeoPixelDriver::init() {
    m_strip.begin();
    m_strip.setBrightness(40);
    setAllPixels(0, 0, 255); // Booting Blue
}

void NeoPixelDriver::setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t i = 0; i < m_strip.numPixels(); i++) {
        m_strip.setPixelColor(i, m_strip.Color(r, g, b));
    }
    m_strip.show();
}

void NeoPixelDriver::update(NovaState currentState) {
    switch (currentState) {
    case NovaState::LISTENING:
        setAllPixels(0, 255, 0); // Solid Green
        break;

    case NovaState::THINKING:
        setAllPixels(255, 255, 0); // Solid Yellow
        break;

    case NovaState::SPEAKING:
        m_pulseBrightness += m_pulseDirection;
        if (m_pulseBrightness >= 200) {
            m_pulseBrightness = 200;
            m_pulseDirection = -5;
        }
        if (m_pulseBrightness <= 20) {
            m_pulseBrightness = 20;
            m_pulseDirection = 5;
        }
        setAllPixels(m_pulseBrightness / 2, 0, m_pulseBrightness); // Pulsing Purple
        break;

    case NovaState::SLEEPING:
        m_strip.clear();
        m_strip.show();
        break;

    default:
        setAllPixels(0, 0, 255); // Solid Blue
        break;
    }
}
