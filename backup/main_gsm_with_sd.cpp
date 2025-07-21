/*
 * EXAMPLE: GSM API Handler dengan SD Card Backup
 * 
 * Contoh penggunaan GSMApiHandler class yang sudah include SD card logging
 * File ini menunjukkan cara mengintegrasikan API handler yang baru untuk GSM
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../lib/ApiHandler/gsm_api_handler.h"
#include "../include/config_gsm.h"

// Global objects
GSMApiHandler* apiHandler;
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// Timing
unsigned long lastApiPost = 0;
const unsigned long API_POST_INTERVAL = 120000; // 2 menit untuk GSM (lebih hemat data)

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("🚀 VAT Subsoil Monitor - GSM + SD Card Version");
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
    
    // 4. Create GSM API Handler instance
    apiHandler = new GSMApiHandler(MODEM_RX, MODEM_TX, API_URL, DEVICE_ID, GSM_APN);
    
    // 5. Setup SD Card through API Handler
    apiHandler->setupSDCard();
    
    // 6. Initialize GSM modem
    Serial.println("📡 Initializing GSM modem...");
    if (!apiHandler->initialize()) {
        Serial.println("❌ GSM initialization failed!");
        while(1) {
            ledBlink(LED2_PIN, 1, 1000);
            delay(2000);
        }
    }
    Serial.println("✅ GSM modem initialized");
    
    // 7. Connect to cellular network
    Serial.println("📡 Connecting to cellular network...");
    if (!apiHandler->connect()) {
        Serial.println("❌ GSM connection failed!");
        Serial.println("💾 System will continue with SD-only logging");
        // Don't halt - continue with SD logging only
    } else {
        Serial.println("✅ GSM connected!");
        apiHandler->printStatus();
    }
    
    Serial.println("🎉 System ready!");
    Serial.println("Commands: GSM, API, SENSOR, TEST, POWER");
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
        
        if (command == "GSM") {
            Serial.println("\n📡 GSM Status:");
            apiHandler->printStatus();
            apiHandler->printNetworkInfo();
        }
        else if (command == "POWER") {
            Serial.println("\n🔄 Power cycling GSM modem...");
            apiHandler->powerCycle();
            delay(5000);
            
            if (apiHandler->initialize()) {
                Serial.println("✅ GSM reinitialized");
                if (apiHandler->connect()) {
                    Serial.println("✅ GSM reconnected");
                } else {
                    Serial.println("❌ GSM reconnection failed");
                }
            } else {
                Serial.println("❌ GSM reinitialization failed");
            }
        }
        else if (command == "API") {
            Serial.println("\n🧪 Testing API with sample data...");
            if (apiHandler->sendTestData()) {
                Serial.println("✅ API test successful!");
            } else {
                Serial.println("❌ API test failed! (but saved to SD card)");
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
            
            // Test GSM connection
            if (!apiHandler->testConnection()) {
                Serial.println("❌ GSM connection test failed");
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
            Serial.println("❓ Unknown command. Available: GSM, API, SENSOR, TEST, POWER");
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
        
        // Send data via GSM API Handler (includes automatic SD backup)
        Serial.println("🚀 Sending data via GSM...");
        bool success = apiHandler->sendSensorData(distance1, distance2, lat, lon, depth);
        
        if (success) {
            Serial.println("✅ Data transmission successful!");
            ledBlink(LED1_PIN, 2, 300);
        } else {
            Serial.println("❌ Data transmission failed (but saved to SD!)");
            Serial.println("💾 Data will be available for manual sync from SD card");
            ledBlink(LED2_PIN, 2, 300);
        }
    }
    
    delay(100);
}

/*
 * FITUR UTAMA IMPLEMENTASI GSM + SD:
 * 
 * 1. 💾 SD Card Backup Otomatis:
 *    - Data tersimpan ke SD card pada setiap pengiriman (sukses/gagal)
 *    - Format file: /vatlog_YYYY-MM-DD.csv
 *    - Critical backup saat GSM gagal/no signal
 * 
 * 2. 🔄 GSM Error Handling:
 *    - HTTP 601 error → retry dengan power cycle
 *    - No signal → data tersimpan ke SD
 *    - SIM card error → fallback ke SD-only mode
 *    - Power management untuk LILYGO T-Call
 * 
 * 3. 📊 Commands Available:
 *    - GSM: Check GSM status, signal, network info
 *    - POWER: Manual power cycle jika GSM bermasalah
 *    - API: Test API connection dengan sample data
 *    - SENSOR: Check sensor readings & GPS status
 *    - TEST: Full system test
 * 
 * 4. ⚡ Power Optimized:
 *    - Interval pengiriman lebih jarang (2 menit vs 1 menit WiFi)
 *    - Proper power management untuk GSM modem
 *    - LED indicators untuk status monitoring
 * 
 * 5. 🛠️ Robust Operation:
 *    - Continues operation even if GSM fails
 *    - SD card menyimpan semua data untuk manual sync
 *    - Power cycle command untuk recovery
 *    - Detailed logging untuk troubleshooting
 */
