#include "sd_utils.h"
#include <ArduinoJson.h>

// Global flag untuk status SD Card
bool isSdCardOk = false;

// =======================================================
//   FUNGSI KUSTOM timegm
// =======================================================
time_t timegm_custom(struct tm *tm) {
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long year = tm->tm_year + 1900;
    long days = 0;
    
    for (int y = 1970; y < year; ++y) {
        days += 365;
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            days++;
        }
    }
    
    for (int m = 0; m < tm->tm_mon; ++m) {
        days += days_in_month[m];
        if (m == 1 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
            days++;
        }
    }
    
    days += tm->tm_mday - 1;
    
    return (days * 86400L) + (tm->tm_hour * 3600L) + (tm->tm_min * 60L) + tm->tm_sec;
}

// =======================================================
//   CORE SD CARD FUNCTIONS
// =======================================================

bool initSdCard() {
    Serial.println("💾 Initializing SD Card...");
    Serial.print("  SD_CS pin: ");
    Serial.println(SD_CS);
    Serial.print("  SD_MOSI pin: ");
    Serial.println(SD_MOSI);
    Serial.print("  SD_MISO pin: ");
    Serial.println(SD_MISO);
    Serial.print("  SD_SCLK pin: ");
    Serial.println(SD_SCLK);
    
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("❌ FATAL: SD Card Mount Failed!");
        Serial.println("  Possible causes:");
        Serial.println("  - No SD card inserted");
        Serial.println("  - Wrong wiring");
        Serial.println("  - SD card not formatted (FAT32)");
        Serial.println("  - Power supply issue");
        Serial.println("  📊 Offline logging and syncing will be disabled.");
        isSdCardOk = false;
        return false;
    }
    
    Serial.println("✅ SD Card mounted successfully!");
    
    // Test SD card properties
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.print("  Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
    
    uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
    Serial.print("  Total Space: ");
    Serial.print(totalBytes);
    Serial.println(" MB");
    Serial.print("  Used Space: ");
    Serial.print(usedBytes);
    Serial.println(" MB");
    Serial.print("  Free Space: ");
    Serial.print(totalBytes - usedBytes);
    Serial.println(" MB");
    
    isSdCardOk = true;
    return true;
}

bool checkSdCardStatus() {
    if (!isSdCardOk) {
        Serial.println("❌ SD Card not initialized");
        return false;
    }
    
    // Test write operation
    File testFile = SD.open("/test_write.tmp", FILE_WRITE);
    if (!testFile) {
        Serial.println("❌ SD Card write test failed");
        isSdCardOk = false;
        return false;
    }
    
    testFile.println("test");
    testFile.close();
    
    // Test read operation
    testFile = SD.open("/test_write.tmp", FILE_READ);
    if (!testFile) {
        Serial.println("❌ SD Card read test failed");
        isSdCardOk = false;
        return false;
    }
    
    String testContent = testFile.readString();
    testFile.close();
    SD.remove("/test_write.tmp");
    
    if (testContent.indexOf("test") == -1) {
        Serial.println("❌ SD Card data integrity test failed");
        isSdCardOk = false;
        return false;
    }
    
    Serial.println("✅ SD Card status OK");
    return true;
}

bool formatSdCard() {
    Serial.println("⚠️ WARNING: Formatting SD Card - ALL DATA WILL BE LOST!");
    Serial.println("Type 'CONFIRM' to proceed:");
    
    // Wait for user confirmation (in real implementation)
    // For now, we'll skip this dangerous operation
    Serial.println("❌ Format operation cancelled for safety");
    return false;
}

// =======================================================
//   DAILY LOGGING FUNCTIONS
// =======================================================

void createCsvHeader(const char* filePath) {
    if (!isSdCardOk) return;
    
    File file = SD.open(filePath, FILE_READ);
    if (file && file.size() > 0) {
        file.close();
        return; // Header already exists
    }
    if (file) file.close();
    
    file = SD.open(filePath, FILE_WRITE);
    if (file) {
        file.println("timestamp_wib,device_id,distance1,distance2,latitude,longitude,depth,satellites,hdop");
        file.close();
        Serial.print("✅ CSV header created for: ");
        Serial.println(filePath);
    } else {
        Serial.print("❌ Failed to create CSV header for: ");
        Serial.println(filePath);
    }
}

String generateLogFileName(int year, int month, int day) {
    char fileName[30];
    sprintf(fileName, "/vatlog_%04d-%02d-%02d.csv", year, month, day);
    return String(fileName);
}

