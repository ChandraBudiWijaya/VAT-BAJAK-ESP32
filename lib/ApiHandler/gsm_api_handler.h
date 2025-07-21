#ifndef GSM_API_HANDLER_H
#define GSM_API_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <HardwareSerial.h>
#include "../include/config.h"

// Check if TINY_GSM_MODEM_SIM800 is not already defined
#ifndef TINY_GSM_MODEM_SIM800
#define TINY_GSM_MODEM_SIM800
#endif

class GSMApiHandler {
private:
    TinyGsm* modem;
    TinyGsmClient* client;
    HardwareSerial* gsmSerial;
    String deviceId;
    bool isConnected;
    
    // Production API configuration (Tested & Working)
    const char* server = GSM_SERVER;        // "api-vatsubsoil-dev.ggfsystem.com"
    const char* resource = GSM_RESOURCE;    // "/subsoils"
    const int port = GSM_PORT;              // 443
    const char* apn = GSM_APN;              // "M2MAUTOTRONIC"
    const char* gprsUser = GSM_USER;        // ""
    const char* gprsPass = GSM_PASS;        // ""
    
    // Power management methods
    void powerOnSIM800LManual();
    void testATCommands();
    
public:
    GSMApiHandler(const char* device_id);
    ~GSMApiHandler();
    
    // Initialization and connection
    bool initialize();
    bool connect();
    bool disconnect();
    bool isModemConnected();
    
    // Main API methods
    bool sendSensorData(float distance1, float distance2, float latitude, float longitude, float depth);
    bool sendToProductionAPI(const String& payload);
    
    // Test methods
    bool sendHTTPTestRequest(const String& payload);
    bool sendHTTPSTestRequest(const String& payload);
    bool sendHTTPSATRequest(const String& payload);
    bool sendHTTPRequest(const String& payload);
    
    // Utility methods
    String createProductionJsonPayload(float d1, float d2, float lat, float lon, float depth);
    String getTimestamp();
    String getGSMNetworkTime();
    
    // Debug methods
    void printStatus();
    void printNetworkInfo();
};

#endif
