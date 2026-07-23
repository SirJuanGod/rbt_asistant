#ifndef SPEAKER_TASK_H
#define SPEAKER_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "drivers/audio_i2s_driver.h"
#include "drivers/ring_buffer.h"

bool startSpeakerTask(AudioI2sDriver* i2sDriver, AudioRingBuffer* ringBuffer);
bool isSpeakerTalking();
uint32_t getLastSpeakerTime();

#endif // SPEAKER_TASK_H
