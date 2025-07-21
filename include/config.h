#ifndef CONFIG_H
#define CONFIG_H

// --- COMMON API CONFIGURATION ---
static const char* API_URL = "https://api-vatsubsoil-dev.ggfsystem.com/subsoils";
static const char* DEVICE_ID = "BJK0001";

// --- GSM MODULE CONFIGURATION ---
// GSM Module Configuration - LILYGO T-Call SIM800L
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26

// GSM Serial Configuration
static const int GSM_RX_PIN = MODEM_RX;  // 26
static const int GSM_TX_PIN = MODEM_TX;  // 27
static const long GSM_BAUD_RATE = 9600;  // SIM800L default baudrate

// GSM Network Configuration (Production API Configuration - Working Format)
static const char* GSM_APN = "M2MAUTOTRONIC";
static const char* GSM_USER = "";
static const char* GSM_PASS = "";

// GSM Production API Configuration (Tested & Working)
static const char* GSM_SERVER = "api-vatsubsoil-dev.ggfsystem.com";
static const char* GSM_RESOURCE = "/subsoils";
static const int GSM_PORT = 443;  // HTTPS port untuk API production

// GSM Settings
static const unsigned long GSM_TIMEOUT = 30000; // 30 seconds
static const unsigned long GSM_RETRY_INTERVAL = 60000; // 60 seconds
static const int GSM_MAX_RETRIES = 3;

// --- WIFI CONFIGURATION ---
// WiFi Configuration
static const char* WIFI_SSID = "Kiwi Gejrot";
static const char* WIFI_PASSWORD = "1122334455";

// WiFi Settings
static const unsigned long WIFI_TIMEOUT = 10000; // 10 seconds
static const unsigned long WIFI_RETRY_INTERVAL = 30000; // 30 seconds

// NTP Configuration
static const char* NTP_SERVER1 = "pool.ntp.org";
static const char* NTP_SERVER2 = "time.nist.gov";
static const int TIMEZONE_OFFSET = 7 * 3600; // UTC+7 (WIB)

// --- COMMON PIN CONFIGURATION ---
// Pin untuk LED Indikator
#define LED1_PIN 12
#define LED2_PIN 25

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

// --- COMMON TIMING CONFIGURATION ---
static const unsigned long SAVE_SD_INTERVAL = 1000;  // Simpan ke SD setiap 1 detik
static const unsigned long POST_INTERVAL = 20000;    // Kirim ke API setiap 20 detik
static const unsigned long HTTP_TIMEOUT = 20000;     // 20 seconds

// --- SENSOR CONFIGURATION ---
#define SENSOR_BAUD_RATE 9600
#define SENSOR_READ_DELAY 4  // Delay untuk stabilitas sensor reading (ms)

// --- DEBUGGING CONFIGURATION ---
#define SENSOR_DEBUG_INTERVAL 10000  // Debug sensor setiap 10 detik
#define GPS_DEBUG_INTERVAL 15000     // Debug GPS setiap 15 detik

#endif // CONFIG_H
