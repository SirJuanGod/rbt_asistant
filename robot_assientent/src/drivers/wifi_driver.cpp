#include "drivers/wifi_driver.h"

WifiDriver::WifiDriver()
    : m_mode(WifiMode_t::AP_MODE)
    , m_initialized(false)
{
}

bool WifiDriver::initAP(const char* ssid, const char* passphrase) {
    WiFi.mode(WIFI_MODE_AP);
    bool result = WiFi.softAP(ssid, passphrase);
    if (result) {
        m_mode = WifiMode_t::AP_MODE;
        m_initialized = true;
    }
    return result;
}

bool WifiDriver::initStation(const char* ssid, const char* passphrase) {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, passphrase);
    m_mode = WifiMode_t::STATION_MODE;
    m_initialized = true;
    return true;
}

bool WifiDriver::isConnected() const {
    if (!m_initialized) return false;
    if (m_mode == WifiMode_t::AP_MODE) {
        return (WiFi.softAPgetStationNum() > 0);
    } else {
        return (WiFi.status() == WL_CONNECTED);
    }
}

int8_t WifiDriver::getRssi() const {
    if (m_mode == WifiMode_t::STATION_MODE && WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }
    return 0;
}

String WifiDriver::getIpAddress() const {
    if (m_mode == WifiMode_t::AP_MODE) {
        return WiFi.softAPIP().toString();
    } else {
        return WiFi.localIP().toString();
    }
}
