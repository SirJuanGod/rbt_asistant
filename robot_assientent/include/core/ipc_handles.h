#ifndef IPC_HANDLES_H
#define IPC_HANDLES_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

// Global IPC handles for inter-task communication
extern QueueHandle_t g_commandQueue;
extern QueueHandle_t g_telemetryQueue;
extern EventGroupHandle_t g_systemEventGroup;
extern SemaphoreHandle_t g_stateMutex;

// Initialization function for FreeRTOS IPC primitives
bool initSystemIPC();

#endif // IPC_HANDLES_H
