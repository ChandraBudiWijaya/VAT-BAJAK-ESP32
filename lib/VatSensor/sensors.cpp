#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "../../include/config.h"
#include "sensors.h"

// --- OBJEK SENSOR & GPS ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);     // Use Serial2 for GPS seperti kode yang bekerja
SoftwareSerial sensorSerial1(32, 33); // Pin order seperti kode yang bekerja: (TX, RX)
SoftwareSerial sensorSerial2(19, 18); // Pin order seperti kode yang bekerja: (TX, RX)

// --- VARIABEL DATA SENSOR ---
float distance1 = 0.0;
float distance2 = 0.0;
float latitude = 0.0;
float longitude = 0.0;
float altitude = 0.0;

byte data_buffer1[4];
byte CS1;
byte data_buffer2[4];
byte CS2;

void setup_sensors() {
    Serial.println("🔧 Starting sensor setup...");
    
    // Setup GPS Serial - menggunakan konfigurasi dari config.h
    Serial.print("📍 Setting up GPS on Serial2, pins RX:");
    Serial.print(GPS_RX_PIN);
    Serial.print(", TX:");
    Serial.print(GPS_TX_PIN);
    Serial.print(", Baud:");
    Serial.println(GPS_BAUD);
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    delay(100);
    
    // Setup Sensor Serial - menggunakan konstanta dari config.h
    Serial.println("📏 Setting up Ultrasonic Sensors...");
    Serial.print("  Sensor1: TX=");
    Serial.print(SENSOR1_TX_PIN);
    Serial.print(", RX=");
    Serial.println(SENSOR1_RX_PIN);
    Serial.print("  Sensor2: TX=");
    Serial.print(SENSOR2_TX_PIN);
    Serial.print(", RX=");
    Serial.println(SENSOR2_RX_PIN);
    Serial.print("  Baud Rate: ");
    Serial.println(SENSOR_BAUD_RATE);
    
    sensorSerial1.begin(SENSOR_BAUD_RATE);
    sensorSerial2.begin(SENSOR_BAUD_RATE);
    delay(100);
    
    Serial.println("✅ Sensor setup completed!");
}

void read_ultrasonic_sensors() {
    static unsigned long lastDebug = 0;
    bool debug = (millis() - lastDebug > SENSOR_DEBUG_INTERVAL);
    
    if (debug) {
        Serial.println("📏 Reading ultrasonic sensors...");
        lastDebug = millis();
    }
    
    // Baca Sensor 1 - mengikuti kode yang bekerja
    if (sensorSerial1.available() > 0) {
        delay(SENSOR_READ_DELAY); // Delay dari config.h untuk stabilitas
        if (sensorSerial1.read() == 0xff) {
            data_buffer1[0] = 0xff;
            for (int i = 1; i < 4; i++) {
                data_buffer1[i] = sensorSerial1.read();
            }
            CS1 = data_buffer1[0] + data_buffer1[1] + data_buffer1[2];
            if (data_buffer1[3] == CS1) {
                distance1 = (data_buffer1[1] << 8) + data_buffer1[2];
                distance1 = distance1 / 10.0; // Convert to cm
                if (debug) {
                    Serial.print("✅ Sensor1 valid reading: ");
                    Serial.println(distance1);
                }
            } else if (debug) {
                Serial.println("❌ Sensor1 checksum failed");
            }
        }
    }

    // Baca Sensor 2 - mengikuti kode yang bekerja
    if (sensorSerial2.available() > 0) {
        delay(SENSOR_READ_DELAY); // Delay dari config.h untuk stabilitas
        if (sensorSerial2.read() == 0xff) {
            data_buffer2[0] = 0xff;
            for (int i = 1; i < 4; i++) {
                data_buffer2[i] = sensorSerial2.read();
            }
            CS2 = data_buffer2[0] + data_buffer2[1] + data_buffer2[2];
            if (data_buffer2[3] == CS2) {
                distance2 = (data_buffer2[1] << 8) + data_buffer2[2];
                distance2 = distance2 / 10.0; // Convert to cm
                if (debug) {
                    Serial.print("✅ Sensor2 valid reading: ");
                    Serial.println(distance2);
                }
            } else if (debug) {
                Serial.println("❌ Sensor2 checksum failed");
            }
        }
    }
}

void read_gps_data() {
    static unsigned long lastDebug = 0;
    bool debug = (millis() - lastDebug > GPS_DEBUG_INTERVAL);
    
    // Mengikuti kode yang bekerja - baca semua data GPS yang tersedia
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            // GPS sentence berhasil di-decode
            if (debug) {
            }
        }
    }
    
    // Update variabel global jika GPS valid - mengikuti kode yang bekerja
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        altitude = gps.altitude.meters();
        
        if (gps.location.isUpdated() && debug) {
            Serial.print("✅ GPS location updated: ");
            Serial.print(latitude, 6);
            Serial.print(", ");
            Serial.println(longitude, 6);
            lastDebug = millis();
        }
    }
}

float calculate_depth() {
    // Rumus Kedalaman
    return 2.84 * distance2 - 16.6;
}

void display_sensor_data() {
    read_gps_data(); // pastikan data GPS terbaru
    Serial.print("D1: ");
    Serial.print(distance1);
    Serial.print(" cm, D2: ");
    Serial.print(distance2);
    Serial.print(" cm, Lat: ");
    Serial.print(latitude, 7);
    Serial.print(", Lon: ");
    Serial.print(longitude, 7);
    Serial.print(", Kedalaman: ");
    Serial.println(calculate_depth());
}