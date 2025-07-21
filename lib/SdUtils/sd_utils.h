#ifndef SD_UTILS_H
#define SD_UTILS_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <time.h>
#include "FS.h"

// Include unified config file
#include "../include/config.h"

// File definitions untuk queue system
#define QUEUE_FILE "/offline_queue.txt"
#define PROGRESS_FILE "/queue_progress.txt"
#define DAILY_LOG_PREFIX "/vatlog_"

// Struktur data untuk sensor readings
struct VatSensorData {
    bool isValid;
    float distance1;
    float distance2;
    float latitude;
    float longitude;
    float depth;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t satellites;
    float hdop;
};

// Flag status global untuk keamanan operasi SD Card
extern bool isSdCardOk;

// =======================================================
//   FUNGSI KUSTOM timegm
//   Mengkonversi struct tm (UTC) ke Unix timestamp.
//   Dibutuhkan karena timegm() tidak standar di semua platform.
// =======================================================
time_t timegm_custom(struct tm *tm);

// =======================================================
//   CORE SD CARD FUNCTIONS
// =======================================================

/**
 * @brief Menginisialisasi modul SD Card dengan pin yang dikonfigurasi
 * @return true jika berhasil, false jika gagal
 */
bool initSdCard();

/**
 * @brief Cek status SD Card dan tampilkan info
 * @return true jika SD Card OK, false jika bermasalah
 */
bool checkSdCardStatus();

/**
 * @brief Format ulang SD Card (HATI-HATI: Menghapus semua data!)
 * @return true jika berhasil
 */
bool formatSdCard();

// =======================================================
//   DAILY LOGGING FUNCTIONS
// =======================================================

/**
 * @brief Menulis data sensor ke log harian di SD Card dalam format waktu WIB (UTC+7)
 * @param data Struktur data sensor yang akan disimpan
 */
void writeToDailyLog(const VatSensorData& data);

/**
 * @brief Buat header file CSV jika file baru
 * @param filePath Path ke file CSV
 */
void createCsvHeader(const char* filePath);

/**
 * @brief Generate nama file log berdasarkan tanggal
 * @param year Tahun
 * @param month Bulan
 * @param day Hari
 * @return String path file log
 */
String generateLogFileName(int year, int month, int day);

// =======================================================
//   OFFLINE QUEUE FUNCTIONS
// =======================================================

/**
 * @brief Membaca posisi byte terakhir dari file progres
 * @return Posisi byte terakhir yang telah di-sync
 */
unsigned long readProgress();

/**
 * @brief Menulis posisi byte baru ke file progres
 * @param position Posisi byte baru
 */
void writeProgress(unsigned long position);

/**
 * @brief Memeriksa apakah file antrean offline ada dan tidak kosong
 * @return true jika ada data yang belum di-sync
 */
bool isOfflineQueueNotEmpty();

/**
 * @brief Menambahkan satu baris payload JSON ke file antrean offline
 * @param payload JSON string yang akan ditambahkan ke queue
 */
void addToOfflineQueue(const char* payload);

/**
 * @brief Membaca baris berikutnya dari antrean berdasarkan pointer progres
 * @return String JSON payload atau empty string jika queue kosong
 */
String readNextLineFromQueue();

/**
 * @brief Mengembalikan pointer ke posisi sebelumnya jika pengiriman gagal
 * @param line String yang gagal dikirim
 */
void revertProgress(const String& line);

/**
 * @brief Bersihkan file queue setelah semua data berhasil di-sync
 */
void clearOfflineQueue();

// =======================================================
//   UTILITY FUNCTIONS
// =======================================================

/**
 * @brief Konversi VatSensorData ke JSON string
 * @param data Struktur data sensor
 * @return JSON string
 */
String sensorDataToJson(const VatSensorData& data);

/**
 * @brief Get formatted timestamp WIB (UTC+7)
 * @param data Struktur data sensor dengan info waktu
 * @return String timestamp format ISO 8601
 */
String getWibTimestamp(const VatSensorData& data);

/**
 * @brief Hitung ukuran queue file dalam bytes
 * @return Ukuran file queue dalam bytes
 */
size_t getQueueFileSize();

/**
 * @brief Hitung jumlah baris dalam queue file
 * @return Jumlah baris yang belum di-sync
 */
int getQueueLineCount();

/**
 * @brief Tampilkan statistik SD Card dan queue
 */
void printSdCardStats();

/**
 * @brief Backup critical data ke multiple locations
 * @param data Struktur data sensor
 */
void backupCriticalData(const VatSensorData& data);

// =======================================================
//   MAINTENANCE FUNCTIONS
// =======================================================

/**
 * @brief Bersihkan log file yang sudah terlalu lama (> 30 hari)
 * @param daysToKeep Jumlah hari untuk menyimpan log (default: 30)
 */
void cleanupOldLogs(int daysToKeep = 30);

/**
 * @brief Kompres log file untuk menghemat space
 * @param filePath Path ke file yang akan dikompres
 */
bool compressLogFile(const char* filePath);

/**
 * @brief Export semua data ke file untuk backup eksternal
 * @param exportPath Path file export
 * @return true jika berhasil
 */
bool exportAllData(const char* exportPath);

#endif // SD_UTILS_H