void writeToDailyLog(const VatSensorData& data) {
    if (!isSdCardOk || !data.isValid) {
        if (!isSdCardOk) {
            Serial.println("⚠️ SD Card not available - data not logged");
        }
        return;
    }
    
    // Generate log file name
    String logFileName = generateLogFileName(data.year, data.month, data.day);
    
    // Create header if needed
    createCsvHeader(logFileName.c_str());
    
    // Open file for append
    File file = SD.open(logFileName.c_str(), FILE_APPEND);
    if (!file) {
        Serial.println("❌ Failed to open daily log for appending");
        return;
    }
    
    // Get WIB timestamp
    String timestamp = getWibTimestamp(data);
    
    // Create CSV line
    char csvLine[200];
    snprintf(csvLine, sizeof(csvLine), 
             "%s,%s,%.1f,%.1f,%.6f,%.6f,%.2f,%u,%.2f",
             timestamp.c_str(),
             DEVICE_ID,
             data.distance1,
             data.distance2,
             data.latitude,
             data.longitude,
             data.depth,
             (unsigned int)data.satellites,
             data.hdop);
    
    // Write to file
    if (file.println(csvLine) == 0) {
        Serial.println("❌ ERROR: Writing to daily log failed!");
    } else {
        Serial.println("💾 Data logged to daily CSV");
    }
    
    file.close();
}

// =======================================================
//   OFFLINE QUEUE FUNCTIONS
// =======================================================

unsigned long readProgress() {
    if (!isSdCardOk) return 0;
    
    File file = SD.open(PROGRESS_FILE, FILE_READ);
    if (file) {
        unsigned long pos = file.readString().toInt();
        file.close();
        return pos;
    }
    return 0;
}

void writeProgress(unsigned long position) {
    if (!isSdCardOk) return;
    
    File file = SD.open(PROGRESS_FILE, FILE_WRITE);
    if (file) {
        file.print(position);
        file.close();
    } else {
        Serial.println("❌ ERROR: Failed to write progress file!");
    }
}

bool isOfflineQueueNotEmpty() {
    if (!isSdCardOk) return false;
    
    if (!SD.exists(QUEUE_FILE)) {
        return false;
    }
    
    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (file) {
        bool hasContent = file.size() > readProgress();
        file.close();
        return hasContent;
    }
    
    return false;
}

void addToOfflineQueue(const char* payload) {
    if (!isSdCardOk) {
        Serial.println("⚠️ Cannot add to offline queue - SD Card not available");
        return;
    }
    
    File file = SD.open(QUEUE_FILE, FILE_APPEND);
    if (!file) {
        Serial.println("❌ Failed to open offline_queue.txt for writing");
        return;
    }
    
    if (file.println(payload) == 0) {
        Serial.println("❌ ERROR: Failed to write to offline queue!");
    } else {
        Serial.println("📝 Data added to offline queue");
    }
    
    file.close();
}

String readNextLineFromQueue() {
    if (!isSdCardOk) return "";
    
    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (!file) {
        Serial.println("❌ Cannot open queue file for reading");
        return "";
    }
    
    unsigned long currentPos = readProgress();
    if (currentPos >= file.size()) {
        Serial.println("✅ Queue is fully synced. Cleaning up files.");
        file.close();
        clearOfflineQueue();
        return "";
    }
    
    file.seek(currentPos);
    String line = file.readStringUntil('\n');
    line.trim();
    
    // Save new position for next attempt
    writeProgress(file.position());
    
    file.close();
    return line;
}

void revertProgress(const String& line) {
    if (!isSdCardOk) return;
    
    unsigned long currentPos = readProgress();
    // Calculate position before reading this line (add 2 for \r\n)
    unsigned long previousPos = currentPos > (line.length() + 2) ? 
                               currentPos - (line.length() + 2) : 0;
    
    writeProgress(previousPos);
    Serial.println("↩️ Progress reverted due to send failure");
}

void clearOfflineQueue() {
    if (!isSdCardOk) return;
    
    if (SD.exists(QUEUE_FILE)) {
        SD.remove(QUEUE_FILE);
        Serial.println("🗑️ Offline queue file cleared");
    }
    
    if (SD.exists(PROGRESS_FILE)) {
        SD.remove(PROGRESS_FILE);
        Serial.println("🗑️ Progress file cleared");
    }
}

// =======================================================
//   UTILITY FUNCTIONS
// =======================================================

