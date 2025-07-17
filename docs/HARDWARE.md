# Hardware Setup Guide - VAT Subsoil Monitor

## 🔧 Komponen yang Dibutuhkan

### 1. Mikrokontroler
- **ESP32 LilyGO T-Display**
  - Built-in TFT display
  - Wi-Fi & Bluetooth capability
  - Multiple GPIO pins
  - Operating voltage: 3.3V

### 2. GSM Module
- **SIM800L GSM/GPRS Module**
  - Quad-band GSM/GPRS
  - Operating voltage: 3.7-4.2V
  - Current consumption: 2A peak
  - Antena GSM diperlukan

### 3. GPS Module
- **Compatible GPS Module** (NEO-6M atau NEO-8M)
  - UART communication
  - Operating voltage: 3.3V
  - Antena GPS diperlukan

### 4. Sensor Ultrasonik
- **2x Ultrasonic Distance Sensors**
  - UART communication
  - Waterproof design recommended
  - Range: 0.2m - 10m
  - Operating voltage: 5V

### 5. Storage
- **MicroSD Card Module**
  - SPI communication
  - Support FAT32 format
  - Class 10 recommended

### 6. Indicators
- **2x LED Indicators**
  - Different colors (Red/Green)
  - 220Ω resistors
  - 3mm or 5mm LEDs

### 7. Power Supply
- **LiPo Battery 3.7V**
  - Capacity: 2000mAh minimum
  - JST connector
  - Built-in protection circuit

### 8. Enclosure
- **Waterproof Enclosure**
  - IP65 rating minimum
  - Size: accommodate all components
  - Cable glands for external connections

## 📐 Wiring Diagram

### ESP32 LilyGO T-Display Pin Connections

```
ESP32 Pin    |    Component      |    Function
-------------|-------------------|------------------
3.3V         |    GPS Module     |    Power (+)
GND          |    GPS Module     |    Ground (-)
GPIO 22      |    GPS Module     |    RX Pin
GPIO 21      |    GPS Module     |    TX Pin
-------------|-------------------|------------------
3.3V         |    SD Module      |    VCC
GND          |    SD Module      |    GND
GPIO 13      |    SD Module      |    MOSI
GPIO 15      |    SD Module      |    MISO
GPIO 14      |    SD Module      |    SCK
GPIO 2       |    SD Module      |    CS
-------------|-------------------|------------------
3.3V         |    LED 1          |    Anode (+)
GPIO 12      |    LED 1          |    Cathode (-)
3.3V         |    LED 2          |    Anode (+)
GPIO 25      |    LED 2          |    Cathode (-)
-------------|-------------------|------------------
5V           |    Sensor 1       |    Power (+)
GND          |    Sensor 1       |    Ground (-)
GPIO 33      |    Sensor 1       |    RX Pin
GPIO 32      |    Sensor 1       |    TX Pin
-------------|-------------------|------------------
5V           |    Sensor 2       |    Power (+)
GND          |    Sensor 2       |    Ground (-)
GPIO 18      |    Sensor 2       |    RX Pin
GPIO 19      |    Sensor 2       |    TX Pin
-------------|-------------------|------------------
4V           |    SIM800L        |    VCC
GND          |    SIM800L        |    GND
GPIO 26      |    SIM800L        |    RX Pin
GPIO 27      |    SIM800L        |    TX Pin
GPIO 4       |    SIM800L        |    PWKEY
GPIO 5       |    SIM800L        |    RST
GPIO 23      |    SIM800L        |    Power Enable
```

## ⚡ Power Management

### Voltage Levels
- **ESP32**: 3.3V (internal regulator from 5V)
- **SIM800L**: 4V (requires external regulator)
- **GPS Module**: 3.3V
- **Ultrasonic Sensors**: 5V
- **SD Card**: 3.3V

### Power Supply Circuit
```
LiPo 3.7V → Boost Converter → 5V Rail → Components
              ↓
           3.3V Regulator → 3.3V Rail → ESP32, GPS, SD
              ↓
           4V Regulator → 4V Rail → SIM800L
```

### Current Consumption Estimates
- **ESP32**: 240mA (active), 10μA (deep sleep)
- **SIM800L**: 300mA (standby), 2A (transmission peak)
- **GPS**: 50mA (acquiring), 25mA (tracking)
- **Sensors**: 20mA each
- **SD Card**: 100mA (write), 50mA (read)
- **LEDs**: 20mA each

**Total Average**: ~650mA
**Peak Consumption**: ~2.5A

