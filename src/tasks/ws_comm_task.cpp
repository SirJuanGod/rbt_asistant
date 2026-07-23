#include "tasks/ws_comm_task.h"
#include "config/system_config.h"
#include "core/system_types.h"
#include "core/ipc_handles.h"

static WebSocketsClient* s_wsClient = nullptr;
static AudioRingBuffer* s_ringBuffer = nullptr;

static void webSocketEventHandler(WStype_t type, uint8_t* payload, size_t length) {
    CommandMsg_t cmd;
    memset(&cmd, 0, sizeof(cmd));

    switch (type) {
    case WStype_CONNECTED:
        Serial.println(F("[+] Successfully Linked to Python Brain!"));
        if (s_ringBuffer != nullptr) s_ringBuffer->clear();
        cmd.type = CommandType::CMD_SET_STATE;
        cmd.target_state = NovaState::LISTENING;
        xQueueSend(g_commandQueue, &cmd, 0);
        break;

    case WStype_DISCONNECTED:
        Serial.println(F("[-] Disconnected from Brain."));
        if (s_ringBuffer != nullptr) s_ringBuffer->clear();
        cmd.type = CommandType::CMD_SET_STATE;
        cmd.target_state = NovaState::BOOTING;
        xQueueSend(g_commandQueue, &cmd, 0);
        break;

    case WStype_TEXT: {
        String msg = String((char*)payload).substring(0, length);
        Serial.print(F("[TEXT CMD] "));
        Serial.println(msg);

        if (msg == "CLEAR_AUDIO") {
            if (s_ringBuffer != nullptr) s_ringBuffer->clear();
        } else if (msg == "SLEEP") {
            cmd.type = CommandType::CMD_SLEEP;
            cmd.target_state = NovaState::SLEEPING;
            xQueueSend(g_commandQueue, &cmd, 0);
            if (s_wsClient != nullptr) s_wsClient->disconnect();
        } else if (msg == "RGB_ON") {
            cmd.type = CommandType::CMD_IR_SEND;
            cmd.ir_code = IrCommand::RGB_ON;
            xQueueSend(g_commandQueue, &cmd, 0);
        } else if (msg == "RGB_OFF") {
            cmd.type = CommandType::CMD_IR_SEND;
            cmd.ir_code = IrCommand::RGB_OFF;
            xQueueSend(g_commandQueue, &cmd, 0);
        } else if (msg == "RGB_RED") {
            cmd.type = CommandType::CMD_IR_SEND;
            cmd.ir_code = IrCommand::RGB_RED;
            xQueueSend(g_commandQueue, &cmd, 0);
        } else if (msg == "RGB_GREEN") {
            cmd.type = CommandType::CMD_IR_SEND;
            cmd.ir_code = IrCommand::RGB_GREEN;
            xQueueSend(g_commandQueue, &cmd, 0);
        } else if (msg == "RGB_BLUE") {
            cmd.type = CommandType::CMD_IR_SEND;
            cmd.ir_code = IrCommand::RGB_BLUE;
            xQueueSend(g_commandQueue, &cmd, 0);
        }
        break;
    }

    case WStype_BIN:
        if (length > 0 && s_ringBuffer != nullptr) {
            cmd.type = CommandType::CMD_SET_STATE;
            cmd.target_state = NovaState::SPEAKING;
            xQueueSend(g_commandQueue, &cmd, 0);

            s_ringBuffer->write(payload, length);
        }
        break;
    }
}

static void wsCommTaskFunction(void* pvParameters) {
    if (s_wsClient != nullptr) {
        s_wsClient->begin(
            SystemConfig::Network::WEBSOCKET_SERVER,
            SystemConfig::Network::WEBSOCKET_PORT,
            SystemConfig::Network::WEBSOCKET_PATH
        );
        s_wsClient->onEvent(webSocketEventHandler);
        s_wsClient->setReconnectInterval(5000);
    }

    for (;;) {
        if (s_wsClient != nullptr) {
            s_wsClient->loop();
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

bool startWsCommTask(WebSocketsClient* wsClient, AudioRingBuffer* ringBuffer) {
    s_wsClient = wsClient;
    s_ringBuffer = ringBuffer;

    BaseType_t result = xTaskCreatePinnedToCore(
        wsCommTaskFunction,
        "WsCommTask",
        SystemConfig::Tasks::COMM_STACK_SIZE,
        nullptr,
        SystemConfig::Tasks::COMM_PRIORITY,
        nullptr,
        SystemConfig::Tasks::COMM_CORE_ID
    );

    return (result == pdPASS);
}