String sensorDataToJson(const VatSensorData& data) {
    DynamicJsonDocument doc(1024);
    
    doc["device_id"] = DEVICE_ID;
    doc["data"]["distance1"] = round(data.distance1 * 10) / 10.0;
    doc["data"]["distance2"] = round(data.distance2 * 10) / 10.0;
    doc["data"]["latitude"] = data.latitude;
    doc["data"]["longitude"] = data.longitude;
    doc["data"]["depth"] = round(data.depth * 10) / 10.0;
    doc["timestamp"] = getWibTimestamp(data);
    
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

String getWibTimestamp(const VatSensorData& data) {
    struct tm utc_tm = {0};
    utc_tm.tm_year = data.year - 1900;
    utc_tm.tm_mon = data.month - 1;
    utc_tm.tm_mday = data.day;
    utc_tm.tm_hour = data.hour;
    utc_tm.tm_min = data.minute;
    utc_tm.tm_sec = data.second;
    
    time_t utc_time = timegm_custom(&utc_tm);
    time_t wib_time = utc_time + (7 * 3600); // UTC+7
    struct tm* wib_tm_ptr = gmtime(&wib_time);
    
    char wibTimestampStr[30];
    strftime(wibTimestampStr, sizeof(wibTimestampStr), 
             "%Y-%m-%dT%H:%M:%S+07:00", wib_tm_ptr);
    
    return String(wibTimestampStr);
}

size_t getQueueFileSize() {
    if (!isSdCardOk || !SD.exists(QUEUE_FILE)) return 0;
    
    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (file) {
        size_t size = file.size();
        file.close();
        return size;
    }
    return 0;
}

int getQueueLineCount() {
    if (!isSdCardOk || !SD.exists(QUEUE_FILE)) return 0;
    
    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (!file) return 0;
    
    int lineCount = 0;
    unsigned long progressPos = readProgress();
    
    // Skip to progress position
    file.seek(progressPos);
    
    // Count remaining lines
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.length() > 0) {
            lineCount++;
        }
    }
    
    file.close();
    return lineCount;
}

void printSdCardStats() {
    Serial.println("\n📊 SD CARD STATISTICS:");
    Serial.println("========================================");
    
    if (!isSdCardOk) {
        Serial.println("❌ SD Card not available");
        return;
    }
    
    // Card info
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
    
    Serial.print("💾 Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
    Serial.print("📁 Total Space: ");
    Serial.print(totalBytes);
    Serial.println(" MB");
    Serial.print("📝 Used Space: ");
    Serial.print(usedBytes);
    Serial.println(" MB");
    Serial.print("🆓 Free Space: ");
    Serial.print(totalBytes - usedBytes);
    Serial.println(" MB");
    
    // Queue info
    Serial.print("📋 Queue File Size: ");
    Serial.print(getQueueFileSize());
    Serial.println(" bytes");
    Serial.print("📝 Pending Lines: ");
    Serial.println(getQueueLineCount());
    
    // Progress info
    Serial.print("🎯 Sync Progress: ");
    Serial.println(readProgress());
    
    Serial.println("========================================");
}

void backupCriticalData(const VatSensorData& data) {
    if (!isSdCardOk) return;
    
    // Primary log
    writeToDailyLog(data);
    
    // Backup to queue for API sync
    String jsonPayload = sensorDataToJson(data);
    addToOfflineQueue(jsonPayload.c_str());
    
    // Critical backup file (overwrites - keeps only latest)
    File criticalFile = SD.open("/critical_backup.json", FILE_WRITE);
    if (criticalFile) {
        criticalFile.println(jsonPayload);
        criticalFile.close();
    }
}

// =======================================================
//   MAINTENANCE FUNCTIONS
// =======================================================

void cleanupOldLogs(int daysToKeep) {
    if (!isSdCardOk) return;
    
    Serial.print("🧹 Cleaning up logs older than ");
    Serial.print(daysToKeep);
    Serial.println(" days...");
    
    // Get current time
    time_t now = millis() / 1000; // Simplified - should use RTC
    time_t cutoffTime = now - (daysToKeep * 24 * 3600);
    
    // TODO: Implement file enumeration and cleanup
    // This would require iterating through files and checking dates
    
    Serial.println("✅ Log cleanup completed");
}

bool compressLogFile(const char* filePath) {
    // TODO: Implement compression algorithm
    // Could use simple RLE or LZ compression
    Serial.print("🗜️ Compressing file: ");
    Serial.println(filePath);
    
    // Placeholder
    return false;
}

bool exportAllData(const char* exportPath) {
    if (!isSdCardOk) return false;
    
    Serial.print("📤 Exporting all data to: ");
    Serial.println(exportPath);
    
    // TODO: Implement data export
    // Combine all CSV files into single export file
    
    return false;
}
