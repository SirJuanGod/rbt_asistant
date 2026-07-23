#ifndef AUDIO_I2S_DRIVER_H
#define AUDIO_I2S_DRIVER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include "config/system_config.h"

class AudioI2sDriver {
public:
    AudioI2sDriver();
    ~AudioI2sDriver() = default;

    // Initialize I2S0 (Microphone RX) and I2S1 (Speaker TX)
    bool init();

    // Read raw PCM samples from INMP441 Mic (I2S_NUM_0)
    size_t readMicrophone(int16_t* buffer, size_t samples_to_read);

    // Write raw PCM samples to MAX98357A Speaker (I2S_NUM_1)
    size_t writeSpeaker(const uint8_t* buffer, size_t bytes_to_write);

private:
    bool m_micInitialized;
    bool m_speakerInitialized;
};

#endif // AUDIO_I2S_DRIVER_H
