# ESP32 VAT Subsoil Monitor - Dual Network System

## 📋 Project Overview

VAT (Vibrating Anchor Tool) Subsoil Monitor adalah sistem monitoring kedalaman tanah menggunakan sensor ultrasonik dengan **dual network capability** - mendukung koneksi **WiFi** dan **GSM/GPRS** untuk pengiriman data real-time ke server API. Sistem ini dirancang untuk monitoring alat bajak dan kedalaman tanah dalam aplikasi pertanian.

## 🌐 Network Configurations

Proyek ini telah dipisahkan menjadi **3 versi** berdasarkan jenis koneksi:

### 📁 File Structure
```
src/
├── main.cpp           # Versi asli (aktif) - Hybrid
├── main_wifi.cpp      # ✅ Versi WiFi Only
└── main_gsm.cpp       # ✅ Versi GSM Only

include/
├── config_wifi.h      # 🔧 Konfigurasi WiFi & API
├── config_gsm.h       # 🔧 Konfigurasi GSM & API
└── config.h           # 🔧 Common config & SD pins

lib/
├── ApiHandler/        # 🆕 Modular API handlers
│   ├── wifi_api_handler.h/.cpp  # WiFi API dengan SD backup
│   └── gsm_api_handler.h/.cpp   # GSM API dengan SD backup
├── VatSensor/         # 📏 Custom sensor library
├── indicators/        # 💡 LED indicator library
└── SdUtils/           # 💾 Advanced SD card utilities
    ├── sd_utils.h     # 🆕 Comprehensive SD management
    └── sd_utils.cpp   # 🆕 Offline queue & sync system

backup/
├── main_wifi_with_sd.cpp    # 🆕 Example WiFi + SD integration
├── main_gsm_with_sd.cpp     # 🆕 Example GSM + SD integration
└── main_advanced_sd.cpp     # 🆕 Advanced SD Utils demo

docs/
├── NETWORK_SETUP.md   # 📖 Panduan Network Setup
└── PLATFORMIO_GUIDE.md # 📖 Panduan PlatformIO
```

### 🚀 Quick Start Commands

#### WiFi Version:
```bash
pio run -e wifi --target upload
pio device monitor -e wifi
```

#### GSM Version:
```bash
pio run -e gsm --target upload  
pio device monitor -e gsm
```

#### Default Version (Hybrid):
```bash
pio run --target upload
pio device monitor
```

## 💾 SD Card Utils & Offline System

### Advanced Features:

#### 📝 Automatic Data Logging:
- **Daily CSV Files**: `/vatlog_YYYY-MM-DD.csv` dengan headers otomatis
- **Structured Data**: timestamp_wib, device_id, distance1, distance2, latitude, longitude, depth, satellites, hdop
- **WIB Timezone**: Timestamp dalam format Indonesia (UTC+7)

#### 🔄 Offline Queue System:
- **Automatic Backup**: Semua data tersimpan ke SD saat API gagal/offline
- **Smart Sync**: Auto-sync ke server ketika koneksi pulih
- **Progress Tracking**: Resume sync dari posisi terakhir jika terputus
- **Rate Limiting**: Mencegah overload server saat sync batch data

#### 🛠️ Maintenance & Monitoring:
- **Storage Stats**: Monitor ukuran file, space tersisa, queue status
- **Data Integrity**: Verification dan health check otomatis  
- **Error Recovery**: Robust handling untuk corruption/failures
- **Cleanup Tools**: Auto-cleanup old logs, compression utilities

### SD Card Commands:
```bash
# Monitor SD operations
SDCARD    # Check SD card status & reinitialize if needed
STATS     # Display storage statistics & queue info
SYNC      # Manual trigger offline data sync to server
```

## 🔧 Hardware Requirements

### Komponen Utama:
- **ESP32 Development Board** - Mikrokontroler utama
- **GPS Module** - Tracking posisi geografis  
- **2x Ultrasonic Sensors** - Pengukuran jarak/kedalaman
- **2x LED Indicators** - Indikator status sistem

### Network Hardware (Pilih Salah Satu):

#### Untuk WiFi Version:
- ✅ **WiFi Router** dengan internet access
- ✅ **ESP32** (built-in WiFi)

#### Untuk GSM Version:
- ✅ **SIM800L/A6/A7 GSM Module** 
- ✅ **SIM Card** dengan paket data aktif
- ✅ **Power Supply 2A+** (GSM butuh power besar)

