#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Configuration & IPC Headers
#include "config/system_config.h"
#include "core/system_types.h"
#include "core/ipc_handles.h"

// Baremetal HAL Driver Abstractions
#include "drivers/audio_i2s_driver.h"
#include "drivers/neopixel_driver.h"
#include "drivers/ir_driver.h"
#include "drivers/ring_buffer.h"

// FreeRTOS Task Modules
#include "tasks/speaker_task.h"
#include "tasks/mic_voice_task.h"
#include "tasks/ws_comm_task.h"
#include "tasks/control_task.h"
#include "tasks/telemetry_task.h"

// Hardware Driver & Subsystem Instances
static AudioI2sDriver g_audioI2sDriver;
static NeoPixelDriver g_neopixelDriver;
static IrDriver       g_irDriver;
static AudioRingBuffer g_jitterRingBuffer;
static WebSocketsClient g_webSocketClient;

void setup() {
    // 1. Diagnostics Serial Initialization
    Serial.begin(115200);
    Serial.println(F("\n=================================================="));
    Serial.println(F("    NOVA CORE - FREERTOS MODULAR AUDIO & VOICE   "));
    Serial.println(F("=================================================="));

    // 2. Baremetal Hardware Drivers Initialization
    Serial.println(F("[INIT] NeoPixel LED Ring & IR Transmitter..."));
    g_neopixelDriver.init();
    g_irDriver.init();

    Serial.println(F("[INIT] Audio I2S (INMP441 Mic & MAX98357A Speaker)..."));
    if (!g_audioI2sDriver.init()) {
        Serial.println(F("[ERROR] Audio I2S Initialization Failed!"));
    }

    Serial.println(F("[INIT] Thread-Safe Jitter Ring Buffer (32 KB)..."));
    if (!g_jitterRingBuffer.init(SystemConfig::Audio::RING_BUFFER_SIZE)) {
        Serial.println(F("[FATAL] Ring Buffer Allocation Failed!"));
    }

    // 3. Wi-Fi Wireless Connection
    Serial.println(F("[INIT] Connecting to Wi-Fi Network..."));
    WiFi.begin(SystemConfig::Network::DEFAULT_SSID, SystemConfig::Network::DEFAULT_PASS);
    
    // Non-blocking status check during boot animation
    uint32_t wifiStartMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStartMs < 8000)) {
        delay(200);
        Serial.print(F("."));
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print(F("\n[WIFI] Connected! Local IP: "));
        Serial.println(WiFi.localIP());
    } else {
        Serial.println(F("\n[WIFI] No Wi-Fi network connected. Entering Standalone/Offline Voice Mode."));
    }

    // 4. FreeRTOS IPC Primitives Initialization
    Serial.println(F("[INIT] FreeRTOS IPC Queues & Semaphores..."));
    if (!initSystemIPC()) {
        Serial.println(F("[FATAL] IPC Initialization Failed!"));
    }

    // 5. Spawn FreeRTOS Multitasking Threads
    Serial.println(F("[INIT] Spawning FreeRTOS Autonomous Tasks..."));

    if (!startControlTask(&g_neopixelDriver, &g_irDriver)) {
        Serial.println(F("[ERROR] Failed to launch ControlTask!"));
    }

    if (!startSpeakerTask(&g_audioI2sDriver, &g_jitterRingBuffer)) {
        Serial.println(F("[ERROR] Failed to launch SpeakerTask!"));
    }

    if (!startMicVoiceTask(&g_audioI2sDriver, &g_webSocketClient)) {
        Serial.println(F("[ERROR] Failed to launch MicVoiceTask!"));
    }

    if (!startWsCommTask(&g_webSocketClient, &g_jitterRingBuffer)) {
        Serial.println(F("[ERROR] Failed to launch WsCommTask!"));
    }

    if (!startTelemetryTask(&g_webSocketClient, &g_jitterRingBuffer)) {
        Serial.println(F("[ERROR] Failed to launch TelemetryTask!"));
    }

    Serial.println(F("[SUCCESS] Nova Core setup complete. FreeRTOS Scheduler running."));

    // 6. Delete setup task to free resources to FreeRTOS scheduler
    vTaskDelete(NULL);
}

void loop() {
    // Empty: Managed autonomously by FreeRTOS tasks
}