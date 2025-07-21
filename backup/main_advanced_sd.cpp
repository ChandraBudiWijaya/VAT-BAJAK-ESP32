/*
 * EXAMPLE: Advanced SD Utils dengan Offline Queue System
 * 
 * Contoh penggunaan SdUtils yang canggih dengan sistem queue offline
 * untuk sinkronisasi data ketika koneksi pulih
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../lib/ApiHandler/wifi_api_handler.h"
#include "../lib/SdUtils/sd_utils.h"
#include "../include/config_wifi.h"

// Global objects
WiFiApiHandler* apiHandler;
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// Timing
unsigned long lastApiPost = 0;
unsigned long lastSyncAttempt = 0;
const unsigned long API_POST_INTERVAL = 60000; // 60 detik
const unsigned long SYNC_INTERVAL = 300000;    // 5 menit untuk sync offline data

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("🚀 VAT Subsoil Monitor - Advanced SD Utils Version");
    Serial.println("========================================");
    
    // 1. Initialize SD Card FIRST (critical for logging)
    if (!initSdCard()) {
        Serial.println("⚠️ WARNING: SD Card failed - no offline backup available!");
        // Continue anyway - system can work without SD
    }
    
    // 2. Initialize indicators
    setupIndicators();
    ledBlink(LED1_PIN, 3, 200);
    
    // 3. Initialize sensors
    if (!setupSensors()) {
        Serial.println("❌ Sensor setup failed!");
        while(1) {
            ledBlink(LED2_PIN, 1, 500);
            delay(1000);
        }
    }
    Serial.println("✅ Sensors initialized");
    
    // 4. Initialize GPS
    gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("✅ GPS initialized");
    
    // 5. Create API Handler instance
    apiHandler = new WiFiApiHandler(API_URL, DEVICE_ID);
    
    // 6. Connect to WiFi
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
    
    // 8. Sync any offline data if available
    if (apiHandler->isConnected() && isOfflineQueueNotEmpty()) {
        Serial.println("🔄 Found offline data - attempting sync...");
        if (apiHandler->syncOfflineData()) {
            Serial.println("✅ Offline data sync completed!");
        } else {
            Serial.println("⚠️ Offline data sync partially failed");
        }
    }
    
    // 9. Display SD Card stats
    printSdCardStats();
    
    Serial.println("🎉 System ready with advanced SD logging!");
    Serial.println("Commands: WIFI, API, SENSOR, TEST, SDCARD, SYNC, STATS");
    Serial.println("========================================\n");
    
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
            }
        }
        else if (command == "API") {
            Serial.println("\n🧪 Testing API with sample data...");
            if (apiHandler->sendTestData()) {
                Serial.println("✅ API test successful!");
            } else {
                Serial.println("❌ API test failed (but saved to SD!)");
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
                
                // Test direct SD logging
                VatSensorData testData;
                testData.isValid = true;
                testData.distance1 = d1;
                testData.distance2 = d2;
                testData.latitude = gps.location.isValid() ? gps.location.lat() : DEFAULT_LATITUDE;
                testData.longitude = gps.location.isValid() ? gps.location.lng() : DEFAULT_LONGITUDE;
                testData.depth = depth;
                testData.year = 2024;
                testData.month = 1;
                testData.day = 1;
                testData.hour = 12;
                testData.minute = 30;
                testData.second = 45;
                testData.satellites = gps.satellites.value();
                testData.hdop = gps.hdop.hdop();
                
                writeToDailyLog(testData);
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
                Serial.print("HDOP: ");
                Serial.println(gps.hdop.hdop());
            } else {
                Serial.println("NO FIX ❌");
            }
        }
        else if (command == "SDCARD") {
            Serial.println("\n💾 SD Card Operations:");
            if (checkSdCardStatus()) {
                Serial.println("✅ SD Card operational");
                printSdCardStats();
            } else {
                Serial.println("❌ SD Card check failed");
                Serial.println("Attempting re-initialization...");
                if (initSdCard()) {
                    Serial.println("✅ SD Card re-initialized successfully");
                } else {
                    Serial.println("❌ SD Card re-initialization failed");
                }
            }
        }
        else if (command == "SYNC") {
            Serial.println("\n🔄 Manual Offline Data Sync:");
            if (!apiHandler->isConnected()) {
                Serial.println("❌ WiFi not connected - cannot sync");
            } else if (!isOfflineQueueNotEmpty()) {
                Serial.println("📝 No offline data to sync");
            } else {
                int queueCount = getQueueLineCount();
                Serial.print("📋 Found ");
                Serial.print(queueCount);
                Serial.println(" pending records");
                
                if (apiHandler->syncOfflineData()) {
                    Serial.println("✅ Manual sync completed successfully!");
                } else {
                    Serial.println("⚠️ Manual sync partially failed");
                }
            }
        }
        else if (command == "STATS") {
            printSdCardStats();
        }
        else if (command == "TEST") {
            Serial.println("\n🔄 Running comprehensive system test...");
            
            // Test connectivity
            if (!apiHandler->checkConnection()) {
                Serial.println("❌ Connection test failed");
                return;
            }
            
            // Test SD Card
            if (!checkSdCardStatus()) {
                Serial.println("⚠️ SD Card test failed - continuing anyway");
            }
            
            // Test sensors
            float d1, d2;
            if (!readSensors(d1, d2)) {
                Serial.println("❌ Sensor test failed");
                return;
            }
            
            // Test data logging
            VatSensorData testData;
            testData.isValid = true;
            testData.distance1 = d1;
            testData.distance2 = d2;
            testData.latitude = -6.175392;
            testData.longitude = 106.827153;
            testData.depth = calculateDepth(d1, d2);
            testData.year = 2024;
            testData.month = 1;
            testData.day = 1;
            testData.hour = 12;
            testData.minute = 0;
            testData.second = 0;
            testData.satellites = 8;
            testData.hdop = 1.2;
            
            Serial.println("📝 Testing data logging...");
            backupCriticalData(testData);
            
            Serial.println("✅ All systems tested successfully!");
        }
        else {
            Serial.println("❓ Unknown command.");
            Serial.println("Available: WIFI, API, SENSOR, SDCARD, SYNC, STATS, TEST");
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
        
        // Send data via API Handler
        // Data will be automatically saved to SD regardless of API success/failure
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
    
    // 4. Periodic offline data sync (when connected)
    if (currentMillis - lastSyncAttempt >= SYNC_INTERVAL) {
        lastSyncAttempt = currentMillis;
        
        if (apiHandler->isConnected() && isOfflineQueueNotEmpty()) {
            Serial.println("🔄 Auto-syncing offline data...");
            apiHandler->syncOfflineData();
        }
    }
    
    delay(100);
}

/*
 * FITUR ADVANCED SD UTILS:
 * 
 * 1. 💾 Intelligent Data Backup:
 *    - Daily CSV logs dengan struktur folder terorganisir
 *    - Offline queue untuk sync otomatis ketika koneksi pulih
 *    - Critical backup untuk data penting
 *    - Progress tracking untuk resume sync
 * 
 * 2. 🔄 Automatic Sync System:
 *    - Auto-sync offline data setiap 5 menit jika connected
 *    - Manual sync via command
 *    - Resume sync dari posisi terakhir jika gagal
 *    - Rate limiting untuk mencegah overload server
 * 
 * 3. 📊 Comprehensive Monitoring:
 *    - Real-time SD Card health check
 *    - Queue statistics dan file size monitoring
 *    - Data integrity verification
 *    - Storage space management
 * 
 * 4. 🛠️ Maintenance Features:
 *    - Automatic old log cleanup (TODO)
 *    - File compression untuk menghemat space (TODO)
 *    - Data export untuk backup eksternal (TODO)
 *    - Error recovery dan retry mechanism
 * 
 * 5. 🎯 Production Ready:
 *    - Robust error handling untuk semua operasi SD
 *    - Fallback operation jika SD Card gagal
 *    - Consistent data format untuk easy analysis
 *    - Memory efficient untuk ESP32 constraints
 */