### Pin Configuration:

#### LED Indicators
- LED1: Pin 12
- LED2: Pin 25

#### GPS Module  
- RX: Pin 16
- TX: Pin 17

#### Ultrasonic Sensors
- Sensor 1 Trigger: Pin 32
- Sensor 1 Echo: Pin 33
- Sensor 2 Trigger: Pin 19  
- Sensor 2 Echo: Pin 18

#### GSM Module (Untuk GSM Version - LILYGO T-Call SIM800L)
- MODEM_RX: Pin 26
- MODEM_TX: Pin 27  
- MODEM_PWKEY: Pin 4
- MODEM_RST: Pin 5
- MODEM_POWER_ON: Pin 23

#### SD Card Module (Optional - untuk data backup)
- SD_MOSI: Pin 13
- SD_MISO: Pin 15
- SD_SCLK: Pin 14
- SD_CS: Pin 2

## 📡 Software Features

### Core Functionality:

#### 🔧 Sensor Monitoring:
- **GPS Position Tracking** - Real-time location dengan TinyGPS++
- **Dual Ultrasonic Sensors** - Pengukuran jarak presisi  
- **Automated Depth Calculation** - Kalkulasi kedalaman tanah
- **LED Status Indicators** - Visual feedback sistem

#### 📡 Network Communication:

##### WiFi Version (`main_wifi.cpp`):
- ✅ **WiFi Auto-Connect** dengan reconnection
- ✅ **NTP Time Sync** untuk timestamp akurat
- ✅ **HTTP POST** ke REST API
- ✅ **Real-time Data Upload** setiap 60 detik
- ✅ Commands: `WIFI`, `API`, `SENSOR`, `TEST`

##### GSM Version (`main_gsm.cpp`):  
- ✅ **GSM/GPRS Auto-Connect** dengan AT commands
- ✅ **HTTP via GSM** menggunakan TinyGSM
- ✅ **SIM Card Detection** dan network registration
- ✅ **Robust Error Handling** dengan retry mechanism
- ✅ Commands: `GSM`, `API`, `SENSOR`, `TEST`

#### 📊 Data Management:
- **JSON Payload Format** - Structured data untuk API
- **Timestamp Integration** - ISO 8601 format dengan WIB timezone
- **Error Logging** - Comprehensive error tracking
- **Memory Management** - Optimized untuk ESP32
- **💾 Advanced SD Utils** - Offline queue & sync system
  - Daily CSV logs dengan auto-header creation
  - Offline queue untuk data backup saat koneksi terputus
  - Auto-sync ketika koneksi pulih dengan progress tracking
  - Data integrity verification dan storage monitoring

## 🚀 Quick Setup Guide

### 1. Choose Your Network Version

#### Option A: WiFi Setup
```bash
# 1. Edit WiFi config
# Edit include/config_wifi.h:
#   - WIFI_SSID = "YourWiFiName"
#   - WIFI_PASSWORD = "YourPassword"

# 2. Build & Upload
pio run -e wifi --target upload

# 3. Monitor
pio device monitor -e wifi
```

#### Option B: GSM Setup  
```bash
# 1. Edit GSM config
# Edit include/config_gsm.h:
#   - GSM_APN = "internet" (sesuai provider)
#   - Check pin connections (RX=4, TX=2)

# 2. Hardware wiring GSM module
# 3. Build & Upload
pio run -e gsm --target upload

# 4. Monitor
pio device monitor -e gsm
```

### 2. API Configuration

Edit di file config yang sesuai:
```cpp
const char* API_URL = "https://your-api-server.com/api/subsoils";
const char* DEVICE_ID = "BJK0001";
```

## 🛠️ Development

### Environment Commands:

#### Build All Versions:
```bash
pio run -e wifi -e gsm -e default
```

#### Clean Build:
```bash
pio run --target clean
pio run -e wifi --target clean
```

#### Library Management:
```bash
pio pkg list                    # List installed packages
pio pkg update                  # Update all packages
pio pkg install                 # Install dependencies
```

### Serial Commands:

#### WiFi Version:
- `WIFI` - Check WiFi status & signal strength
- `API` - Test API connection dengan sample data
- `SENSOR` - Check sensor readings
- `TIME` - Show current NTP time

