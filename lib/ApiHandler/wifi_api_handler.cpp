#include "wifi_api_handler.h"
#include <time.h>

WiFiApiHandler::WiFiApiHandler(const char* url, const char* device_id, unsigned long timeout_ms) {
    apiUrl = String(url);
    deviceId = String(device_id);
    timeout = timeout_ms;
}

void WiFiApiHandler::setupSDCard() {
    Serial.println("💾 Setting up SD Card for WiFi API Handler...");
    initSdCard();
}

bool WiFiApiHandler::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiApiHandler::checkConnection() {
    if (!isConnected()) {
        Serial.println("❌ WiFi not connected");
        return false;
    }
    
    Serial.print("✅ WiFi connected - IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("📶 RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    return true;
}

String WiFiApiHandler::getISOTimestamp() {
    char timestamp[32];
    
    // Try to use NTP time if WiFi connected
    if (isConnected()) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            // Format ISO 8601 with timezone +07:00 (WIB)
            sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d+07:00", 
                    timeinfo.tm_year + 1900, 
                    timeinfo.tm_mon + 1, 
                    timeinfo.tm_mday,
                    timeinfo.tm_hour, 
                    timeinfo.tm_min, 
                    timeinfo.tm_sec);
            return String(timestamp);
        }
    }
    
    // Fallback: use millis() as timestamp
    unsigned long currentMillis = millis();
    sprintf(timestamp, "2024-01-01T%02d:%02d:%02d+07:00", 
            (int)((currentMillis / 3600000) % 24),
            (int)((currentMillis / 60000) % 60), 
            (int)((currentMillis / 1000) % 60));
    return String(timestamp);
}

String WiFiApiHandler::createJsonPayload(float d1, float d2, float lat, float lon, float depth) {
    DynamicJsonDocument doc(1024);
    
    doc["device_id"] = deviceId;
    doc["data"]["distance1"] = round(d1 * 10) / 10.0;  // Round to 1 decimal
    doc["data"]["distance2"] = round(d2 * 10) / 10.0;
    doc["data"]["latitude"] = lat;
    doc["data"]["longitude"] = lon;
    doc["timestamp"] = getISOTimestamp();
    
    String payload;
    serializeJsonPretty(doc, payload);
    return payload;
}

bool WiFiApiHandler::sendSensorData(float distance1, float distance2, float latitude, float longitude, float depth) {
    if (!isConnected()) {
        Serial.println("❌ WiFi not connected - cannot send data");
        return false;
    }
    
    Serial.println("");
    Serial.println("🚀 SENDING DATA TO API VIA WiFi");
    Serial.println("========================================");
    Serial.print("Distance1: "); Serial.print(distance1); Serial.println(" cm");
    Serial.print("Distance2: "); Serial.print(distance2); Serial.println(" cm");
    Serial.print("Latitude: "); Serial.println(latitude, 6);
    Serial.print("Longitude: "); Serial.println(longitude, 6);
    Serial.print("Depth: "); Serial.print(depth); Serial.println(" cm");
    Serial.println("========================================");
    
    // Create JSON payload
    String payload = createJsonPayload(distance1, distance2, latitude, longitude, depth);
    
    Serial.println("📋 JSON Payload:");
    Serial.println(payload);
    Serial.println("========================================");
    
    // Send HTTP POST request
    HTTPClient http;
    http.setTimeout(timeout);
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP32-VAT-Monitor/1.0");
    
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("✅ HTTP Response Code: ");
        Serial.println(httpResponseCode);
        
        if (httpResponseCode == 200 || httpResponseCode == 201) {
            Serial.println("✅ Data sent successfully!");
            Serial.print("📥 Response: ");
            Serial.println(response);
            
            // Save to SD card for backup/logging
            saveToOfflineQueue(distance1, distance2, latitude, longitude, depth);
            
            http.end();
            return true;
        } else {
            Serial.print("⚠️ API returned error code: ");
            Serial.println(httpResponseCode);
            Serial.print("📥 Response: ");
            Serial.println(response);
            
            // Save to offline queue for later sync
            saveToOfflineQueue(distance1, distance2, latitude, longitude, depth);
            Serial.println("💾 Data saved to offline queue for later sync");
        }
    } else {
        Serial.print("❌ HTTP Error: ");
        Serial.println(httpResponseCode);
        
        // Save to offline queue when network fails (critical backup!)
        saveToOfflineQueue(distance1, distance2, latitude, longitude, depth);
        Serial.println("💾 Data saved to offline queue due to network failure");
        
        // Print more detailed error info
        switch(httpResponseCode) {
            case HTTPC_ERROR_CONNECTION_REFUSED:
                Serial.println("Connection refused");
                break;
            case HTTPC_ERROR_SEND_HEADER_FAILED:
                Serial.println("Send header failed");
                break;
            case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
                Serial.println("Send payload failed");
                break;
            case HTTPC_ERROR_NOT_CONNECTED:
                Serial.println("Not connected");
                break;
            case HTTPC_ERROR_CONNECTION_LOST:
                Serial.println("Connection lost");
                break;
            case HTTPC_ERROR_READ_TIMEOUT:
                Serial.println("Read timeout");
                break;
        }
    }
    
    http.end();
    return false;
}