## 🔌 Detailed Connection Instructions

### 1. ESP32 LilyGO T-Display Preparation
```
1. Install USB driver for CP2104
2. Test basic functionality
3. Check pin continuity
4. Verify 3.3V and 5V outputs
```

### 2. SIM800L Module Setup
```
1. Insert SIM card (nano SIM)
2. Connect antenna properly
3. Check power requirements (4V, 2A peak)
4. Test AT commands via serial
5. Verify network registration
```

### 3. GPS Module Configuration
```
1. Connect antenna
2. Test with u-center software
3. Configure baud rate (9600)
4. Verify NMEA output
5. Test satellite lock outdoors
```

### 4. Ultrasonic Sensors
```
1. Verify sensor type and protocol
2. Test individual sensors
3. Configure communication parameters
4. Calibrate distance readings
5. Waterproof connections
```

### 5. SD Card Module
```
1. Format SD card to FAT32
2. Test SPI communication
3. Verify file operations
4. Check write protection
```

## 🛠️ Assembly Steps

### Step 1: Breadboard Prototyping
1. Create power rails (3.3V, 5V, GND)
2. Connect ESP32 to breadboard
3. Add power regulation circuits
4. Test basic connectivity

### Step 2: Module Integration
1. Connect GPS module and test
2. Add SIM800L with proper power
3. Connect ultrasonic sensors
4. Add SD card module
5. Connect LED indicators

### Step 3: Testing Phase
1. Test each module individually
2. Verify power consumption
3. Check data communication
4. Test GSM connectivity
5. Verify GPS lock

### Step 4: PCB/Perfboard Assembly
1. Design PCB layout (optional)
2. Solder components to perfboard
3. Add proper connectors
4. Include test points
5. Add mounting holes

### Step 5: Enclosure Assembly
1. Mount PCB in enclosure
2. Add cable glands
3. Connect external antennas
4. Install indicators/switches
5. Seal all openings

## 📊 Testing Checklist

### Power Testing
- [ ] Check all voltage levels
- [ ] Measure current consumption
- [ ] Test battery life
- [ ] Verify power regulation

### Communication Testing
- [ ] UART communication to all modules
- [ ] SPI communication to SD card
- [ ] GSM network registration
- [ ] GPS satellite lock
- [ ] Sensor data reading

### Functional Testing
- [ ] Data logging to SD card
- [ ] API data transmission
- [ ] LED indicator operation
- [ ] Error handling
- [ ] System recovery

### Environmental Testing
- [ ] Operating temperature range
- [ ] Humidity resistance
- [ ] Vibration tolerance
- [ ] Waterproof testing
- [ ] EMI/EMC compliance

## 🚨 Common Issues & Solutions

### Issue 1: SIM800L Not Responding
**Symptoms**: No AT command response
**Solutions**:
- Check power supply (needs 4V, 2A capability)
- Verify baud rate (usually 115200)
- Check wiring (TX/RX crossed)
- Ensure proper antenna connection

### Issue 2: GPS No Fix
**Symptoms**: No satellite lock
**Solutions**:
- Move to open sky area
- Check antenna connection
- Wait for cold start (up to 15 minutes)
- Verify power supply

### Issue 3: SD Card Errors
**Symptoms**: Cannot read/write files
**Solutions**:
- Format to FAT32
- Check SPI wiring
- Verify 3.3V power level
- Test with different SD card

### Issue 4: High Power Consumption
**Symptoms**: Battery drains quickly
**Solutions**:
- Implement sleep modes
- Check for current leaks
- Optimize transmission intervals
- Use power-efficient components

### Issue 5: Sensor Inaccurate Readings
**Symptoms**: Inconsistent distance measurements
**Solutions**:
- Calibrate sensors
- Check mounting stability
- Verify power supply
- Add noise filtering

## 🔧 Maintenance

### Regular Checks
- Clean antenna connections
- Check battery voltage
- Verify SD card health
- Test GSM signal strength
- Calibrate sensors if needed

### Preventive Maintenance
- Replace SD card annually
- Check enclosure seals
- Update firmware as needed
- Monitor data quality
- Replace battery when capacity drops

## 📞 Support Resources

- **ESP32 Documentation**: [Official Espressif Docs]
- **SIM800L Manual**: [SIMCom Official Documentation]
- **TinyGPS++ Library**: [GitHub Repository]
- **TinyGSM Library**: [GitHub Repository]

---

**Hardware Version**: 1.0
**Last Updated**: July 16, 2025
