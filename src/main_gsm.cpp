#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "../lib/ApiHandler/gsm_api_handler.h"
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../include/config.h"

// Global objects (using TESTED & WORKING TinyGSM handler)
GSMApiHandler gsmHandler(DEVICE_ID);

// Timing variables
unsigned long lastPostTime = 0;
unsigned long lastSensorTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 VAT BAJAK ESP32 - GSM Current Version");
    Serial.println("==========================================");
    Serial.println("Using TESTED & WORKING TinyGSM Method");
    Serial.println("Based on successful test implementation");
    Serial.println("==========================================");
    
    // Initialize indicators (LEDs)
    setup_leds();
    
    // Initialize sensors
    Serial.println("🔧 Initializing sensors...");
    setup_sensors();
    
    // Initialize GSM with tested method (same as working version)
    Serial.println("📡 Initializing GSM (TESTED TinyGSM Method)...");
    if (gsmHandler.initialize()) {
        Serial.println("✅ GSM initialization successful");
        
        // Connect to network
        Serial.println("🌐 Connecting to GSM network...");
        if (gsmHandler.connect()) {
            Serial.println("✅ GSM connected successfully");
            
            // Print network info (like working version)
            gsmHandler.printNetworkInfo();
            
            // Test with sample data (exact same format as working version)
            Serial.println("🧪 Testing with sample data...");
            bool testResult = gsmHandler.sendSensorData(5.20, 52.80, -4.823621, 105.226395, 100.0);
            if (testResult) {
                Serial.println("🎉 Test successful! Ready for production");
            } else {
                Serial.println("⚠️ Test failed, but continuing...");
            }
        } else {
            Serial.println("❌ GSM connection failed");
        }
    } else {
        Serial.println("❌ GSM initialization failed");
    }
    
    Serial.println("🚀 Setup complete - entering main loop");
    Serial.println("📊 Data will be sent every " + String(POST_INTERVAL/1000) + " seconds");
    Serial.println("==========================================");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Read sensors periodically (same timing as working version)
    if (currentTime - lastSensorTime >= SAVE_SD_INTERVAL) {
        // Read sensor data using existing functions
        read_ultrasonic_sensors();
        read_gps_data();
        
        // Print sensor data using existing global variables
        Serial.println("📊 Current Sensor Readings:");
        Serial.printf("Distance1: %.2f cm\n", distance1);
        Serial.printf("Distance2: %.2f cm\n", distance2);
        Serial.printf("GPS: %.6f, %.6f (Alt: %.2f)\n", latitude, longitude, altitude);
        
        // Update LEDs
        update_leds(distance1, distance2);
        
        // Update timing
        lastSensorTime = currentTime;
    }
    
    // Send data to API periodically (same as working version)
    if (currentTime - lastPostTime >= POST_INTERVAL) {
        if (gsmHandler.isModemConnected()) {
            Serial.println("\n🚀 SENDING DATA TO PRODUCTION API");
            Serial.println("=====================================");
            Serial.println("📡 Target: api-vatsubsoil-dev.ggfsystem.com/subsoils");
            Serial.println("🔒 Using HTTPS with exact working format");
            
            // Calculate depth if needed
            float depth = calculate_depth();
            
            // Send to API using TESTED TinyGSM method with current sensor data
            // This uses the exact same format that works in your test code:
            // {"type":"sensor","deviceId":"BJK0001","gps":{"lat":-4.823621,"lon":105.226395,"alt":100.0,"sog":0,"cog":0},"ultrasonic":{"dist1":5.20,"dist2":52.80},"timestamp":"2025-07-21T09:52:00+07:00"}
            bool success = gsmHandler.sendSensorData(
                distance1, 
                distance2,
                latitude, 
                longitude, 
                depth
            );
            
            if (success) {
                Serial.println("✅ Data sent successfully to production API!");
                Serial.println("🎉 Sistem monitoring tanah subsoil berjalan normal");
                
                // Success LED indication (like your test code)
                for (int i = 0; i < 3; i++) {
                    digitalWrite(LED1_PIN, HIGH);
                    delay(200);
                    digitalWrite(LED1_PIN, LOW);
                    delay(200);
                }
            } else {
                Serial.println("❌ Failed to send data to production API");
                Serial.println("🔧 Cek koneksi internet atau format data");
                
                // Error LED indication
                for (int i = 0; i < 3; i++) {
                    digitalWrite(LED2_PIN, HIGH);
                    delay(500);
                    digitalWrite(LED2_PIN, LOW);
                    delay(500);
                }
            }
        } else {
            Serial.println("❌ GSM not connected - attempting reconnection...");
            
            // Try to reconnect
            if (gsmHandler.connect()) {
                Serial.println("✅ GSM reconnected");
            }
        }
        
        lastPostTime = currentTime;
    }
    
    // Small delay to prevent overwhelming the system
    delay(100);
}

