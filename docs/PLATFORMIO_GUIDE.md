# PlatformIO Configuration Guide

## 📋 Available Environments

Proyek ini memiliki 3 environment yang bisa dipilih:

### 1. **`default`** - Versi Aktif (main.cpp)
- Menggunakan file `src/main.cpp` yang sedang aktif
- Support both WiFi dan GSM libraries
- Environment default saat build tanpa parameter

### 2. **`wifi`** - Versi WiFi Only  
- Menggunakan file `src/main_wifi.cpp`
- Hanya include WiFi libraries
- Optimized untuk koneksi WiFi

### 3. **`gsm`** - Versi GSM Only
- Menggunakan file `src/main_gsm.cpp` 
- Include GSM/GPRS libraries
- Optimized untuk koneksi GSM

## 🚀 Cara Build & Upload

### Build Default Environment:
```bash
pio run
pio run --target upload
```

### Build WiFi Version:
```bash
pio run -e wifi
pio run -e wifi --target upload
```

### Build GSM Version:
```bash
pio run -e gsm  
pio run -e gsm --target upload
```

### Build All Environments:
```bash
pio run -e wifi -e gsm -e default
```

## 📡 Monitor Serial

### Monitor Default:
```bash
pio device monitor
```

### Monitor Specific Environment:
```bash
pio device monitor -e wifi
pio device monitor -e gsm
```

## 🔧 Build Flags Explanation

### Common Flags:
- `-I include`: Include directory untuk header files
- `-D CONFIG_ARDUHAL_LOG_DEFAULT_LEVEL`: Arduino log level
- `-D CORE_DEBUG_LEVEL=3`: ESP32 debug level

### WiFi Specific:
- `-D USE_WIFI=1`: Enable WiFi features
- `-D USE_GSM=0`: Disable GSM features

### GSM Specific:
- `-D TINY_GSM_MODEM_SIM800`: GSM modem type
- `-D USE_WIFI=0`: Disable WiFi features  
- `-D USE_GSM=1`: Enable GSM features

## 📦 Libraries per Environment

### WiFi Environment:
- TinyGPSPlus (GPS parsing)
- ArduinoJson (JSON handling)
- Built-in WiFi libraries

### GSM Environment:
- TinyGPSPlus (GPS parsing)
- ArduinoJson (JSON handling)
- TinyGSM (GSM communication)
- EspSoftwareSerial (Serial communication)

## 🔄 Switching Between Versions

### Method 1: Use Different Environments
```bash
# Build WiFi version
pio run -e wifi --target upload

# Build GSM version  
pio run -e gsm --target upload
```

### Method 2: Copy Files (untuk development)
```bash
# Switch to WiFi
cp src/main_wifi.cpp src/main.cpp
pio run --target upload

# Switch to GSM
cp src/main_gsm.cpp src/main.cpp
pio run --target upload
```

## 🛠️ Development Tips

### Clean Build:
```bash
pio run --target clean
pio run -e wifi --target clean
```

### Check Dependencies:
```bash
pio pkg list
pio pkg list -e wifi
```

### Update Libraries:
```bash
pio pkg update
```

### Library Installation:
```bash
pio pkg install
```

## 📊 Build Size Comparison

Environment yang lebih spesifik akan menghasilkan binary yang lebih kecil:

- **default**: ~1.2MB (include semua libraries)
- **wifi**: ~1.0MB (tanpa GSM libraries)  
- **gsm**: ~1.1MB (tanpa WiFi optimization)

## 🚨 Troubleshooting

### Build Error pada Library:
```bash
pio pkg uninstall <library_name>
pio pkg install <library_name>
```

### Environment Conflict:
```bash
pio run --target clean
rm -rf .pio
pio run
```

### Serial Monitor Issues:
```bash
pio device list
pio device monitor --port <specific_port>
```
