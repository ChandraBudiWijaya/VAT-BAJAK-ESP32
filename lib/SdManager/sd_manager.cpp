#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "../../include/config.h"
#include "sd_manager.h"

void setup_sd_card() {
    Serial.println("Initializing SD card...");
    Serial.print("  SD_CS pin: ");
    Serial.println(SD_CS);
    Serial.print("  SD_MOSI pin: ");
    Serial.println(SD_MOSI);
    Serial.print("  SD_MISO pin: ");
    Serial.println(SD_MISO);
    Serial.print("  SD_SCLK pin: ");
    Serial.println(SD_SCLK);
    
    Serial.print("  Starting SPI...");
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    Serial.println(" OK");
    
    Serial.print("  Mounting SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println(" FAILED!");
        Serial.println("  SD card mount failed. Possible causes:");
        Serial.println("  - No SD card inserted");
        Serial.println("  - Wrong wiring");
        Serial.println("  - SD card not formatted (FAT32)");
        Serial.println("  - Power supply issue");
        return;
    }
    Serial.println(" SUCCESS!");
    
    // Test SD card
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.print("  SD Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
}

void check_and_create_logfile(const char* filename) {
    if (!SD.exists(filename)) {
        Serial.print("File log hari ini tidak ada, membuat ");
        Serial.println(filename);
        File myFile = SD.open(filename, FILE_WRITE);
        if (myFile) {
            // Tulis header
            myFile.println("Timestamp,Distance1,Distance2,Latitude,Longitude,Kedalaman");
            myFile.close();
            Serial.println("Header berhasil ditulis.");
        } else {
            Serial.println("Gagal membuat file log.");
        }
    }
}

void save_data_to_sd(const char* filename, const char* timestamp, float d1, float d2, float lat, float lon, float depth) {
    File myFile = SD.open(filename, FILE_APPEND);
    if (myFile) {
        myFile.printf("%s,%.1f,%.1f,%.7f,%.7f,%.2f\n", timestamp, d1, d2, lat, lon, depth);
        myFile.close();
        Serial.println("Data disimpan ke SD Card.");
    } else {
        Serial.println("Gagal membuka file di SD card untuk menyimpan data.");
    }
}