bool WiFiApiHandler::sendTestData() {
    Serial.println("\n🧪 WiFi API TEST:");
    
    // Test data
    float test_d1 = 23.5;
    float test_d2 = 32.1;
    float test_lat = -6.175392;
    float test_lon = 106.827153;
    float test_depth = 0;
    
    return sendSensorData(test_d1, test_d2, test_lat, test_lon, test_depth);
}

void WiFiApiHandler::printStatus() {
    Serial.println("\n📡 WiFi API Handler Status:");
    Serial.print("API URL: ");
    Serial.println(apiUrl);
    Serial.print("Device ID: ");
    Serial.println(deviceId);
    Serial.print("Timeout: ");
    Serial.print(timeout);
    Serial.println(" ms");
    Serial.print("WiFi Status: ");
    Serial.println(isConnected() ? "Connected ✅" : "Disconnected ❌");
    
    if (isConnected()) {
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal Strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    }
}

bool WiFiApiHandler::testApiConnection() {
    if (!isConnected()) {
        Serial.println("❌ WiFi not connected - cannot test API");
        return false;
    }
    
    Serial.println("\n🔍 Testing API Connection...");
    
    HTTPClient http;
    http.setTimeout(5000);  // Shorter timeout for test
    http.begin(apiUrl);
    
    int httpResponseCode = http.GET();  // Simple GET to test connectivity
    
    if (httpResponseCode > 0) {
        Serial.print("✅ API Server reachable - Response: ");
        Serial.println(httpResponseCode);
        http.end();
        return true;
    } else {
        Serial.print("❌ API Server unreachable - Error: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }
}

bool WiFiApiHandler::syncOfflineData() {
    if (!isConnected()) {
        Serial.println("❌ WiFi not connected - cannot sync offline data");
        return false;
    }
    
    if (!isOfflineQueueNotEmpty()) {
        Serial.println("📝 No offline data to sync");
        return true;
    }
    
    Serial.println("🔄 Syncing offline data...");
    int syncCount = 0;
    int maxSyncAttempts = 10; // Prevent infinite loop
    
    while (isOfflineQueueNotEmpty() && syncCount < maxSyncAttempts) {
        String payload = readNextLineFromQueue();
        if (payload.length() == 0) break;
        
        Serial.print("📤 Syncing payload ");
        Serial.print(syncCount + 1);
        Serial.print(": ");
        Serial.println(payload.substring(0, 50) + "...");
        
        // Send via HTTP
        HTTPClient http;
        http.setTimeout(timeout);
        http.begin(apiUrl);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("User-Agent", "ESP32-VAT-Monitor/1.0-Sync");
        
        int httpResponseCode = http.POST(payload);
        
        if (httpResponseCode == 200 || httpResponseCode == 201) {
            Serial.println("✅ Offline data synced successfully");
            syncCount++;
        } else {
            Serial.print("❌ Sync failed with code: ");
            Serial.println(httpResponseCode);
            revertProgress(payload);
            http.end();
            break;
        }
        
        http.end();
        delay(1000); // Rate limiting
    }
    
    if (syncCount > 0) {
        Serial.print("✅ Successfully synced ");
        Serial.print(syncCount);
        Serial.println(" offline records");
    }
    
    return syncCount > 0;
}

void WiFiApiHandler::saveToOfflineQueue(float distance1, float distance2, float latitude, float longitude, float depth) {
    // Create VatSensorData structure
    VatSensorData data;
    data.isValid = true;
    data.distance1 = distance1;
    data.distance2 = distance2;
    data.latitude = latitude;
    data.longitude = longitude;
    data.depth = depth;
    
    // Get current time (simplified - should use RTC)
    unsigned long currentMillis = millis();
    data.year = 2024;
    data.month = 1;
    data.day = 1;
    data.hour = (currentMillis / 3600000) % 24;
    data.minute = (currentMillis / 60000) % 60;
    data.second = (currentMillis / 1000) % 60;
    data.satellites = 0;
    data.hdop = 0.0;
    
    // Save to daily log and offline queue
    backupCriticalData(data);
}
