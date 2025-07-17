#ifndef CONFIG_H
#define CONFIG_H

// --- PENGATURAN MODEM GSM ---
// Note: TINY_GSM_MODEM_SIM800 is defined in platformio.ini build_flags

// Ganti APN sesuai dengan kartu SIM yang Anda gunakan
static const char APN[] = "M2MAUTOTRONIC";
static const char GPRS_USER[] = "";
static const char GPRS_PASS[] = "";

// --- PENGATURAN API ---
static const char* API_URL = "https://api-vatsubsoil-dev.ggfsystem.com/subsoils";
static const char* DEVICE_ID = "BJK0001"; // Ganti dengan ID perangkat Anda

// --- PENGATURAN PIN (SESUAI DENGAN test_sensor_bajak.ino) ---
// Pin untuk LED Indikator
#define LED1_PIN 12
#define LED2_PIN 25

// Pin untuk modem SIM800L
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26

// Pin untuk GPS (menggunakan HardwareSerial2)
#define GPS_RX_PIN 22
#define GPS_TX_PIN 21
#define GPS_BAUD 9600

// Pin untuk Sensor Ultrasonik (menggunakan SoftwareSerial)
// Sensor1: SoftwareSerial(TX=32, RX=33)
#define SENSOR1_TX_PIN 32
#define SENSOR1_RX_PIN 33
// Sensor2: SoftwareSerial(TX=19, RX=18) 
#define SENSOR2_TX_PIN 19
#define SENSOR2_RX_PIN 18

// Pin untuk SD Card
#define SD_MOSI 13
#define SD_MISO 15
#define SD_SCLK 14
#define SD_CS 2

// --- PENGATURAN INTERVAL ---
static const unsigned long SAVE_SD_INTERVAL = 1000;  // Simpan ke SD setiap 1 detik
static const unsigned long POST_INTERVAL = 20000;    // Kirim ke API setiap 20 detik

// --- PENGATURAN SENSOR ---
#define SENSOR_BAUD_RATE 9600
#define SENSOR_READ_DELAY 4  // Delay untuk stabilitas sensor reading (ms)

// --- PENGATURAN DEBUGGING ---
#define SENSOR_DEBUG_INTERVAL 10000  // Debug sensor setiap 10 detik
#define GPS_DEBUG_INTERVAL 15000     // Debug GPS setiap 15 detik

#endif // CONFIG_H