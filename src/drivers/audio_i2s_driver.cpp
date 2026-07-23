#include "drivers/audio_i2s_driver.h"

AudioI2sDriver::AudioI2sDriver()
    : m_micInitialized(false)
    , m_speakerInitialized(false)
{
}

bool AudioI2sDriver::init() {
    // 1. Microphone INMP441 Configuration (I2S_NUM_0, 16kHz, 16-bit Mono RX)
    i2s_config_t mic_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SystemConfig::Audio::MIC_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 128,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t mic_pins = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = SystemConfig::Pins::MIC_SCK,
        .ws_io_num = SystemConfig::Pins::MIC_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = SystemConfig::Pins::MIC_SD
    };

    esp_err_t err0 = i2s_driver_install(I2S_NUM_0, &mic_config, 0, NULL);
    esp_err_t err1 = i2s_set_pin(I2S_NUM_0, &mic_pins);

    if (err0 == ESP_OK && err1 == ESP_OK) {
        m_micInitialized = true;
    }

    // 2. Speaker MAX98357A Configuration (I2S_NUM_1, 24kHz, 16-bit Mono TX)
    i2s_config_t spk_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SystemConfig::Audio::SPK_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 16,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t spk_pins = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = SystemConfig::Pins::SPK_BCLK,
        .ws_io_num = SystemConfig::Pins::SPK_LRC,
        .data_out_num = SystemConfig::Pins::SPK_DIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t err2 = i2s_driver_install(I2S_NUM_1, &spk_config, 0, NULL);
    esp_err_t err3 = i2s_set_pin(I2S_NUM_1, &spk_pins);

    if (err2 == ESP_OK && err3 == ESP_OK) {
        m_speakerInitialized = true;
    }

    return (m_micInitialized && m_speakerInitialized);
}

size_t AudioI2sDriver::readMicrophone(int16_t* buffer, size_t samples_to_read) {
    if (!m_micInitialized) return 0;
    size_t bytes_read = 0;
    size_t bytes_to_read = samples_to_read * sizeof(int16_t);
    
    i2s_read(I2S_NUM_0, buffer, bytes_to_read, &bytes_read, portMAX_DELAY);
    return bytes_read;
}

size_t AudioI2sDriver::writeSpeaker(const uint8_t* buffer, size_t bytes_to_write) {
    if (!m_speakerInitialized) return 0;
    size_t bytes_written = 0;
    
    i2s_write(I2S_NUM_1, buffer, bytes_to_write, &bytes_written, portMAX_DELAY);
    return bytes_written;
}
