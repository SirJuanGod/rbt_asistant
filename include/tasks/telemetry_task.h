#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebSocketsClient.h>
#include "drivers/ring_buffer.h"

bool startTelemetryTask(WebSocketsClient* wsClient, AudioRingBuffer* ringBuffer);

#endif // TELEMETRY_TASK_H
