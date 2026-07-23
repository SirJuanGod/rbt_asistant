#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <Arduino.h>

enum class BlinkPattern : uint8_t {
    OFF = 0,
    ON,
    SLOW_BLINK,    // e.g. IDLE state
    FAST_BLINK,    // e.g. RUNNING state
    HEARTBEAT,     // e.g. Normal operation pulse
    ERROR_PATTERN  // e.g. Error / Fault condition
};

class GpioDriver {
public:
    GpioDriver();
    ~GpioDriver() = default;

    // Initialize hardware pins in baremetal style
    void init(uint8_t statusLedPin, uint8_t buttonPin, uint8_t triggerPin);

    // Non-blocking update routine (Call periodically or inside task tick)
    void update(uint32_t current_time_ms);

    // LED Pattern control
    void setLedPattern(BlinkPattern pattern);
    void toggleLed();
    void setLedState(bool on);

    // Non-blocking button input reading
    bool readButtonDebounced(uint32_t current_time_ms);

    // Baremetal trigger output
    void setTriggerState(bool active);

private:
    uint8_t m_ledPin;
    uint8_t m_buttonPin;
    uint8_t m_triggerPin;

    BlinkPattern m_currentPattern;
    uint32_t m_lastBlinkMs;
    bool m_ledState;
    uint8_t m_heartbeatStep;

    // Button debounce state
    bool m_lastRawButtonState;
    bool m_debouncedButtonState;
    uint32_t m_lastDebounceMs;
};

#endif // GPIO_DRIVER_H
