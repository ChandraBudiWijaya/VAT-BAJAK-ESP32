#ifndef WIFI_API_HANDLER_H
#define WIFI_API_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../SdUtils/sd_utils.h"

class WiFiApiHandler {
private:
    String apiUrl;
    String deviceId;
    unsigned long timeout;
    
public:
    WiFiApiHandler(const char* url, const char* device_id, unsigned long timeout_ms = 20000);
    
    // Setup methods
    void setupSDCard();
    
    // Offline sync methods
    bool syncOfflineData();
    void saveToOfflineQueue(float distance1, float distance2, float latitude, float longitude, float depth);
    
    // Connection management
    bool isConnected();
    bool checkConnection();
    
    // API methods
    bool sendSensorData(float distance1, float distance2, float latitude, float longitude, float depth);
    bool sendTestData();
    
    // Utility methods
    String createJsonPayload(float d1, float d2, float lat, float lon, float depth);
    String getISOTimestamp();
    
    // Debug methods
    void printStatus();
    bool testApiConnection();
};

#endif
