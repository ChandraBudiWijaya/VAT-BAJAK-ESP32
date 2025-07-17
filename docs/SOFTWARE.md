# Software Development Guide - VAT Subsoil Monitor

## 🏗️ Architecture Overview

### System Architecture
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Hardware      │    │   Application   │    │   External      │
│   Abstraction   │←→  │   Logic         │←→  │   Services      │
│   Layer         │    │   Layer         │    │   Layer         │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       │                        │                        │
   ┌───────┐              ┌─────────┐              ┌─────────┐
   │Sensors│              │Main Loop│              │API Server│
   │  GPS  │              │Data Proc│              │SD Storage│
   │  GSM  │              │Error Hdl│              │         │
   │SD Card│              │         │              │         │
   └───────┘              └─────────┘              └─────────┘
```

### Module Structure
```
src/
├── main.cpp                 # Entry point & main loop
├── lib/
│   ├── VatSensor/          # Sensor & GPS management
│   ├── ApiHandler/         # GSM & API communication
│   ├── SdManager/          # Local storage management
│   └── indicators/         # LED status indicators
└── include/
    └── config.h            # System configuration
```

## 📋 Development Environment

### Prerequisites
- **PlatformIO IDE** (VS Code extension recommended)
- **Git** (for version control)
- **ESP32 Toolchain** (auto-installed by PlatformIO)

### Dependencies
```ini
lib_deps = 
    mikalhart/TinyGPSPlus@^1.1.0      # GPS parsing
    vshymanskyy/TinyGSM@^0.12.0       # GSM communication
    plerup/EspSoftwareSerial@^8.2.0   # Additional serial ports
```

### Build Configuration
```ini
[env:lilygo-t-display]
platform = espressif32
board = lilygo-t-display
framework = arduino

build_flags = 
    -D TINY_GSM_MODEM_SIM800    # GSM modem type
    -I include                   # Include directory

monitor_speed = 115200
upload_speed = 921600
```

## 🏛️ Code Architecture

### 1. Main Application (main.cpp)

```cpp
// Application lifecycle
void setup() {
    // 1. Initialize serial communication
    // 2. Setup hardware modules
    // 3. Perform system checks
    // 4. LED startup sequence
}

void loop() {
    // 1. Read sensor data continuously
    // 2. Update LED indicators
    // 3. Save data to SD (interval-based)
    // 4. Transmit to API (interval-based)
    // 5. Handle errors gracefully
}
```

### 2. Sensor Management (VatSensor/)

#### sensors.h
```cpp
// Global variables
extern float distance1, distance2;
extern float latitude, longitude, altitude;

// Function prototypes
void setup_sensors();
void read_gps_data();
void read_ultrasonic_sensors();
float calculate_depth();
void display_sensor_data();
```

#### sensors.cpp Features
- **GPS Parsing**: TinyGPS++ integration
- **Dual Sensor Reading**: HardwareSerial + SoftwareSerial
- **Data Validation**: Range checking and error handling
- **Depth Calculation**: Algorithmic processing

### 3. Communication (ApiHandler/)

#### api_handler.h
```cpp
// GSM Functions
void setup_modem();
bool connect_gprs();

// API Functions
void sendDataToAPI(float dist1, float dist2, 
                   float lat, float lon, float alt, 
                   const char* timestamp);
```

#### api_handler.cpp Features
- **Modem Initialization**: Power management and AT commands
- **GPRS Connection**: Network registration and data context
- **HTTP Client**: Manual HTTP POST implementation
- **Error Recovery**: Automatic retry mechanisms

### 4. Storage Management (SdManager/)

#### sd_manager.h
```cpp
// SD Card Functions
void setup_sd_card();
void check_and_create_logfile(const char* filename);
void save_data_to_sd(const char* filename, const char* timestamp,
                     float dist1, float dist2, float lat, float lon, float depth);
```

#### sd_manager.cpp Features
- **File System**: FAT32 compatibility
- **Daily Logs**: Automatic file rotation
- **CSV Format**: Structured data storage
- **Error Handling**: Graceful degradation

### 5. Visual Feedback (indicators/)

#### indicators.h
```cpp
// LED Control
void setup_leds();
void update_leds(float distance1, float distance2);
```

#### indicators.cpp Features
- **Status Indication**: System state visualization
- **Distance Feedback**: Proportional LED response
- **Error Signaling**: Fault condition indication

## 🔧 Configuration Management

### config.h Structure
```cpp
// Network Configuration
static const char APN[] = "M2MAUTOTRONIC";
static const char GPRS_USER[] = "";
static const char GPRS_PASS[] = "";

// API Configuration
static const char* API_URL = "https://api-vatsubsoil-dev.ggfsystem.com/subsoils";
static const char* DEVICE_ID = "BJK0001";

// Hardware Pin Definitions
#define LED1_PIN 12
#define LED2_PIN 25
#define MODEM_RST 5
// ... (all pin definitions)

