#include <Arduino.h>
#include <TinyGPS++.h>
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../lib/ApiHandler/wifi_api_handler.h"
#include "../include/config.h"

// Forward declarations
bool sendDataToAPI(float d1, float d2, float lat, float lon, float depth);
String getISOTimestamp();
void setupWiFi();
void handleWiFiReconnection();

// Flag untuk mengontrol sensor mode  
bool USE_REAL_SENSORS = true;   // ENABLED untuk testing sensor hardware
bool SENSORS_INITIALIZED = false;
bool WIFI_CONNECTED = false;

// Timing - lebih sering untuk sensor testing
unsigned long lastApiPost = 0;
const unsigned long API_POST_INTERVAL = 60000; // 60 detik untuk API (lebih jarang)
const unsigned long SENSOR_READ_INTERVAL = 1000; // 1 detik untuk sensor (lebih sering)

// Display timer
unsigned long displayTimer = 0;
unsigned long sensorTimer = 0;
unsigned long wifiCheckTimer = 0;
int displayCount = 0;
int sensorReadCount = 0;

// Fungsi untuk membuat ISO timestamp
String getISOTimestamp() {
    char timestamp[32];
    
    // Coba gunakan NTP time jika WiFi connected
    if (WiFi.status() == WL_CONNECTED) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            // Format ISO 8601 dengan timezone +07:00 (WIB)
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
    
    // Fallback: gunakan millis() sebagai timestamp
    unsigned long currentMillis = millis();
    // Konversi ke detik dan format sebagai Unix timestamp
    sprintf(timestamp, "2024-01-01T%02d:%02d:%02d+07:00", 
            (int)((currentMillis / 3600000) % 24),
            (int)((currentMillis / 60000) % 60), 
            (int)((currentMillis / 1000) % 60));
    return String(timestamp);
}

void setupWiFi() {
    Serial.println("📡 Connecting to WiFi...");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long wifiStart = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        WIFI_CONNECTED = true;
        Serial.println("");
        Serial.println("✅ WiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        // Setup NTP untuk mendapatkan waktu yang akurat
        Serial.println("🕐 Setting up NTP time...");
        configTime(TIMEZONE_OFFSET, 0, NTP_SERVER1, NTP_SERVER2);
        
        // Tunggu sampai NTP sync
        struct tm timeinfo;
        int ntpRetries = 0;
        while (!getLocalTime(&timeinfo) && ntpRetries < 10) {
            delay(1000);
            Serial.print(".");
            ntpRetries++;
        }
        
        if (getLocalTime(&timeinfo)) {
            Serial.println("");
            Serial.println("✅ NTP time synchronized!");
            Serial.print("Current time: ");
            Serial.println(getISOTimestamp());
        } else {
            Serial.println("");
            Serial.println("⚠️ NTP sync failed - using fallback timestamp");
        }
    } else {
        WIFI_CONNECTED = false;
        Serial.println("");
        Serial.println("❌ WiFi connection failed - will retry later");
    }
}

void handleWiFiReconnection() {
    if (millis() - wifiCheckTimer >= WIFI_RETRY_INTERVAL) {
        wifiCheckTimer = millis();
        
        if (WiFi.status() != WL_CONNECTED && WIFI_CONNECTED) {
            Serial.println("❌ WiFi disconnected - attempting reconnection...");
            WIFI_CONNECTED = false;
        }
        
        if (!WIFI_CONNECTED) {
            setupWiFi();
        }
    }
}

