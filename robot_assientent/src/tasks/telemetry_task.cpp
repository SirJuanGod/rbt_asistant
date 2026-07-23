#include "tasks/telemetry_task.h"
#include <WiFi.h>
#include "config/system_config.h"
#include "core/system_types.h"
#include "core/ipc_handles.h"
#include "tasks/control_task.h"

static WebSocketsClient* s_wsClient = nullptr;
static AudioRingBuffer* s_ringBuffer = nullptr;

static void telemetryTaskFunction(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(SystemConfig::Tasks::TELEMETRY_PERIOD_MS);

    TelemetryMsg_t telemetryData;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        telemetryData.timestamp_ms = millis();
        telemetryData.current_state = getCurrentNovaState();
        telemetryData.free_heap_bytes = ESP.getFreeHeap();
        telemetryData.wifi_connected = (WiFi.status() == WL_CONNECTED);
        telemetryData.websocket_connected = (s_wsClient != nullptr && s_wsClient->isConnected());
        telemetryData.ring_buffer_available = (s_ringBuffer != nullptr) ? s_ringBuffer->available() : 0;

        xQueueOverwrite(g_telemetryQueue, &telemetryData);
    }
}

bool startTelemetryTask(WebSocketsClient* wsClient, AudioRingBuffer* ringBuffer) {
    s_wsClient = wsClient;
    s_ringBuffer = ringBuffer;

    BaseType_t result = xTaskCreatePinnedToCore(
        telemetryTaskFunction,
        "TelemetryTask",
        SystemConfig::Tasks::TELEMETRY_STACK_SIZE,
        nullptr,
        SystemConfig::Tasks::TELEMETRY_PRIORITY,
        nullptr,
        SystemConfig::Tasks::TELEMETRY_CORE_ID
    );

    return (result == pdPASS);
}
