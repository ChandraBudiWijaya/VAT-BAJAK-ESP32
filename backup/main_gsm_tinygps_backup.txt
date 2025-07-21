#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "../lib/ApiHandler/gsm_tinygps_handler.h"
#include "../lib/VatSensor/sensors.h"
#include "../lib/indicators/indicators.h"
#include "../include/config.h"

// Global objects
GSMTinyGpsHandler gsmHandler(DEVICE_ID);

// Timing variables
unsigned long lastPostTime = 0;
unsigned long lastSensorTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 VAT BAJAK ESP32 - GSM TinyGPS Version");
    Serial.println("========================================");
    Serial.println("Using TESTED & WORKING TinyGSM Method");
    Serial.println("========================================");
    
    // Initialize indicators (LEDs)
    setup_leds();
    
    // Initialize sensors
    Serial.println("🔧 Initializing sensors...");
    setup_sensors();
    
    // Initialize GSM with tested method
    Serial.println("📡 Initializing GSM (TinyGSM Method)...");
    if (gsmHandler.initialize()) {
        Serial.println("✅ GSM initialization successful");
        
        // Connect to network
        Serial.println("🌐 Connecting to GSM network...");
        if (gsmHandler.connect()) {
            Serial.println("✅ GSM connected successfully");
            
            // Print network info
            gsmHandler.printNetworkInfo();
            
            // Test with sample data
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
    Serial.println("========================================");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Read sensors periodically
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
    
    // Send data to API periodically
    if (currentTime - lastPostTime >= POST_INTERVAL) {
        if (gsmHandler.isModemConnected()) {
            Serial.println("\n🚀 SENDING DATA TO PRODUCTION API");
            Serial.println("=====================================");
            
            // Calculate depth if needed
            float depth = calculate_depth();
            
            // Send to API using TinyGSM method with current sensor data
            bool success = gsmHandler.sendSensorData(
                distance1, 
                distance2,
                latitude, 
                longitude, 
                depth
            );
            
            if (success) {
                Serial.println("✅ Data sent successfully!");
            } else {
                Serial.println("❌ Failed to send data");
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

// Optional: Add function for manual testing via Serial commands
void serialEvent() {
    if (Serial.available()) {
        String command = Serial.readString();
        command.trim();
        
        if (command == "test") {
            Serial.println("🧪 Manual test initiated...");
            bool result = gsmHandler.sendSensorData(12.34, 56.78, -6.175392, 106.827153, 25.5);
            Serial.println(result ? "✅ Test successful" : "❌ Test failed");
        }
        else if (command == "status") {
            gsmHandler.printStatus();
            gsmHandler.printNetworkInfo();
            display_sensor_data(); // Use existing function
        }
        else if (command == "reconnect") {
            Serial.println("🔄 Reconnecting GSM...");
            gsmHandler.disconnect();
            delay(2000);
            if (gsmHandler.connect()) {
                Serial.println("✅ Reconnection successful");
            } else {
                Serial.println("❌ Reconnection failed");
            }
        }
        else if (command == "sensors") {
            Serial.println("📊 Reading sensors manually...");
            read_ultrasonic_sensors();
            read_gps_data();
            display_sensor_data();
        }
        else {
            Serial.println("📋 Available commands:");
            Serial.println("  test      - Send test data");
            Serial.println("  status    - Show system status");
            Serial.println("  reconnect - Reconnect GSM");
            Serial.println("  sensors   - Read sensors manually");
        }
    }
}
