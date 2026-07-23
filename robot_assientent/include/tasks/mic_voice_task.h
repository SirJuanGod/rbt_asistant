#ifndef MIC_VOICE_TASK_H
#define MIC_VOICE_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebSocketsClient.h>
#include "drivers/audio_i2s_driver.h"

bool startMicVoiceTask(AudioI2sDriver* i2sDriver, WebSocketsClient* wsClient);

#endif // MIC_VOICE_TASK_H
