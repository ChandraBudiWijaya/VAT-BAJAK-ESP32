# Serial Monitor Troubleshooting Guide

## 🔍 Problem Analysis

Berdasarkan output yang Anda terima:
```
load:0x40080400,len:3028
entry 0x400805e4
E (200) esp_core_dump_flash: Core dum�␆"�х�check failed:
Calculated checksum='7108291c'
Image checksum='ffffffff'

--- VAT Subsoil Monitor (GSM) ---
Initializing SD card... m�␀
```

## 🚨 Issues Identified

1. **Core Dump Error**: Previous crash/corruption
2. **Garbled Characters**: Serial communication issue
3. **System Hang**: Program stops at SD card initialization

## 🛠️ Step-by-Step Solutions

### Solution 1: Serial Communication Fix

#### Check Baud Rate
```bash
# In platformio.ini, ensure:
monitor_speed = 115200

# In VS Code terminal:
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe device monitor --baud 115200
```

#### Reset Serial Buffer
Add to code:
```cpp
void setup() {
    Serial.begin(115200);
    delay(3000);
    while(Serial.available()) Serial.read(); // Clear buffer
    Serial.println("=== SYSTEM START ===");
}
```

### Solution 2: Hardware Issues

#### SD Card Problems
- **Check SD card insertion** (properly seated)
- **Format SD card to FAT32** (32GB max recommended)
- **Check wiring**:
  ```
  ESP32 → SD Module
  Pin 13 → MOSI
  Pin 15 → MISO  
  Pin 14 → SCLK
  Pin 2  → CS
  3.3V   → VCC
  GND    → GND
  ```

#### Power Supply Issues
- **Check 3.3V and 5V rails**
- **Ensure adequate current capacity**
- **SIM800L needs 4V @ 2A peak**

### Solution 3: ESP32 Recovery

#### Flash Erase (Complete Reset)
```bash
# Option 1: PlatformIO
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target erase

# Option 2: esptool
esptool.py --chip esp32 --port COM[X] erase_flash
```

#### Upload Minimal Test Code
```cpp
void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("ESP32 Basic Test - Working!");
}

void loop() {
    static int count = 0;
    Serial.print("Count: ");
    Serial.println(++count);
    delay(1000);
}
```

### Solution 4: Debug Mode

#### Enable ESP32 Debug Output
Add to platformio.ini:
```ini
build_flags = 
    -D CORE_DEBUG_LEVEL=5
    -D TINY_GSM_MODEM_SIM800
    -I include

monitor_filters = esp32_exception_decoder
```

#### Progressive Hardware Testing
1. **Test 1**: Basic serial only
2. **Test 2**: Add LEDs
3. **Test 3**: Add SD card
4. **Test 4**: Add sensors
5. **Test 5**: Add GSM module

## 🔧 Quick Fixes to Try

### Fix 1: Simple Serial Test
Replace main.cpp temporarily:
```cpp
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("ESP32 Serial Test - OK!");
    Serial.println("If you see this, serial is working.");
}

void loop() {
    static unsigned long timer = 0;
    if (millis() - timer > 1000) {
        timer = millis();
        Serial.print("Uptime: ");
        Serial.print(millis()/1000);
        Serial.println(" seconds");
    }
}
```

### Fix 2: Check USB Connection
- **Try different USB cable**
- **Try different USB port**
- **Check Device Manager** for COM port
- **Update/reinstall CH340/CP210x drivers**

### Fix 3: Hardware Isolation
Disconnect all external hardware:
- Remove SD card
- Disconnect sensors
- Disconnect GSM module
- Keep only ESP32 + USB

### Fix 4: Reset ESP32
- **Press RESET button** while monitoring
- **Hold BOOT button** + press RESET for download mode
- **Power cycle** (disconnect/reconnect USB)

## 📋 Diagnostic Commands

### Check PlatformIO
```bash
# List devices
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe device list

# Monitor with specific port
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe device monitor --port COM3

# Clean build
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target clean
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run
```

### Windows Device Manager
1. Open Device Manager
2. Look for "Ports (COM & LPT)"
3. Find ESP32 device
4. Note COM port number
5. Right-click → Update driver if needed

## ⚡ Emergency Recovery

### Complete Factory Reset
```bash
# 1. Erase everything
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target erase

# 2. Upload bootloader
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target bootloader

# 3. Upload minimal firmware
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target upload
```

### Alternative Tools
- **Arduino IDE**: Try uploading via Arduino IDE
- **ESP32 Flash Tool**: Use Espressif's official tool
- **esptool.py**: Direct flash tool

## 🎯 Expected Results

After fixes, you should see:
```
=================================
VAT Subsoil Monitor Starting...
=================================
Free Heap: 298896
CPU Frequency: 240 MHz
1. Setting up LEDs...
   LEDs initialized ✓
2. Setting up SD card...
   SD card setup SKIPPED for debugging
...
```

## 📞 If Still Not Working

### Contact Information
- **Hardware Issue**: Check connections step by step
- **Software Issue**: Try minimal test code first
- **Driver Issue**: Reinstall USB drivers
- **ESP32 Issue**: Try different ESP32 board

### Additional Resources
- ESP32 Troubleshooting Guide
- PlatformIO Documentation
- ESP32 Arduino Core Issues on GitHub

---

**Next Step**: Try Solution 1 (Serial Fix) first, then Solution 3 (ESP32 Recovery) if needed.
