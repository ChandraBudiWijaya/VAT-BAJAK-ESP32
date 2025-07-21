# ESP32 VAT SUBSOIL MONITOR

> **Status:** ✅ PRODUCTION READY  
> **Current Version:** GSM TinyGPS Implementation  
> **Last Updated:** July 21, 2025

## 📋 Project Overview

Sistem monitoring tanah subsoil menggunakan ESP32 LILYGO T-Call dengan sensor ultrasonik dan GPS. Mendukung koneksi WiFi dan GSM untuk mengirim data ke production API.

## 🚀 Quick Start

### Build & Upload
```bash
# WiFi Version
pio run -e wifi -t upload

# GSM Version (Current Active)
pio run -e gsm -t upload
```

### Serial Commands
```
test       - Send test data to production API
status     - Show system and network status  
reconnect  - Reconnect network
sensors    - Read sensors manually
production - Force send current data to production
```

## 📁 Project Structure (Cleaned)

```
├── src/
│   ├── main_gsm.cpp        # 🔥 ACTIVE: GSM Production Version
│   └── main_wifi.cpp       # 📶 WiFi Production Version
├── lib/
│   ├── ApiHandler/         # Network communication handlers
│   │   ├── gsm_api_handler.h/.cpp   # 🔥 GSM API handler (TinyGSM based)
│   │   └── wifi_api_handler.h/.cpp  # 📶 WiFi API handler
│   ├── VatSensor/         # Sensor reading functions  
│   ├── indicators/        # LED control
│   └── SdManager/         # SD card operations
├── include/
│   └── config.h           # 🔧 Unified configuration
├── backup/                # 📦 Archive of old implementations
└── docs/                  # 📚 Documentation
```

## 🔧 Configuration

**Current API Target:** `https://api-vatsubsoil-dev.ggfsystem.com/subsoils`

**GSM Settings:**
- APN: M2MAUTOTRONIC  
- Module: SIM800L (LILYGO T-Call)
- Baudrate: 9600

**WiFi Settings:**
- SSID: Kiwi Gejrot
- Timeout: 10 seconds

## 📊 Data Format

```json
{
  "type": "sensor",
  "deviceId": "BJK0001", 
  "gps": {
    "lat": -4.823621,
    "lon": 105.226395,
    "alt": 100.0,
    "sog": 0,
    "cog": 0
  },
  "ultrasonic": {
    "dist1": 5.20,
    "dist2": 52.80
  },
  "timestamp": "2025-07-21T09:52:00+07:00"
}
```

## 🎯 Hardware Setup

- **ESP32:** LILYGO T-Call with SIM800L
- **Sensors:** 2x Ultrasonic (UART)
- **GPS:** Built-in module  
- **LEDs:** Status indicators (Pin 12, 25)
- **SD Card:** Data logging (optional)

## 📈 Production Status

- ✅ GSM connectivity stable
- ✅ Real sensor integration working
- ✅ Production API communication tested
- ✅ LED indicators functional
- ✅ Serial command interface active

## 🔍 Troubleshooting

1. **GSM Connection Issues:** Use `reconnect` command
2. **Sensor Reading Problems:** Check UART connections  
3. **API Errors:** Verify network connectivity with `status`
4. **Build Issues:** Ensure PlatformIO environment is clean

## 🧹 Cleanup Summary (July 21, 2025)

### ✅ Files Removed:
- `src/main.cpp` (empty)
- `src/main_gsm_backup.cpp` (outdated)
- `include/config_gsm.h` (empty/unused)
- `lib/ApiHandler/gsm_api_handler.cpp/h` (old AT-command version)

### 📁 Files Moved to Backup:
- `src/main_gsm_tinygps.cpp` → `backup/main_gsm_tinygps_working.cpp`
- `src/main_gsm_tinygps.txt` → `backup/main_gsm_tinygps_backup.txt`

### 🔄 Files Renamed for Clarity:
- `lib/ApiHandler/gsm_tinygps_handler.*` → `lib/ApiHandler/gsm_api_handler.*`
- `GSMTinyGpsHandler` class → `GSMApiHandler` class

### 🔧 Simplified Configuration:
- Unified `platformio.ini` with only essential environments
- Single `config.h` file for all configurations
- Streamlined build process

### 📊 Current Active Files:
- **GSM Production:** `src/main_gsm.cpp`
- **WiFi Production:** `src/main_wifi.cpp`
- **Configuration:** `include/config.h`
- **Libraries:** All in `lib/` folder
- **Backups:** All in `backup/` folder
