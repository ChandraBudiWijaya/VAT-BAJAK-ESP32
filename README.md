# VAT Subsoil Monitor (GSM) - Project Documentation

## 📋 Deskripsi Proyek

VAT (Vibrating Anchor Tool) Subsoil Monitor adalah sistem monitoring kedalaman tanah menggunakan sensor ultrasonik yang terintegrasi dengan teknologi GSM untuk pengiriman data real-time ke server API. Sistem ini dirancang khusus untuk monitoring alat bajak dan kedalaman tanah dalam aplikasi pertanian.

## 🔧 Hardware Requirements

### Komponen Utama:
- **ESP32 LilyGO T-Display** - Mikrokontroler utama
- **SIM800L GSM Module** - Komunikasi data ke server
- **GPS Module** - Tracking posisi geografis
- **2x Ultrasonic Sensors** - Pengukuran jarak/kedalaman
- **SD Card Module** - Penyimpanan data lokal
- **2x LED Indicators** - Indikator status sistem

### Pin Configuration:

#### LED Indicators
- LED1: Pin 12
- LED2: Pin 25

#### GSM SIM800L Module
- RST: Pin 5
- PWKEY: Pin 4
- POWER_ON: Pin 23
- TX: Pin 27
- RX: Pin 26

#### GPS Module
- RX: Pin 22
- TX: Pin 21

#### Ultrasonic Sensors
- Sensor 1 TX: Pin 32
- Sensor 1 RX: Pin 33
- Sensor 2 TX: Pin 19
- Sensor 2 RX: Pin 18

#### SD Card Module
- MOSI: Pin 13
- MISO: Pin 15
- SCLK: Pin 14
- CS: Pin 2

## 📡 Software Features

### Core Functionality:
1. **Real-time Data Collection**
   - GPS position tracking
   - Dual ultrasonic distance measurement
   - Automated depth calculation

2. **Data Storage**
   - Local SD card logging
   - Daily log file generation
   - CSV format with timestamps

3. **GSM Communication**
   - GPRS connectivity
   - RESTful API data transmission
   - JSON payload format

4. **Visual Indicators**
   - LED status indicators
   - Distance-based feedback

## 🚀 Quick Start Guide

### 1. Environment Setup
```bash
# Install PlatformIO
pip install platformio

# Clone or download project
cd "VAT BAJAK GGF"

# Build project
pio run

# Upload to device
pio run --target upload
```

### 2. Configuration
Edit `include/config.h` untuk mengonfigurasi:
- APN kartu SIM
- URL API endpoint
- Device ID
- Pin assignments

### 3. API Configuration
Default API endpoint: `https://api-vatsubsoil-dev.ggfsystem.com/subsoils`

Format data yang dikirim:
```json
{
  "type": "sensor",
  "deviceId": "BJK0001",
  "gps": {
    "lat": -6.123456,
    "lon": 106.123456,
    "alt": 100.5,
    "sog": 0,
    "cog": 0
  },
  "ultrasonic": {
    "dist1": 25.4,
    "dist2": 23.8
  },
  "timestamp": "2025-01-15T10:30:00"
}
```

## 📁 Project Structure

```
VAT BAJAK GGF/
├── include/
│   ├── config.h              # Konfigurasi utama sistem
│   └── README
├── lib/
│   ├── ApiHandler/           # Modul komunikasi GSM & API
│   │   ├── api_handler.h
│   │   └── api_handler.cpp
│   ├── indicators/           # Modul LED indicators
│   │   ├── indicators.h
│   │   └── indicators.cpp
│   ├── SdManager/           # Modul manajemen SD card
│   │   ├── sd_manager.h
│   │   └── sd_manager.cpp
│   ├── VatSensor/           # Modul sensor dan GPS
│   │   ├── sensors.h
│   │   └── sensors.cpp
│   └── README
├── src/
│   └── main.cpp             # Program utama
├── test/
│   └── README
├── platformio.ini           # Konfigurasi PlatformIO
└── README.md               # Dokumentasi ini
```

## ⚙️ Configuration Details

### GSM Settings
```cpp
static const char APN[] = "M2MAUTOTRONIC";
static const char GPRS_USER[] = "";
static const char GPRS_PASS[] = "";
```

### API Settings
```cpp
static const char* API_URL = "https://api-vatsubsoil-dev.ggfsystem.com/subsoils";
static const char* DEVICE_ID = "BJK0001";
```

### Timing Intervals
```cpp
static const unsigned long SAVE_SD_INTERVAL = 1000;   // 1 detik
static const unsigned long POST_INTERVAL = 20000;     // 20 detik
```

## 🔄 System Workflow

1. **Initialization**
   - Setup LED indicators
   - Initialize SD card
   - Configure sensors dan GPS
   - Setup GSM modem

2. **Main Loop**
   - Baca data GPS kontinyu
   - Baca sensor ultrasonik
   - Update LED indicators
   - Simpan data ke SD card (setiap 1 detik)
   - Kirim data ke API (setiap 20 detik)

3. **Data Processing**
   - Validasi data GPS
   - Kalkulasi kedalaman
   - Format timestamp
   - Generate JSON payload

## 🛠️ Troubleshooting

### Common Issues:

1. **GPS tidak lock**
   - Pastikan GPS memiliki pandangan langit yang jelas
   - Tunggu beberapa menit untuk cold start

2. **GSM tidak connect**
   - Cek signal strength
   - Verifikasi APN settings
   - Pastikan kartu SIM aktif

3. **SD card error**
   - Format SD card ke FAT32
   - Cek koneksi wiring
   - Pastikan SD card tidak write-protected

4. **Sensor readings tidak akurat**
   - Cek koneksi sensor
   - Pastikan tidak ada interferensi
   - Kalibrasi jika diperlukan

## 📊 Data Format

### SD Card Log Format (CSV)
```
timestamp,distance1,distance2,latitude,longitude,depth
2025-01-15T10:30:00,25.4,23.8,-6.123456,106.123456,24.6
```

### API Payload Format (JSON)
```json
{
  "type": "sensor",
  "deviceId": "BJK0001",
  "gps": {
    "lat": -6.123456,
    "lon": 106.123456,
    "alt": 100.5,
    "sog": 0,
    "cog": 0
  },
  "ultrasonic": {
    "dist1": 25.4,
    "dist2": 23.8
  },
  "timestamp": "2025-01-15T10:30:00"
}
```

## 🔐 Security Considerations

- Gunakan HTTPS untuk API communication
- Implementasi authentication jika diperlukan
- Secure storage untuk credentials
- Regular firmware updates

## 📈 Performance Metrics

- **GPS Update Rate**: 1 Hz
- **Sensor Sampling**: Continuous
- **Data Logging**: 1 second intervals
- **API Transmission**: 20 second intervals
- **Power Consumption**: ~200mA (dengan GSM aktif)

## 🤝 Contributing

1. Fork the project
2. Create feature branch
3. Commit changes
4. Push to branch
5. Create Pull Request

## 📄 License

[Specify your license here]

## 📞 Support

Untuk support dan pertanyaan:
- Email: [your-email@domain.com]
- GitHub Issues: [repository-url]

---

**Last Updated**: July 16, 2025
**Version**: 1.0.0
**Author**: [Your Name]
