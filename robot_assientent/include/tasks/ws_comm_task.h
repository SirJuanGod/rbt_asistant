#ifndef WS_COMM_TASK_H
#define WS_COMM_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebSocketsClient.h>
#include "drivers/ring_buffer.h"

bool startWsCommTask(WebSocketsClient* wsClient, AudioRingBuffer* ringBuffer);

#endif // WS_COMM_TASK_H
