/*
 * EXAMPLE: WiFi API Handler dengan SD Card Backup
 * 
 * Contoh penggunaan WiFiApiHandler class yang sudah include SD card logging
 * File ini menunjukkan cara mengintegrasikan API handler yang baru
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../lib/ApiHandler/wifi_api_handler.h"
#include "../include/config_wifi.h"

// Global objects
WiFiApiHandler* apiHandler;
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// Timing
unsigned long lastApiPost = 0;
const unsigned long API_POST_INTERVAL = 60000; // 60 detik

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("🚀 VAT Subsoil Monitor - WiFi + SD Card Version");
    Serial.println("========================================");
    
    // 1. Initialize indicators
    setupIndicators();
    ledBlink(LED1_PIN, 3, 200); // Boot indicator
    
    // 2. Initialize sensors
    if (!setupSensors()) {
        Serial.println("❌ Sensor setup failed!");
        while(1) {
            ledBlink(LED2_PIN, 1, 500);
            delay(1000);
        }
    }
    Serial.println("✅ Sensors initialized");
    
    // 3. Initialize GPS
    gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("✅ GPS initialized");
    
    // 4. Create API Handler instance
    apiHandler = new WiFiApiHandler(API_URL, DEVICE_ID);
    
    // 5. Setup SD Card through API Handler
    apiHandler->setupSDCard();
    
    // 6. Connect to WiFi (built-in reconnection in API handler)
    Serial.println("📡 Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        ledBlink(LED1_PIN, 1, 100);
    }
    Serial.println("\n✅ WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // 7. Setup NTP time
    configTime(7*3600, 0, "pool.ntp.org", "id.pool.ntp.org");
    Serial.println("✅ NTP time configured");
    
    Serial.println("🎉 System ready!");
    Serial.println("Commands: WIFI, API, SENSOR, TEST");
    Serial.println("========================================\n");
    
    // Indicator ready
    digitalWrite(LED1_PIN, HIGH);
}

void loop() {
    unsigned long currentMillis = millis();
    
    // 1. Process GPS data
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            // GPS data updated
        }
    }
    
    // 2. Check for commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        if (command == "WIFI") {
            Serial.println("\n📡 WiFi Status:");
            Serial.print("Status: ");
            Serial.println(apiHandler->isConnected() ? "Connected ✅" : "Disconnected ❌");
            if (apiHandler->isConnected()) {
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                Serial.print("RSSI: ");
                Serial.print(WiFi.RSSI());
                Serial.println(" dBm");
                Serial.print("Gateway: ");
                Serial.println(WiFi.gatewayIP());
            }
        }
        else if (command == "API") {
            Serial.println("\n🧪 Testing API with sample data...");
            if (apiHandler->sendTestData()) {
                Serial.println("✅ API test successful!");
            } else {
                Serial.println("❌ API test failed!");
            }
        }
        else if (command == "SENSOR") {
            Serial.println("\n📏 Current Sensor Readings:");
            float d1, d2;
            if (readSensors(d1, d2)) {
                Serial.print("Distance 1: ");
                Serial.print(d1);
                Serial.println(" cm");
                Serial.print("Distance 2: ");
                Serial.print(d2);
                Serial.println(" cm");
                
                float depth = calculateDepth(d1, d2);
                Serial.print("Calculated Depth: ");
                Serial.print(depth);
                Serial.println(" cm");
            } else {
                Serial.println("❌ Failed to read sensors");
            }
            
            Serial.print("\nGPS Status: ");
            if (gps.location.isValid()) {
                Serial.println("FIXED ✅");
                Serial.print("Satellites: ");
                Serial.println(gps.satellites.value());
                Serial.print("Latitude: ");
                Serial.println(gps.location.lat(), 6);
                Serial.print("Longitude: ");
                Serial.println(gps.location.lng(), 6);
            } else {
                Serial.println("NO FIX ❌");
            }
        }
        else if (command == "TEST") {
            Serial.println("\n🔄 Running full system test...");
            
            // Test connectivity
            if (!apiHandler->checkConnection()) {
                Serial.println("❌ Connection test failed");
                return;
            }
            
            // Test sensors
            float d1, d2;
            if (!readSensors(d1, d2)) {
                Serial.println("❌ Sensor test failed");
                return;
            }
            
            Serial.println("✅ All systems operational!");
        }
        else {
            Serial.println("❓ Unknown command. Available: WIFI, API, SENSOR, TEST");
        }
    }
    
    // 3. Periodic data transmission
    if (currentMillis - lastApiPost >= API_POST_INTERVAL) {
        lastApiPost = currentMillis;
        
        Serial.println("\n⏰ Periodic data transmission...");
        
        // Read sensors
        float distance1, distance2;
        if (!readSensors(distance1, distance2)) {
            Serial.println("❌ Failed to read sensors - skipping transmission");
            ledBlink(LED2_PIN, 3, 200);
            return;
        }
        
        // Get GPS data
        float lat = 0, lon = 0;
        if (gps.location.isValid()) {
            lat = gps.location.lat();
            lon = gps.location.lng();
        } else {
            Serial.println("⚠️ GPS not fixed - using default coordinates");
            lat = DEFAULT_LATITUDE;
            lon = DEFAULT_LONGITUDE;
        }
        
        // Calculate depth
        float depth = calculateDepth(distance1, distance2);
        
        // Send data via API Handler (includes automatic SD backup)
        Serial.println("🚀 Sending data...");
        bool success = apiHandler->sendSensorData(distance1, distance2, lat, lon, depth);
        
        if (success) {
            Serial.println("✅ Data transmission successful!");
            ledBlink(LED1_PIN, 2, 300);
        } else {
            Serial.println("❌ Data transmission failed (but saved to SD!)");
            ledBlink(LED2_PIN, 2, 300);
        }
    }
    
    delay(100);
}

/*
 * FITUR UTAMA IMPLEMENTASI INI:
 * 
 * 1. 💾 SD Card Backup Otomatis:
 *    - Data tersimpan ke SD card pada setiap pengiriman (sukses/gagal)
 *    - Format file: /vatlog_YYYY-MM-DD.csv
 *    - Header CSV: Timestamp,Distance1,Distance2,Latitude,Longitude,Kedalaman
 * 
 * 2. 🔄 Error Handling:
 *    - API gagal → data tetap tersimpan ke SD
 *    - WiFi putus → data tersimpan ke SD untuk sync nanti
 *    - Power failure → data di SD tidak hilang
 * 
 * 3. 📊 Commands Available:
 *    - WIFI: Check WiFi status & signal
 *    - API: Test API connection dengan sample data
 *    - SENSOR: Check sensor readings & GPS status
 *    - TEST: Full system test
 * 
 * 4. 🔧 Easy Integration:
 *    - Ganti main_wifi.cpp dengan contoh ini
 *    - Atau copy pattern untuk integrasi ke code existing
 *    - APIHandler otomatis handle WiFi reconnection & SD logging
 */
