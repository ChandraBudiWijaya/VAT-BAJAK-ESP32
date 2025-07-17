#include <Arduino.h>
#include <TinyGPS++.h>
#include <esp_core_dump.h>
#include "../lib/VatSensor/sensors.h"

void setup() {
    // Clear any previous core dump
    esp_core_dump_image_erase();
    
    Serial.begin(115200);
    delay(5000); // Give more time for serial to stabilize
    
    // Clear serial buffer
    while(Serial.available()) {
        Serial.read();
    }
    
    Serial.println("");
    Serial.println("========================================");
    Serial.println("VAT SUBSOIL MONITOR - ESP32 READY!");
    Serial.println("========================================");
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    Serial.println("Serial communication is working properly!");
    Serial.println("Setup completed successfully!");
    Serial.println("========================================");
    
    // Setup sensors dengan error handling
    Serial.println("Initializing sensors...");
    Serial.flush();
    delay(1000);
    
    try {
        setup_sensors();
        Serial.println("Sensors setup completed!");
    } catch (...) {
        Serial.println("ERROR: Sensor setup failed!");
    }
    
    Serial.println("Setup finished - entering main loop");
    Serial.flush();
}

void loop() {
    static unsigned long timer = 0;
    static int count = 0;
    
    // Safety: Check if system is stable
    if (millis() < 10000) {
        delay(100);
        return; // Skip loop untuk 10 detik pertama
    }
    
    // Baca data sensor dengan error handling
    try {
        read_ultrasonic_sensors();
        read_gps_data();
    } catch (...) {
        Serial.println("ERROR: Sensor read failed!");
    }
    
    // Tampilkan data setiap 3 detik
    if (millis() - timer >= 3000) {
        timer = millis();
        count++;
        
        Serial.println("");
        Serial.println("========================================");
        Serial.print("VAT SUBSOIL MONITOR - Reading #");
        Serial.println(count);
        Serial.println("========================================");
        
        // Tampilkan data menggunakan fungsi dari library
        Serial.print("SENSOR DATA: ");
        try {
            display_sensor_data();
        } catch (...) {
            Serial.println("ERROR: Display failed!");
        }
        
        // Info tambahan sistem
        Serial.println("");
        Serial.print("System - Uptime: ");
        Serial.print(millis()/1000);
        Serial.print("s, Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        
        Serial.println("========================================");
        Serial.flush();
    }
    
    delay(100);
}