// Timing Configuration
static const unsigned long SAVE_SD_INTERVAL = 1000;
static const unsigned long POST_INTERVAL = 20000;
```

## 🔄 Data Flow

### 1. Sensor Data Collection
```
GPS Module → UART → TinyGPS++ → GPS Coordinates
Ultrasonic 1 → HardwareSerial → Distance Processing
Ultrasonic 2 → SoftwareSerial → Distance Processing
```

### 2. Data Processing
```
Raw Sensor Data → Validation → Calculation → Formatting
```

### 3. Data Storage
```
Processed Data → CSV Format → SD Card → Daily Log Files
```

### 4. Data Transmission
```
JSON Payload → HTTP POST → GPRS → API Server
```

## 🛡️ Error Handling Strategy

### 1. Sensor Errors
```cpp
// GPS validation
if (gps.date.isValid() && gps.time.isValid() && gps.location.isValid()) {
    // Process valid data
} else {
    // Handle invalid GPS data
}

// Ultrasonic validation
if (data_buffer[3] == checksum) {
    // Use valid reading
} else {
    // Discard corrupted data
}
```

### 2. Communication Errors
```cpp
// GPRS connection
if (!connect_gprs()) {
    Serial.println("Cannot send data, no GPRS connection.");
    return; // Skip transmission, continue operation
}

// HTTP response handling
if (client.connect(host.c_str(), 443)) {
    // Send data
} else {
    Serial.println("Connection to server failed");
    // Log error, continue operation
}
```

### 3. Storage Errors
```cpp
// SD card initialization
if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    // Continue without SD logging
}
```

## 🔍 Debugging & Testing

### Serial Debug Output
```cpp
#define DEBUG_MODE 1

#if DEBUG_MODE
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif
```

### Test Modes
```cpp
// Test individual components
#define TEST_GPS_ONLY 0
#define TEST_SENSORS_ONLY 0
#define TEST_GSM_ONLY 0
#define TEST_SD_ONLY 0
```

### Unit Testing Framework
```cpp
// Example test structure
void test_gps_parsing() {
    // Inject test NMEA sentences
    // Verify parsing results
    // Assert expected outcomes
}

void test_sensor_validation() {
    // Test with known good/bad data
    // Verify validation logic
    // Check error handling
}
```

## 📊 Performance Optimization

### 1. Memory Management
```cpp
// Avoid dynamic allocation
static char buffer[256];  // Static buffers

// Use const for read-only data
static const char* const error_messages[] = {
    "GPS Error", "GSM Error", "SD Error"
};
```

### 2. Power Management
```cpp
// Sleep modes for battery conservation
void enterLightSleep(uint32_t milliseconds) {
    esp_sleep_enable_timer_wakeup(milliseconds * 1000);
    esp_light_sleep_start();
}

// Selective module power control
void powerDownUnusedModules() {
    // Disable unused peripherals
    // Reduce clock speeds when possible
}
```

### 3. Communication Optimization
```cpp
// Batch data transmission
struct DataBatch {
    SensorReading readings[10];
    uint8_t count;
};

// Compress JSON payload
String createCompactPayload(const SensorData& data) {
    // Minimize JSON size
    // Remove unnecessary whitespace
    // Use shorter field names if needed
}
```

## 🔄 Version Control & Deployment

### Git Workflow
```bash
# Feature development
git checkout -b feature/new-sensor-support
git commit -m "Add support for new ultrasonic sensor"
git push origin feature/new-sensor-support

# Release preparation
git checkout main
git merge feature/new-sensor-support
git tag v1.0.1
git push origin main --tags
```

### OTA Updates (Future Enhancement)
```cpp
// Over-the-air update capability
#include <ArduinoOTA.h>

void setup_ota() {
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("Start updating " + type);
    });
    
    ArduinoOTA.begin();
}
```

## 📈 Code Quality

### Coding Standards
- **Naming**: Use descriptive variable names
- **Comments**: Document complex algorithms
- **Formatting**: Consistent indentation (4 spaces)
- **Functions**: Keep functions small and focused

### Code Review Checklist
- [ ] Memory leaks checked
- [ ] Error handling implemented
- [ ] Performance optimized
- [ ] Documentation updated
- [ ] Tests passing

## 🚀 Building & Deployment

### Build Commands
```bash
# Clean build
pio run --target clean

# Compile
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

### Release Process
1. Update version numbers
2. Run full test suite
3. Build release binary
4. Create release notes
5. Tag release in Git
6. Deploy to devices

## 📞 Development Support

### Resources
- **PlatformIO Documentation**: https://docs.platformio.org/
- **ESP32 Arduino Core**: https://github.com/espressif/arduino-esp32
- **TinyGPS++ Library**: https://github.com/mikalhart/TinyGPSPlus
- **TinyGSM Library**: https://github.com/vshymanskyy/TinyGSM

### Community
- **ESP32 Forum**: https://esp32.com/
- **Arduino Forum**: https://forum.arduino.cc/
- **PlatformIO Community**: https://community.platformio.org/

---

**Software Version**: 1.0.0
**Last Updated**: July 16, 2025
