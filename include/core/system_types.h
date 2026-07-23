#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <Arduino.h>

// ============================================================================
// SYSTEM / NOVA STATE MACHINE ENUMERATION
// ============================================================================
enum class NovaState : uint8_t {
    BOOTING = 0,
    LISTENING,
    THINKING,
    SPEAKING,
    SLEEPING,
    EMERGENCY_STOP,
    ERROR
};

// ============================================================================
// COMMAND TYPES
// ============================================================================
enum class CommandType : uint8_t {
    CMD_NONE = 0,
    CMD_SET_STATE,
    CMD_CLEAR_AUDIO_BUFFER,
    CMD_IR_SEND,
    CMD_SLEEP,
    CMD_WAKEUP,
    CMD_VOICE_KEYWORD_DETECTED
};

// ============================================================================
// IR CODE ENUMERATION
// ============================================================================
enum class IrCommand : uint32_t {
    RGB_ON    = 0xF740BF,
    RGB_OFF   = 0xF7C03F,
    RGB_RED   = 0xF720DF,
    RGB_GREEN = 0xF7A05F,
    RGB_BLUE  = 0xF7609F
};

// ============================================================================
// SYSTEM COMMAND MESSAGE STRUCTURE
// ============================================================================
struct CommandMsg_t {
    CommandType type;
    NovaState target_state;
    IrCommand ir_code;
    char text_payload[64];
};

// ============================================================================
// TELEMETRY PAYLOAD STRUCTURE
// ============================================================================
struct TelemetryMsg_t {
    uint32_t timestamp_ms;
    NovaState current_state;
    size_t ring_buffer_available;
    uint32_t free_heap_bytes;
    bool wifi_connected;
    bool websocket_connected;
};

#endif // SYSTEM_TYPES_H
