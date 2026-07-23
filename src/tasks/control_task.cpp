#include "tasks/control_task.h"
#include "config/system_config.h"
#include "core/ipc_handles.h"
#include "tasks/speaker_task.h"

static NovaState s_currentNovaState = NovaState::BOOTING;
static NeoPixelDriver* s_neopixelDriver = nullptr;
static IrDriver* s_irDriver = nullptr;

NovaState getCurrentNovaState() {
    NovaState copy = NovaState::BOOTING;
    if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        copy = s_currentNovaState;
        xSemaphoreGive(g_stateMutex);
    }
    return copy;
}

static void updateNovaState(NovaState newState) {
    if (xSemaphoreTake(g_stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        s_currentNovaState = newState;
        xSemaphoreGive(g_stateMutex);
    }
}

static void controlTaskFunction(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(SystemConfig::Tasks::CONTROL_PERIOD_MS);

    CommandMsg_t rxCommand;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // 1. Consume queued commands non-blocking
        if (xQueueReceive(g_commandQueue, &rxCommand, 0) == pdTRUE) {
            if (rxCommand.type == CommandType::CMD_SET_STATE || 
                rxCommand.type == CommandType::CMD_SLEEP || 
                rxCommand.type == CommandType::CMD_VOICE_KEYWORD_DETECTED) {
                updateNovaState(rxCommand.target_state);
            } else if (rxCommand.type == CommandType::CMD_IR_SEND && s_irDriver != nullptr) {
                s_irDriver->sendIrCommand(rxCommand.ir_code);
            }
        }

        // 2. Check speaker playback timeout transition (SPEAKING -> LISTENING)
        NovaState state = getCurrentNovaState();
        if (state == NovaState::SPEAKING) {
            if (!isSpeakerTalking() && (millis() - getLastSpeakerTime() > 500)) {
                updateNovaState(NovaState::LISTENING);
                state = NovaState::LISTENING;
            }
        }

        // 3. Render NeoPixel LED Ring Animation non-blocking
        if (s_neopixelDriver != nullptr) {
            s_neopixelDriver->update(state);
        }
    }
}

bool startControlTask(NeoPixelDriver* neopixelDriver, IrDriver* irDriver) {
    s_neopixelDriver = neopixelDriver;
    s_irDriver = irDriver;

    BaseType_t result = xTaskCreatePinnedToCore(
        controlTaskFunction,
        "ControlTask",
        SystemConfig::Tasks::CONTROL_STACK_SIZE,
        nullptr,
        SystemConfig::Tasks::CONTROL_PRIORITY,
        nullptr,
        SystemConfig::Tasks::CONTROL_CORE_ID
    );

    return (result == pdPASS);
}