// Serial command interface for debugging and testing (enhanced from working version)
void serialEvent() {
    if (Serial.available()) {
        String command = Serial.readString();
        command.trim();
        
        if (command == "test") {
            Serial.println("🧪 Manual test initiated...");
            Serial.println("📡 Sending test data to production API...");
            // Use exact same test data as working version
            bool result = gsmHandler.sendSensorData(12.34, 56.78, -6.175392, 106.827153, 25.5);
            Serial.println(result ? "✅ Test successful" : "❌ Test failed");
        }
        else if (command == "status") {
            Serial.println("\n📋 SYSTEM STATUS");
            Serial.println("================");
            gsmHandler.printStatus();
            gsmHandler.printNetworkInfo();
            display_sensor_data(); // Use existing function
            Serial.println("📡 API Target: api-vatsubsoil-dev.ggfsystem.com");
            Serial.println("📊 Format: Working JSON structure from test");
        }
        else if (command == "reconnect") {
            Serial.println("🔄 Reconnecting GSM...");
            gsmHandler.disconnect();
            delay(2000);
            if (gsmHandler.connect()) {
                Serial.println("✅ Reconnection successful");
                gsmHandler.printNetworkInfo();
            } else {
                Serial.println("❌ Reconnection failed");
            }
        }
        else if (command == "sensors") {
            Serial.println("📊 Reading sensors manually...");
            read_ultrasonic_sensors();
            read_gps_data();
            display_sensor_data();
            
            // Show what would be sent to API
            float depth = calculate_depth();
            Serial.println("\n📡 Data that would be sent to API:");
            Serial.printf("Distance1: %.2f cm\n", distance1);
            Serial.printf("Distance2: %.2f cm\n", distance2);
            Serial.printf("GPS: %.6f, %.6f\n", latitude, longitude);
            Serial.printf("Depth: %.2f cm\n", depth);
        }
        else if (command == "production") {
            Serial.println("🚀 FORCE SEND TO PRODUCTION API");
            Serial.println("================================");
            
            // Read current sensors
            read_ultrasonic_sensors();
            read_gps_data();
            float depth = calculate_depth();
            
            // Send immediately
            bool result = gsmHandler.sendSensorData(distance1, distance2, latitude, longitude, depth);
            if (result) {
                Serial.println("✅ Production API send successful!");
            } else {
                Serial.println("❌ Production API send failed!");
            }
        }
        else {
            Serial.println("\n📋 Available commands:");
            Serial.println("====================");
            Serial.println("  test       - Send test data to production API");
            Serial.println("  status     - Show system and network status");
            Serial.println("  reconnect  - Reconnect GSM network");
            Serial.println("  sensors    - Read sensors manually");
            Serial.println("  production - Force send current data to production");
            Serial.println("\n🎯 This version uses TESTED & WORKING TinyGSM method");
            Serial.println("📡 Target: api-vatsubsoil-dev.ggfsystem.com/subsoils");
        }
    }
}
