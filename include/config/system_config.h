#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <Arduino.h>

namespace SystemConfig {

// ============================================================================
// HARDWARE PINOUTS (ESP32)
// ============================================================================
namespace Pins {
    // Microphone INMP441 (I2S_NUM_0)
    constexpr gpio_num_t MIC_WS   = GPIO_NUM_5;
    constexpr gpio_num_t MIC_SCK  = GPIO_NUM_18;
    constexpr gpio_num_t MIC_SD   = GPIO_NUM_32;

    // Speaker MAX98357A (I2S_NUM_1)
    constexpr gpio_num_t SPK_LRC  = GPIO_NUM_19;
    constexpr gpio_num_t SPK_BCLK = GPIO_NUM_21;
    constexpr gpio_num_t SPK_DIN  = GPIO_NUM_22;

    // NeoPixel LED Ring (WS2812B)
    constexpr uint8_t LED_PIN     = 4;
    constexpr uint8_t NUM_LEDS    = 12;

    // Infrared (IR) Transmitter
    constexpr uint8_t IR_PIN      = 25;

    // Status LED & User Button
    constexpr uint8_t STATUS_LED  = 2;
    constexpr uint8_t USER_BUTTON = 0;
}

// ============================================================================
// AUDIO CONFIGURATION (I2S)
// ============================================================================
namespace Audio {
    constexpr uint32_t MIC_SAMPLE_RATE    = 16000;
    constexpr uint32_t SPK_SAMPLE_RATE    = 24000;
    constexpr size_t   BUFFER_SAMPLES     = 128;
    constexpr size_t   RING_BUFFER_SIZE   = (32 * 1024); // 32 KB Jitter Buffer
    constexpr size_t   PREBUFFER_BYTES    = 6000;        // 6 KB prebuffer threshold

    // Smart Noise Gate Thresholds
    constexpr int16_t THRESHOLD_IDLE      = 600;
    constexpr int16_t THRESHOLD_TALKING   = 7000;
}

// ============================================================================
// NETWORK & WEBSOCKET SETTINGS
// ============================================================================
namespace Network {
    constexpr char DEFAULT_SSID[]         = "your_wifi_name";
    constexpr char DEFAULT_PASS[]         = "your_wifi_password";
    constexpr char WEBSOCKET_SERVER[]     = "192.168.29.232";
    constexpr uint16_t WEBSOCKET_PORT     = 8000;
    constexpr char WEBSOCKET_PATH[]       = "/esp32";
}

// ============================================================================
// FREERTOS TASKS CONFIGURATION
// ============================================================================
namespace Tasks {
    // Speaker Audio Output Task
    constexpr uint32_t SPEAKER_STACK_SIZE    = 4096;
    constexpr UBaseType_t SPEAKER_PRIORITY   = 4; // High priority for real-time audio
    constexpr BaseType_t SPEAKER_CORE_ID     = 1;

    // Mic & Voice Processing Task
    constexpr uint32_t VOICE_STACK_SIZE      = 6144;
    constexpr UBaseType_t VOICE_PRIORITY     = 3;
    constexpr BaseType_t VOICE_CORE_ID       = 0;

    // WebSocket Wireless Comm Task
    constexpr uint32_t COMM_STACK_SIZE       = 6144;
    constexpr UBaseType_t COMM_PRIORITY      = 2;
    constexpr BaseType_t COMM_CORE_ID        = 0;

    // LED Animation & State Control Task
    constexpr uint32_t CONTROL_STACK_SIZE    = 3072;
    constexpr UBaseType_t CONTROL_PRIORITY   = 1;
    constexpr BaseType_t CONTROL_CORE_ID     = 1;
    constexpr TickType_t CONTROL_PERIOD_MS   = 20; // 50 Hz refresh

    // Telemetry & Diagnostics Task
    constexpr uint32_t TELEMETRY_STACK_SIZE  = 3072;
    constexpr UBaseType_t TELEMETRY_PRIORITY = 1;
    constexpr BaseType_t TELEMETRY_CORE_ID   = 0;
    constexpr TickType_t TELEMETRY_PERIOD_MS = 500; // 2 Hz diagnostics
}

// ============================================================================
// IPC QUEUES
// ============================================================================
namespace IPC {
    constexpr UBaseType_t COMMAND_QUEUE_SIZE  = 16;
    constexpr UBaseType_t TELEMETRY_QUEUE_SIZE= 8;
}

} // namespace SystemConfig

#endif // SYSTEM_CONFIG_H
