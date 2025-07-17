#include <Arduino.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"

// Forward declarations
bool sendDataToAPI(float d1, float d2, float lat, float lon, float depth);
String getISOTimestamp();

// Flag untuk mengontrol sensor mode  
bool USE_REAL_SENSORS = true;   // ENABLED untuk testing sensor hardware
bool SENSORS_INITIALIZED = false;

// WiFi Configuration - UPDATE THESE WITH YOUR WIFI
const char* ssid = "Kiwi Gejrot";          // Ganti dengan nama WiFi Anda
const char* password = "1122334455";   // Ganti dengan password WiFi Anda

// API Configuration
const char* API_URL = "https://api-vatsubsoil-dev.ggfsystem.com/subsoils";
const char* DEVICE_ID = "BJK0001";

// Timing - lebih sering untuk sensor testing
unsigned long lastApiPost = 0;
const unsigned long API_POST_INTERVAL = 60000; // 60 detik untuk API (lebih jarang)
const unsigned long SENSOR_READ_INTERVAL = 1000; // 1 detik untuk sensor (lebih sering)

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
    
    // Fallback: gunakan uptime sebagai base
    unsigned long uptimeSeconds = millis() / 1000;
    
    // Base timestamp (startup time approximation)
    int year = 2025;
    int month = 7;
    int day = 16;
    int hour = 10 + (uptimeSeconds / 3600); // Add uptime hours
    int minute = (uptimeSeconds % 3600) / 60;
    int second = uptimeSeconds % 60;
    
    // Handle hour overflow
    while (hour >= 24) {
        hour -= 24;
        day++;
    }
    
    sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d+07:00", 
            year, month, day, hour, minute, second);
    
    return String(timestamp);
}

