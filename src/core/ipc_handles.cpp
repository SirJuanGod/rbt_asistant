#include "core/ipc_handles.h"
#include "core/system_types.h"
#include "config/system_config.h"

QueueHandle_t g_commandQueue = nullptr;
QueueHandle_t g_telemetryQueue = nullptr;
EventGroupHandle_t g_systemEventGroup = nullptr;
SemaphoreHandle_t g_stateMutex = nullptr;

bool initSystemIPC() {
    g_commandQueue = xQueueCreate(SystemConfig::IPC::COMMAND_QUEUE_SIZE, sizeof(CommandMsg_t));
    g_telemetryQueue = xQueueCreate(SystemConfig::IPC::TELEMETRY_QUEUE_SIZE, sizeof(TelemetryMsg_t));

    g_systemEventGroup = xEventGroupCreate();
    g_stateMutex = xSemaphoreCreateMutex();

    return (g_commandQueue != nullptr && 
            g_telemetryQueue != nullptr && 
            g_systemEventGroup != nullptr && 
            g_stateMutex != nullptr);
}