bool sendDataToAPI(float d1, float d2, float lat, float lon, float depth) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi not connected - cannot send data");
        return false;
    }
    
    Serial.println("");
    Serial.println("🚀 SENDING DATA TO API");
    Serial.println("========================================");
    Serial.print("Distance1: "); Serial.print(d1); Serial.println(" cm");
    Serial.print("Distance2: "); Serial.print(d2); Serial.println(" cm");
    Serial.print("Latitude: "); Serial.println(lat, 6);
    Serial.print("Longitude: "); Serial.println(lon, 6);
    Serial.print("Depth: "); Serial.print(depth); Serial.println(" cm");
    Serial.println("========================================");
    
    // Membuat HTTP request
    HTTPClient http;
    http.setTimeout(20000); // 20 second timeout
    http.begin(API_URL);
    http.addHeader("Content-Type", "application/json");
    
    // Membuat JSON payload dengan format yang PERSIS SAMA
    String payload = "{\n";
    payload += "  \"device_id\": \"" + String(DEVICE_ID) + "\",\n";
    payload += "  \"data\": {\n";
    payload += "    \"distance1\": " + String(d1, 1) + ",\n";
    payload += "    \"distance2\": " + String(d2, 1) + ",\n";
    payload += "    \"latitude\": " + String(lat, 6) + ",\n";
    payload += "    \"longitude\": " + String(lon, 6) + "\n";
    payload += "  },\n";
    payload += "  \"timestamp\": \"" + getISOTimestamp() + "\"\n";
    payload += "}";
    
    Serial.println("📋 JSON Payload:");
    Serial.println(payload);
    Serial.println("========================================");
    
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("✅ HTTP Response Code: ");
        Serial.println(httpResponseCode);
        Serial.print("Response: ");
        Serial.println(response);
        http.end();
        return true;
    } else {
        Serial.print("❌ HTTP Error: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("");
    Serial.println("========================================");
    Serial.println("ESP32 VAT SUBSOIL MONITOR - WiFi VERSION");
    Serial.println("========================================");
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    Serial.println("========================================");
    
    // WiFi Connection
    setupWiFi();
    
    // Real sensor initialization
    if (USE_REAL_SENSORS) {
        Serial.println("🔧 REAL SENSOR TESTING MODE");
        Serial.println("========================================");
        
        bool setupSuccess = false;
        int retryCount = 0;
        const int maxRetries = 3;
        
        while (!setupSuccess && retryCount < maxRetries) {
            retryCount++;
            Serial.print("🔧 Setup attempt ");
            Serial.print(retryCount);
            Serial.print("/");
            Serial.println(maxRetries);
            
            try {
                setup_sensors();
                setupSuccess = true;
                Serial.println("✅ setup_sensors() SUCCESS!");
            } catch (...) {
                Serial.print("❌ Attempt ");
                Serial.print(retryCount);
                Serial.println(" failed");
                if (retryCount < maxRetries) {
                    Serial.println("⏳ Retrying in 2 seconds...");
                    delay(2000);
                }
            }
        }
        
        if (setupSuccess) {
            SENSORS_INITIALIZED = true;
            Serial.println("✅ REAL SENSORS INITIALIZED!");
            
            // Setup LED indicators
            setup_leds();
            Serial.println("✅ LED indicators ready!");
        } else {
            SENSORS_INITIALIZED = false;
            Serial.println("❌ SENSOR INITIALIZATION FAILED - using dummy data");
        }
    }
    
    Serial.println("========================================");
    Serial.println("✅ Setup completed!");
    Serial.println("💡 Commands: SENSOR, DUMMY, WIFI, TIME, TEST, LED, API");
    Serial.println("========================================");
}

void loop() {
    // Handle WiFi reconnection
    handleWiFiReconnection();
    
    // Handle serial commands
    if (Serial.available()) {
        String command = Serial.readString();
        command.trim();
        command.toUpperCase();
        
        if (command == "WIFI") {
            Serial.println("\n📡 WiFi Status:");
            Serial.print("Status: ");
            Serial.println(WiFi.status() == WL_CONNECTED ? "Connected ✅" : "Disconnected ❌");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                Serial.print("RSSI: ");
                Serial.print(WiFi.RSSI());
                Serial.println(" dBm");
            }
        } else if (command == "API") {
            Serial.println("\n🧪 API TEST:");
            if (WiFi.status() == WL_CONNECTED) {
                float test_d1 = SENSORS_INITIALIZED ? distance1 : 23.5;
                float test_d2 = SENSORS_INITIALIZED ? distance2 : 32.1;
                float test_lat = SENSORS_INITIALIZED && gps.location.isValid() ? gps.location.lat() : -6.175392;
                float test_lon = SENSORS_INITIALIZED && gps.location.isValid() ? gps.location.lng() : 106.827153;
                float test_depth = SENSORS_INITIALIZED ? calculate_depth() : (2.84 * test_d2 - 16.6);
                
                bool apiSuccess = sendDataToAPI(test_d1, test_d2, test_lat, test_lon, test_depth);
                Serial.println(apiSuccess ? "✅ API test successful!" : "❌ API test failed!");
            } else {
                Serial.println("❌ WiFi not connected - cannot test API");
            }
        }
    }
    
    // Read sensors
    if (SENSORS_INITIALIZED && millis() - sensorTimer >= SENSOR_READ_INTERVAL) {
        sensorTimer = millis();
        sensorReadCount++;
        
        try {
            read_ultrasonic_sensors();
            read_gps_data();
        } catch (...) {
            Serial.println("⚠️ Error reading sensors");
        }
    }
    
    // Display data and send to API
    if (millis() - displayTimer >= 5000) {
        displayTimer = millis();
        displayCount++;
        
        Serial.println("");
        Serial.println("========================================");
        Serial.print("📊 VAT SUBSOIL MONITOR - Reading #");
        Serial.println(displayCount);
        Serial.println("========================================");
        
        if (SENSORS_INITIALIZED) {
            // Real sensor data
            Serial.println("📏 REAL SENSOR DATA:");
            Serial.print("  Distance 1: ");
            Serial.print(distance1);
            Serial.println(" cm");
            Serial.print("  Distance 2: ");
            Serial.print(distance2);
            Serial.println(" cm");
            
            float depth = calculate_depth();
            Serial.print("🌊 Calculated Depth: ");
            Serial.print(depth);
            Serial.println(" cm");
            
            update_leds(distance1, distance2);
            
            // GPS Data
            Serial.println("🛰️ GPS MODULE:");
            if (gps.location.isValid()) {
                Serial.print("  Status: FIXED ✅ (");
                Serial.print(gps.satellites.value());
                Serial.println(" satellites)");
                Serial.print("  Latitude: ");
                Serial.println(gps.location.lat(), 6);
                Serial.print("  Longitude: ");
                Serial.println(gps.location.lng(), 6);
            } else {
                Serial.println("  Status: SEARCHING... ⏳");
            }
            
            // Send to API
            if (WiFi.status() == WL_CONNECTED && millis() - lastApiPost >= API_POST_INTERVAL) {
                lastApiPost = millis();
                
                float lat = gps.location.isValid() ? gps.location.lat() : 0.0;
                float lon = gps.location.isValid() ? gps.location.lng() : 0.0;
                
                bool apiSuccess = sendDataToAPI(distance1, distance2, lat, lon, depth);
                Serial.println(apiSuccess ? "✅ DATA POSTED SUCCESSFULLY" : "❌ API POST FAILED");
            }
        } else {
            // Dummy data
            Serial.println("🎭 DUMMY DATA (Sensor Failed):");
            float dummy_d1 = 25.3 + (displayCount * 0.1);
            float dummy_d2 = 28.7 + (displayCount * 0.15);
            Serial.print("   Distance 1: ");
            Serial.print(dummy_d1, 1);
            Serial.println(" cm");
            Serial.print("   Distance 2: ");
            Serial.print(dummy_d2, 1);
            Serial.println(" cm");
        }
        
        // System info
        Serial.print("📡 WiFi: ");
        Serial.println(WiFi.status() == WL_CONNECTED ? "Connected ✅" : "Disconnected ❌");
        Serial.print("🔧 Uptime: ");
        Serial.print(millis()/1000);
        Serial.print("s, Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        Serial.println("========================================");
    }
    
    delay(50);
}