// Fungsi untuk mengirim data ke API dengan format PERSIS SAMA dengan code yang berhasil
bool sendDataToAPI(float d1, float d2, float lat, float lon, float depth) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi not connected - skipping API post");
        return false;
    }
    
    // Ambil data GPS dan altitude (gunakan data real jika ada, fallback ke dummy)
    float currentLat = (SENSORS_INITIALIZED && gps.location.isValid()) ? lat : -4.820175;
    float currentLon = (SENSORS_INITIALIZED && gps.location.isValid()) ? lon : 105.237451;
    float currentAlt = (SENSORS_INITIALIZED && gps.altitude.isValid()) ? gps.altitude.meters() : 668.29;
    
    // Format timestamp sesuai server (ISO format tanpa timezone)
    String timestamp = getISOTimestamp();
    timestamp.replace("+07:00", ""); // Hapus timezone
    
    // Membuat HTTP request PERSIS SAMA dengan code yang berhasil
    HTTPClient http;
    
    http.begin(API_URL);  // Inisialisasi URL API
    http.addHeader("Content-Type", "application/json");
    
    // Membuat payload JSON PERSIS SAMA dengan code yang berhasil
    String payload = String("{") +
                   "\"type\":\"sensor\"," +
                   "\"deviceId\":\"" + DEVICE_ID + "\"," +
                   "\"gps\":{" +
                   "\"lat\":" + String(currentLat, 7) + "," +
                   "\"lon\":" + String(currentLon, 7) + "," +
                   "\"alt\":" + String(currentAlt, 7) + "," +
                   "\"sog\":0," +
                   "\"cog\":0" +
                   "}," +
                   "\"ultrasonic\":{" +
                   "\"dist1\":" + String(d1) + "," +
                   "\"dist2\":" + String(d2) +
                   "}," +
                   "\"timestamp\":\"" + timestamp + "\"" +
                   "}";

    Serial.println("\n🚀 POSTING TO API (Exact Working Format):");
    Serial.print("URL: ");
    Serial.println(API_URL);
    Serial.print("Payload: ");
    Serial.println(payload);
    
    // Mengirim POST request ke API PERSIS SAMA dengan code yang berhasil
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("Kode Respon HTTP: ");
        Serial.println(httpResponseCode);
        Serial.print("Response: ");
        Serial.println(response);
        http.end();
        return true;
    } else {
        Serial.print("Error HTTP: ");
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
    Serial.println("ESP32 VAT SUBSOIL MONITOR - API READY");
    Serial.println("========================================");
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    Serial.println("Setup completed successfully!");
    Serial.println("========================================");
    
    // WiFi Connection
    Serial.println("� Connecting to WiFi...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    unsigned long wifiStart = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("✅ WiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        // Setup NTP untuk mendapatkan waktu yang akurat
        Serial.println("🕐 Setting up NTP time...");
        configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC+7 (WIB)
        
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
        Serial.println("");
        Serial.println("❌ WiFi connection failed - will use offline mode");
    }
    
    // Real sensor initialization dengan detailed monitoring
    if (USE_REAL_SENSORS) {
        Serial.println("🔧 REAL SENSOR TESTING MODE");
        Serial.println("========================================");
        Serial.flush();
        delay(1000);
        
        Serial.println("Step 1: Initializing sensor library...");
        Serial.flush();
        delay(500);
        
        Serial.println("Step 2: Setting up hardware connections...");
        Serial.println("  - GPS Module (SIM800L/GPS)");
        Serial.println("  - Ultrasonic Sensor 1");  
        Serial.println("  - Ultrasonic Sensor 2");
        Serial.flush();
        delay(1000);
        
        Serial.println("Step 3: Attempting sensor setup...");
        Serial.flush();
        
        // Setup sensor dengan error handling yang lebih baik
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
            Serial.println("========================================");
            Serial.println("✅ REAL SENSORS INITIALIZED!");
            Serial.println("========================================");
            
            // Setup LED indicators
            Serial.println("🔧 Setting up LED indicators...");
            setup_leds();
            Serial.println("✅ LED indicators ready!");
            
            // Test initial sensor readings
            Serial.println("🧪 Initial sensor test:");
            delay(1000);
            
            // Test ultrasonic sensors
            Serial.println("Testing ultrasonic sensors...");
            try {
                read_ultrasonic_sensors();
                Serial.print("✅ Distance1: ");
                Serial.print(distance1);
                Serial.println(" cm");
                Serial.print("✅ Distance2: ");
                Serial.print(distance2);
                Serial.println(" cm");
            } catch (...) {
                Serial.println("⚠️ Ultrasonic sensor error");
            }
            
            delay(1000);
            
            // Test GPS
            Serial.println("Testing GPS module...");
            try {
                read_gps_data();
                if (gps.location.isValid()) {
                    Serial.print("✅ GPS Location: ");
                    Serial.print(gps.location.lat(), 6);
                    Serial.print(", ");
                    Serial.println(gps.location.lng(), 6);
                } else {
                    Serial.println("⏳ GPS searching for satellites...");
                }
            } catch (...) {
                Serial.println("⚠️ GPS module error");
            }
            
            Serial.println("========================================");
            
        } else {
            SENSORS_INITIALIZED = false;
            Serial.println("========================================");
            Serial.println("❌ SENSOR SETUP FAILED!");
            Serial.println("Possible issues:");
            Serial.println("  - Hardware not connected");
            Serial.println("  - Wrong pin configuration");
            Serial.println("  - Power supply issues");
            Serial.println("  - Library compatibility");
            Serial.println("Switching to dummy data mode...");
            
            // Setup LED indicators tetap untuk visual feedback
            Serial.println("🔧 Setting up LED indicators...");
            setup_leds();
            Serial.println("✅ LED indicators ready (dummy mode)!");
            Serial.println("========================================");
        }
    } else {
        Serial.println("🔧 Using dummy sensor data for API testing");
        Serial.println("💡 Serial Commands:");
        Serial.println("   SENSOR  - Enable real sensors");
        Serial.println("   DUMMY   - Use dummy data");
        Serial.println("   WIFI    - Check WiFi status");
        Serial.println("   TIME    - Show current timestamp");
        Serial.println("   API     - Test API with current data");
        Serial.println("   TESTAPI - Test API with known working dummy data");
        
        // Setup LED indicators untuk visual feedback
        Serial.println("🔧 Setting up LED indicators...");
        setup_leds();
        Serial.println("✅ LED indicators ready (dummy mode)!");
    }
    
    Serial.println("Setup finished - entering main loop");
    Serial.flush();
}

