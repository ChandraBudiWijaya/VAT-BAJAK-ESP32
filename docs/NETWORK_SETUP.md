# ESP32 VAT Subsoil Monitor - Network Configurations

Proyek ini telah dipisahkan menjadi dua versi berdasarkan jenis koneksi network:

## 📁 File Structure

```
src/
├── main.cpp           # Versi asli (aktif)
├── main_wifi.cpp      # Versi WiFi
└── main_gsm.cpp       # Versi GSM

include/
├── config_wifi.h      # Konfigurasi WiFi
└── config_gsm.h       # Konfigurasi GSM
```

## 🔧 Cara Penggunaan

### 1. Untuk Menggunakan Versi WiFi

1. **Copy file WiFi ke main.cpp:**
   ```bash
   cp src/main_wifi.cpp src/main.cpp
   ```

2. **Edit konfigurasi WiFi:**
   - Buka `include/config_wifi.h`
   - Ubah `WIFI_SSID` dan `WIFI_PASSWORD` sesuai WiFi Anda
   - Ubah `API_URL` jika diperlukan

3. **Build dan upload:**
   ```bash
   pio run --target upload
   ```

### 2. Untuk Menggunakan Versi GSM

1. **Copy file GSM ke main.cpp:**
   ```bash
   cp src/main_gsm.cpp src/main.cpp
   ```

2. **Edit konfigurasi GSM:**
   - Buka `include/config_gsm.h`
   - Ubah `GSM_APN` sesuai provider Anda (contoh: "internet", "indosatgprs", "telkomsel")
   - Sesuaikan pin `GSM_RX_PIN` dan `GSM_TX_PIN` dengan wiring Anda
   - Ubah `API_URL` jika diperlukan

3. **Koneksi Hardware GSM (LILYGO T-Call SIM800L):**
   ```
   ESP32 Pin    <->   SIM800L Function
   GPIO 26      <->   MODEM_RX (ESP32 TX -> Modem RX)
   GPIO 27      <->   MODEM_TX (ESP32 RX <- Modem TX)
   GPIO 4       <->   MODEM_PWKEY (Power Key)
   GPIO 5       <->   MODEM_RST (Reset)
   GPIO 23      <->   MODEM_POWER_ON (Power Control)
   3.3V/5V      <->   VCC (check board requirements)
   GND          <->   GND
   ```

4. **Build dan upload:**
   ```bash
   pio run --target upload
   ```

## ⚙️ Konfigurasi Network

### WiFi Configuration (`config_wifi.h`)
- `WIFI_SSID`: Nama WiFi network
- `WIFI_PASSWORD`: Password WiFi
- `WIFI_TIMEOUT`: Timeout koneksi WiFi (default: 10 detik)
- `NTP_SERVER1/2`: Server NTP untuk sinkronisasi waktu

### GSM Configuration (`config_gsm.h`)
- `GSM_APN`: Access Point Name dari provider
- `GSM_RX_PIN/GSM_TX_PIN`: Pin komunikasi dengan modul GSM
- `GSM_TIMEOUT`: Timeout untuk operasi GSM
- `HTTP_TIMEOUT`: Timeout untuk HTTP request

## 📡 Fitur Each Version

### WiFi Version Features:
✅ Koneksi WiFi otomatis dengan reconnection  
✅ NTP time synchronization  
✅ HTTP POST ke API  
✅ Real-time monitoring  
✅ Serial commands: `WIFI`, `API`, `SENSOR`  

### GSM Version Features:
✅ Koneksi GSM/GPRS otomatis dengan reconnection  
✅ HTTP POST via AT commands  
✅ SIM card detection  
✅ Network registration check  
✅ Serial commands: `GSM`, `API`, `SENSOR`  

## 🛠️ Development Commands

### Serial Monitor Commands:
- `WIFI` / `GSM`: Check network status
- `API`: Test API connection
- `SENSOR`: Check sensor readings
- `TEST`: Run system test
- `LED`: Test LED indicators

## 📋 Requirements

### WiFi Version:
- WiFi router dengan akses internet
- ESP32 dengan WiFi capability

### GSM Version:
- Modul GSM (SIM800L/A6/A7)
- SIM card dengan paket data aktif
- Power supply yang cukup untuk modul GSM (minimal 2A)

## 🚨 Important Notes

1. **GSM Power**: Modul GSM membutuhkan power yang stabil dan cukup besar
2. **SIM Card**: Pastikan SIM card memiliki pulsa/paket data
3. **APN Settings**: Sesuaikan APN dengan provider Anda
4. **Pin Configuration**: Pastikan pin GSM tidak konflik dengan sensor lain

## 🔄 Switching Between Versions

Untuk beralih antar versi, cukup copy file yang diinginkan ke `main.cpp`:

```bash
# Untuk WiFi
cp src/main_wifi.cpp src/main.cpp

# Untuk GSM  
cp src/main_gsm.cpp src/main.cpp
```

Lalu build dan upload ulang firmware.
