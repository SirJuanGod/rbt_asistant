#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <Arduino.h>
#include <WiFi.h>
#include "core/system_types.h"

enum class WifiMode_t : uint8_t {
    AP_MODE = 0,
    STATION_MODE
};

class WifiDriver {
public:
    WifiDriver();
    ~WifiDriver() = default;

    // Non-blocking initialization of WiFi Access Point or Station
    bool initAP(const char* ssid, const char* passphrase);
    bool initStation(const char* ssid, const char* passphrase);

    // Non-blocking check
    bool isConnected() const;
    int8_t getRssi() const;
    String getIpAddress() const;

    WifiMode_t getMode() const { return m_mode; }

private:
    WifiMode_t m_mode;
    bool m_initialized;
};

#endif // WIFI_DRIVER_H