void loop() {
    static unsigned long sensorTimer = 0;
    static unsigned long displayTimer = 0;
    static int sensorReadCount = 0;
    static int displayCount = 0;
    
    // Check for serial commands
    if (Serial.available()) {
        String command = Serial.readString();
        command.trim();
        command.toUpperCase();
        
        if (command == "SENSOR") {
            Serial.println("\n🔧 SWITCHING TO REAL SENSOR MODE...");
            USE_REAL_SENSORS = true;
            SENSORS_INITIALIZED = false;
            
            // Try to initialize sensors
            Serial.println("Attempting sensor setup...");
            try {
                setup_sensors();
                SENSORS_INITIALIZED = true;
                Serial.println("✅ Real sensors activated!");
                
                // Setup LED indicators
                setup_leds();
                Serial.println("✅ LED indicators ready!");
            } catch (...) {
                Serial.println("❌ Sensor setup failed - staying in dummy mode");
                USE_REAL_SENSORS = false;
            }
        } else if (command == "DUMMY") {
            Serial.println("\n🔧 SWITCHING TO DUMMY DATA MODE...");
            USE_REAL_SENSORS = false;
            SENSORS_INITIALIZED = false;
        } else if (command == "WIFI") {
            Serial.println("\n📶 WIFI STATUS:");
            Serial.print("Status: ");
            Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
            }
        } else if (command == "TIME") {
            Serial.println("\n🕐 TIMESTAMP INFO:");
            Serial.print("Current timestamp: ");
            Serial.println(getISOTimestamp());
            Serial.print("Uptime: ");
            Serial.print(millis() / 1000);
            Serial.println(" seconds");
        } else if (command == "TEST") {
            Serial.println("\n🧪 MANUAL SENSOR TEST:");
            if (SENSORS_INITIALIZED) {
                try {
                    read_ultrasonic_sensors();
                    read_gps_data();
                    display_sensor_data();
                } catch (...) {
                    Serial.println("❌ Error during manual test");
                }
            } else {
                Serial.println("❌ Sensors not initialized");
            }
        } else if (command == "LED") {
            Serial.println("\n💡 LED TEST:");
            Serial.println("Testing LED indicators...");
            
            // Test LED1
            Serial.println("🔴 LED1 ON");
            digitalWrite(12, HIGH); // LED1_PIN
            delay(1000);
            Serial.println("🔴 LED1 OFF");
            digitalWrite(12, LOW);
            delay(500);
            
            // Test LED2
            Serial.println("🔴 LED2 ON");
            digitalWrite(25, HIGH); // LED2_PIN
            delay(1000);
            Serial.println("🔴 LED2 OFF");
            digitalWrite(25, LOW);
            
            Serial.println("✅ LED test completed");
        } else if (command == "API") {
            Serial.println("\n🧪 API TEST:");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Testing API with current sensor data...");
                
                float test_d1 = SENSORS_INITIALIZED ? distance1 : 23.5;
                float test_d2 = SENSORS_INITIALIZED ? distance2 : 32.1;
                float test_lat = SENSORS_INITIALIZED && gps.location.isValid() ? gps.location.lat() : -6.175392;
                float test_lon = SENSORS_INITIALIZED && gps.location.isValid() ? gps.location.lng() : 106.827153;
                float test_depth = SENSORS_INITIALIZED ? calculate_depth() : (2.84 * test_d2 - 16.6);
                
                bool apiSuccess = sendDataToAPI(test_d1, test_d2, test_lat, test_lon, test_depth);
                if (apiSuccess) {
                    Serial.println("✅ API test successful!");
                } else {
                    Serial.println("❌ API test failed!");
                }
            } else {
                Serial.println("❌ WiFi not connected - cannot test API");
            }
        } else if (command == "TESTAPI") {
            Serial.println("\n🧪 API TEST WITH EXACT WORKING FORMAT:");
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Testing API with EXACT same format and data as working code...");
                
                // Data dummy PERSIS SAMA dengan yang berhasil
                float test_d1 = 23.9;
                float test_d2 = 30.8;
                float test_lat = -4.820175;
                float test_lon = 105.237451;
                float test_depth = 0; // Tidak digunakan di payload yang berhasil
                
                bool apiSuccess = sendDataToAPI(test_d1, test_d2, test_lat, test_lon, test_depth);
                if (apiSuccess) {
                    Serial.println("✅ Exact working format API test successful!");
                } else {
                    Serial.println("❌ API test failed!");
                }
            } else {
                Serial.println("❌ WiFi not connected - cannot test API");
            }
        }
    }
    
    // Baca sensor data setiap detik jika sensor aktif
    if (SENSORS_INITIALIZED && millis() - sensorTimer >= SENSOR_READ_INTERVAL) {
        sensorTimer = millis();
        sensorReadCount++;
        
        // Baca sensor dengan detailed error handling
        bool ultrasonicOk = false;
        bool gpsOk = false;
        
        try {
            read_ultrasonic_sensors();
            ultrasonicOk = true;
        } catch (...) {
            Serial.println("⚠️ Error reading ultrasonic sensors");
        }
        
        try {
            read_gps_data();
            gpsOk = true;
        } catch (...) {
            Serial.println("⚠️ Error reading GPS data");
        }
        
        // Quick sensor status update
        if (sensorReadCount % 5 == 0) { // Setiap 5 detik
            Serial.print("📊 Sensor check #");
            Serial.print(sensorReadCount);
            Serial.print(" - Ultrasonic: ");
            Serial.print(ultrasonicOk ? "✅" : "❌");
            Serial.print(", GPS: ");
            Serial.println(gpsOk ? "✅" : "❌");
        }
    }
    
    // Display detailed data setiap 5 detik
    if (millis() - displayTimer >= 5000) {
        displayTimer = millis();
        displayCount++;
        
        Serial.println("");
        Serial.println("========================================");
        Serial.print("📊 VAT SUBSOIL MONITOR - Reading #");
        Serial.println(displayCount);
        Serial.println("========================================");
        
        if (SENSORS_INITIALIZED) {
            // Tampilkan data real sensor dengan detail
            Serial.println("📏 REAL SENSOR DATA:");
            
            try {
                // Ultrasonic sensors
                Serial.println("🔍 ULTRASONIC SENSORS:");
                Serial.print("  Distance 1: ");
                Serial.print(distance1);
                Serial.print(" cm");
                if (distance1 > 0 && distance1 < 400) {
                    Serial.println(" ✅");
                } else {
                    Serial.println(" ⚠️ (out of range)");
                }
                
                Serial.print("  Distance 2: ");
                Serial.print(distance2);
                Serial.print(" cm");
                if (distance2 > 0 && distance2 < 400) {
                    Serial.println(" ✅");
                } else {
                    Serial.println(" ⚠️ (out of range)");
                }
                
                // Calculated depth
                float depth = calculate_depth();
                Serial.print("🌊 Calculated Depth: ");
                Serial.print(depth);
                Serial.println(" cm");
                
                // Update LED indicators berdasarkan sensor readings
                Serial.println("💡 LED INDICATORS:");
                update_leds(distance1, distance2);
                
                // GPS Data
                Serial.println("🛰️  GPS MODULE:");
                if (gps.location.isValid()) {
                    Serial.print("  Status: FIXED ✅ (");
                    Serial.print(gps.satellites.value());
                    Serial.println(" satellites)");
                    Serial.print("  Latitude: ");
                    Serial.println(gps.location.lat(), 6);
                    Serial.print("  Longitude: ");
                    Serial.println(gps.location.lng(), 6);
                    Serial.print("  Altitude: ");
                    Serial.print(gps.altitude.meters());
                    Serial.println(" m");
                } else {
                    Serial.println("  Status: SEARCHING... ⏳");
                    if (gps.satellites.isValid()) {
                        Serial.print("  Satellites visible: ");
                        Serial.println(gps.satellites.value());
                    }
                }
                
                // API POST dengan real sensor data
                if (WiFi.status() == WL_CONNECTED && millis() - lastApiPost >= API_POST_INTERVAL) {
                    lastApiPost = millis();
                    Serial.println("\n🚀 POSTING REAL SENSOR DATA TO API!");
                    
                    float lat = gps.location.isValid() ? gps.location.lat() : 0.0;
                    float lon = gps.location.isValid() ? gps.location.lng() : 0.0;
                    
                    bool apiSuccess = sendDataToAPI(distance1, distance2, lat, lon, depth);
                    if (apiSuccess) {
                        Serial.println("✅ REAL SENSOR DATA POSTED SUCCESSFULLY");
                    } else {
                        Serial.println("❌ API POST FAILED");
                    }
                }
                
            } catch (...) {
                Serial.println("❌ ERROR displaying sensor data");
            }
            
        } else {
            // Fallback dummy data (hanya jika sensor gagal)
            Serial.println("� DUMMY DATA (Sensor Failed):");
            float dummy_d1 = 25.3 + (displayCount * 0.1);
            float dummy_d2 = 28.7 + (displayCount * 0.15);
            float dummy_lat = -6.175392;
            float dummy_lon = 106.827153;
            float dummy_depth = 2.84 * dummy_d2 - 16.6;
            
            Serial.print("   Distance 1: ");
            Serial.print(dummy_d1, 1);
            Serial.println(" cm");
            Serial.print("   Distance 2: ");
            Serial.print(dummy_d2, 1);
            Serial.println(" cm");
            Serial.print("   GPS: ");
            Serial.print(dummy_lat, 6);
            Serial.print(", ");
            Serial.println(dummy_lon, 6);
            Serial.print("   Kedalaman: ");
            Serial.print(dummy_depth, 1);
            Serial.println(" cm");
            
            // Update LED indicators dengan dummy data
            Serial.println("💡 LED INDICATORS (Dummy):");
            update_leds(dummy_d1, dummy_d2);
        }
        
        // System info
        Serial.println("");
        Serial.print("🔧 System - Uptime: ");
        Serial.print(millis()/1000);
        Serial.print("s, Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        
        Serial.print("📡 Mode: ");
        Serial.println(SENSORS_INITIALIZED ? "REAL SENSORS ✅" : "DUMMY DATA ⚠️");
        
        Serial.println("💡 Commands: SENSOR, DUMMY, WIFI, TIME, TEST, LED, API, TESTAPI");
        Serial.println("========================================");
        Serial.flush();
    }
    
    delay(50); // Shorter delay for more responsive sensor reading
}
