#include "tasks/mic_voice_task.h"
#include "config/system_config.h"
#include "core/system_types.h"
#include "core/ipc_handles.h"
#include "tasks/speaker_task.h"

static AudioI2sDriver* s_i2sDriver = nullptr;
static WebSocketsClient* s_wsClient = nullptr;
static int16_t s_audioBuffer[SystemConfig::Audio::BUFFER_SAMPLES];

// Stub hook for ESP-SR / Offline Espressif Keyword Spotting (WakeNet / MultiNet)
static void processOfflineEspSr(const int16_t* pcm_samples, size_t sample_count) {
    // When operating offline without Wi-Fi/Internet:
    // ESP-SR / MultiNet processes raw 16kHz PCM audio frames locally to detect offline wake words / commands
    int16_t peak = 0;
    for (size_t i = 0; i < sample_count; i++) {
        int16_t abs_val = abs(pcm_samples[i]);
        if (abs_val > peak) peak = abs_val;
    }

    // Offline Voice Trigger Detection (Threshold-based trigger stub ready for ESP-SR model binding)
    if (peak > 12000) {
        CommandMsg_t cmd;
        cmd.type = CommandType::CMD_VOICE_KEYWORD_DETECTED;
        cmd.target_state = NovaState::LISTENING;
        strncpy(cmd.text_payload, "OFFLINE_WAKEWORD", sizeof(cmd.text_payload) - 1);
        xQueueSend(g_commandQueue, &cmd, 0);
    }
}

static void micVoiceTaskFunction(void* pvParameters) {
    for (;;) {
        if (s_i2sDriver == nullptr) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        // 1. Read raw 16kHz PCM audio from INMP441 Microphone (I2S_NUM_0)
        size_t bytes_read = s_i2sDriver->readMicrophone(s_audioBuffer, SystemConfig::Audio::BUFFER_SAMPLES);

        if (bytes_read > 0) {
            bool wsConnected = (s_wsClient != nullptr && s_wsClient->isConnected());
            
            // 2. Smart Noise Gate & Peak Amplitude calculation
            int16_t peak_amplitude = 0;
            int sample_count = bytes_read / sizeof(int16_t);

            for (int i = 0; i < sample_count; i++) {
                int16_t abs_sample = abs(s_audioBuffer[i]);
                if (abs_sample > peak_amplitude) {
                    peak_amplitude = abs_sample;
                }
            }

            // 3. Offline Mode Processing (ESP-SR Offline Voice Command Engine)
            if (!wsConnected) {
                processOfflineEspSr(s_audioBuffer, sample_count);
            } else {
                // 4. Online Mode: Stream audio over WebSocket to Brain server
                bool talking = isSpeakerTalking();
                bool should_send_audio = false;

                if (talking) {
                    if (peak_amplitude > SystemConfig::Audio::THRESHOLD_TALKING) {
                        should_send_audio = true;
                    }
                } else {
                    if (peak_amplitude > SystemConfig::Audio::THRESHOLD_IDLE) {
                        should_send_audio = true;
                    }
                }

                if (should_send_audio && s_wsClient != nullptr) {
                    s_wsClient->sendBIN((uint8_t*)s_audioBuffer, bytes_read);
                }
            }
        }
    }
}

bool startMicVoiceTask(AudioI2sDriver* i2sDriver, WebSocketsClient* wsClient) {
    s_i2sDriver = i2sDriver;
    s_wsClient = wsClient;

    BaseType_t result = xTaskCreatePinnedToCore(
        micVoiceTaskFunction,
        "MicVoiceTask",
        SystemConfig::Tasks::VOICE_STACK_SIZE,
        nullptr,
        SystemConfig::Tasks::VOICE_PRIORITY,
        nullptr,
        SystemConfig::Tasks::VOICE_CORE_ID
    );

    return (result == pdPASS);
}
