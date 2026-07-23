#include "drivers/gpio_driver.h"

GpioDriver::GpioDriver()
    : m_ledPin(2)
    , m_buttonPin(0)
    , m_triggerPin(4)
    , m_currentPattern(BlinkPattern::OFF)
    , m_lastBlinkMs(0)
    , m_ledState(false)
    , m_heartbeatStep(0)
    , m_lastRawButtonState(true)
    , m_debouncedButtonState(true)
    , m_lastDebounceMs(0)
{
}

void GpioDriver::init(uint8_t statusLedPin, uint8_t buttonPin, uint8_t triggerPin) {
    m_ledPin = statusLedPin;
    m_buttonPin = buttonPin;
    m_triggerPin = triggerPin;

    pinMode(m_ledPin, OUTPUT);
    pinMode(m_buttonPin, INPUT_PULLUP);
    pinMode(m_triggerPin, OUTPUT);

    digitalWrite(m_ledPin, LOW);
    digitalWrite(m_triggerPin, LOW);
}

void GpioDriver::setLedPattern(BlinkPattern pattern) {
    if (m_currentPattern != pattern) {
        m_currentPattern = pattern;
        m_lastBlinkMs = millis();
        m_heartbeatStep = 0;
    }
}

void GpioDriver::toggleLed() {
    m_ledState = !m_ledState;
    digitalWrite(m_ledPin, m_ledState ? HIGH : LOW);
}

void GpioDriver::setLedState(bool on) {
    m_ledState = on;
    digitalWrite(m_ledPin, m_ledState ? HIGH : LOW);
}

void GpioDriver::setTriggerState(bool active) {
    digitalWrite(m_triggerPin, active ? HIGH : LOW);
}

bool GpioDriver::readButtonDebounced(uint32_t current_time_ms) {
    bool rawReading = (digitalRead(m_buttonPin) == LOW); // Active LOW

    if (rawReading != m_lastRawButtonState) {
        m_lastDebounceMs = current_time_ms;
        m_lastRawButtonState = rawReading;
    }

    if ((current_time_ms - m_lastDebounceMs) > 50) { // 50ms debounce
        m_debouncedButtonState = rawReading;
    }

    return m_debouncedButtonState;
}

void GpioDriver::update(uint32_t current_time_ms) {
    switch (m_currentPattern) {
    case BlinkPattern::OFF:
        setLedState(false);
        break;

    case BlinkPattern::ON:
        setLedState(true);
        break;

    case BlinkPattern::SLOW_BLINK: // 500ms toggle
        if (current_time_ms - m_lastBlinkMs >= 500) {
            m_lastBlinkMs = current_time_ms;
            toggleLed();
        }
        break;

    case BlinkPattern::FAST_BLINK: // 100ms toggle
        if (current_time_ms - m_lastBlinkMs >= 100) {
            m_lastBlinkMs = current_time_ms;
            toggleLed();
        }
        break;

    case BlinkPattern::HEARTBEAT: // Double pulse pattern
        if (current_time_ms - m_lastBlinkMs >= 150) {
            m_lastBlinkMs = current_time_ms;
            m_heartbeatStep = (m_heartbeatStep + 1) % 6;
            // Steps: ON (0), OFF (1), ON (2), OFF (3, 4, 5)
            bool state = (m_heartbeatStep == 0 || m_heartbeatStep == 2);
            setLedState(state);
        }
        break;

    case BlinkPattern::ERROR_PATTERN: // Triple quick flash
        if (current_time_ms - m_lastBlinkMs >= 80) {
            m_lastBlinkMs = current_time_ms;
            m_heartbeatStep = (m_heartbeatStep + 1) % 10;
            bool state = (m_heartbeatStep == 0 || m_heartbeatStep == 2 || m_heartbeatStep == 4);
            setLedState(state);
        }
        break;
    }
}