#### GSM Version:
- `GSM` - Check GSM status & signal quality  
- `POWER` - Manual GSM power cycle (reset modem)
- `API` - Test API via GSM connection
- `SENSOR` - Check sensor readings
- `TEST` - Run full system test

## 📊 API Integration

### JSON Payload Format:
```json
{
  "device_id": "BJK0001",
  "data": {
    "distance1": 23.5,
    "distance2": 32.1,
    "latitude": -6.175392,
    "longitude": 106.827153
  },
  "timestamp": "2024-01-01T10:30:45+07:00"
}
```

### API Endpoints:
- **POST** `https://api-vatsubsoil-dev.ggfsystem.com/subsoils`
- **Content-Type**: `application/json`
- **Method**: HTTP POST via WiFi atau GSM

## 🔍 Monitoring & Debugging

### Real-time Monitoring:
```bash
# Monitor dengan filter
pio device monitor --filter=colorize

# Monitor specific environment
pio device monitor -e wifi
pio device monitor -e gsm
```

### Debug Output Example:
```
========================================
📊 VAT SUBSOIL MONITOR - Reading #25
========================================
📏 REAL SENSOR DATA:
  Distance 1: 23.5 cm ✅
  Distance 2: 32.1 cm ✅
🌊 Calculated Depth: 74.2 cm
🛰️ GPS MODULE:
  Status: FIXED ✅ (8 satellites)
  Latitude: -6.175392
  Longitude: 106.827153
📡 WiFi: Connected ✅
🚀 POSTING DATA TO API!
✅ DATA POSTED SUCCESSFULLY
========================================
```

## 🔧 Troubleshooting

### WiFi Issues:
```bash
# Check WiFi credentials in config_wifi.h
# Test with: WIFI command in serial monitor
# Check router connectivity
```

### GSM Issues:
```bash
# Check SIM card credit/data package
# Verify APN settings in config_gsm.h  
# Check GSM module power supply (min 2A)
# Test with: GSM command in serial monitor
# Test network with: NETWORK command for diagnostics

# Common HTTP Error Codes:
# 601: Network error or DNS resolution failed
# 602: DNS resolution error  
# 603: TCP/UDP connection error
# 604: TCP/UDP connection timeout

# Debug steps:
# 1. Use GSM command to check signal strength
# 2. Use NETWORK command to test DNS and HTTP
# 3. Use POWER command to reset modem if needed
```

### Sensor Issues:
```bash
# Check pin connections
# Test with: SENSOR command
# Verify power supply to sensors
```

## 📁 Project Structure

```
VAT-BAJAK-ESP32/
├── 📁 src/
│   ├── main.cpp              # Current active (hybrid)
│   ├── main_wifi.cpp         # WiFi-only version
│   └── main_gsm.cpp          # GSM-only version
├── 📁 include/
│   ├── config_wifi.h         # WiFi configuration
│   ├── config_gsm.h          # GSM configuration  
│   └── config.h              # Common config
├── 📁 lib/
│   ├── VatSensor/            # Custom sensor library
│   │   ├── sensors.h
│   │   └── sensors.cpp
│   └── indicators/           # LED indicator library
│       ├── indicators.h
│       └── indicators.cpp
├── 📁 docs/
│   ├── NETWORK_SETUP.md      # Network setup guide
│   ├── PLATFORMIO_GUIDE.md   # PlatformIO guide
│   ├── API.md                # API documentation
│   └── HARDWARE.md           # Hardware guide
├── 📁 backup/
│   ├── main_simple.cpp       # Simple test version
│   └── main_backup.cpp       # Backup version
├── platformio.ini            # Multi-environment config
├── README.md                 # This file
└── LICENSE                   # MIT License
```

## 🚀 Deployment Options

### 1. WiFi Deployment:
✅ Indoor applications  
✅ Workshop/lab environment  
✅ Areas dengan WiFi coverage stabil  
✅ Power dari wall adapter  

### 2. GSM Deployment:
✅ Remote field operations  
✅ Outdoor farming applications  
✅ Areas tanpa WiFi coverage  
✅ Mobile/vehicle installations  
✅ Battery-powered operations  

## 📝 License

MIT License - see [LICENSE](LICENSE) file for details.

## 👥 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

## 📞 Support

- 📖 **Documentation**: Check `docs/` folder
- 🐛 **Issues**: GitHub Issues
- 💬 **Questions**: GitHub Discussions

---

**🔥 Ready untuk production dengan dual network support!**
