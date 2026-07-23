#include "tasks/speaker_task.h"
#include "config/system_config.h"

static AudioI2sDriver* s_i2sDriver = nullptr;
static AudioRingBuffer* s_ringBuffer = nullptr;
static volatile bool s_isTalking = false;
static volatile uint32_t s_lastSpeakerTime = 0;
static volatile bool s_speakerPrimed = false;

bool isSpeakerTalking() {
    return s_isTalking;
}

uint32_t getLastSpeakerTime() {
    return s_lastSpeakerTime;
}

static void speakerTaskFunction(void* pvParameters) {
    uint8_t play_chunk[1024];

    for (;;) {
        if (!s_speakerPrimed) {
            if (s_ringBuffer != nullptr && s_ringBuffer->available() >= SystemConfig::Audio::PREBUFFER_BYTES) {
                s_speakerPrimed = true;
            } else {
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }
        }

        size_t got = (s_ringBuffer != nullptr) ? s_ringBuffer->read(play_chunk, sizeof(play_chunk)) : 0;

        if (got > 0 && s_i2sDriver != nullptr) {
            s_isTalking = true;
            s_lastSpeakerTime = millis();
            s_i2sDriver->writeSpeaker(play_chunk, got);
        } else if (s_i2sDriver != nullptr) {
            memset(play_chunk, 0, sizeof(play_chunk));
            s_i2sDriver->writeSpeaker(play_chunk, sizeof(play_chunk));
            s_speakerPrimed = false;
            s_isTalking = false;
        }
    }
}

bool startSpeakerTask(AudioI2sDriver* i2sDriver, AudioRingBuffer* ringBuffer) {
    s_i2sDriver = i2sDriver;
    s_ringBuffer = ringBuffer;

    BaseType_t result = xTaskCreatePinnedToCore(
        speakerTaskFunction,
        "SpeakerTask",
        SystemConfig::Tasks::SPEAKER_STACK_SIZE,
        nullptr,
        SystemConfig::Tasks::SPEAKER_PRIORITY,
        nullptr,
        SystemConfig::Tasks::SPEAKER_CORE_ID
    );

    return (result == pdPASS);
}
