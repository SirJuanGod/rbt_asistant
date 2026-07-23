#ifndef NEOPIXEL_DRIVER_H
#define NEOPIXEL_DRIVER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config/system_config.h"
#include "core/system_types.h"

class NeoPixelDriver {
public:
    NeoPixelDriver();
    ~NeoPixelDriver() = default;

    void init();
    void update(NovaState currentState);
    void setAllPixels(uint8_t r, uint8_t g, uint8_t b);

private:
    Adafruit_NeoPixel m_strip;
    int m_pulseBrightness;
    int m_pulseDirection;
};

#endif // NEOPIXEL_DRIVER_H
