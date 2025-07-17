# Deployment & User Guide - VAT Subsoil Monitor

## 🚀 Quick Start Guide

### 1. Initial Setup (5 minutes)

#### Hardware Check
```
✓ ESP32 LilyGO T-Display connected
✓ SIM800L module powered and connected
✓ GPS module with antenna
✓ Ultrasonic sensors mounted
✓ SD card inserted and formatted
✓ LED indicators working
✓ Battery connected and charged
```

#### Software Deployment
```bash
# 1. Connect device via USB
# 2. Open terminal in project directory
cd "VAT BAJAK GGF"

# 3. Upload firmware
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe run --target upload

# 4. Monitor startup
C:\Users\LENOVO\.platformio\penv\Scripts\platformio.exe device monitor
```

#### First Boot Verification
```
Expected Serial Output:
--- VAT Subsoil Monitor (GSM) ---
Initializing SD card...
Initializing modem...
Setting up sensors...
Startup sequence... LED akan berkedip selama 5 detik.
Setup selesai. Memulai loop utama...
```

### 2. Configuration

#### Network Settings (config.h)
```cpp
// Update these for your SIM card provider
static const char APN[] = "YOUR_APN_HERE";        // e.g., "internet"
static const char GPRS_USER[] = "YOUR_USERNAME";  // Usually empty
static const char GPRS_PASS[] = "YOUR_PASSWORD";  // Usually empty
```

#### Device Identity
```cpp
// Change device ID for each unit
static const char* DEVICE_ID = "BJK0001";  // Unique identifier
```

#### API Endpoint (if different)
```cpp
// Update if using different API server
static const char* API_URL = "https://your-api-server.com/subsoils";
```

### 3. Field Deployment

#### Pre-Deployment Checklist
- [ ] GPS gets satellite lock (outdoor test)
- [ ] GSM connects to network
- [ ] Sensors read valid distances
- [ ] SD card logging works
- [ ] API transmission successful
- [ ] Battery fully charged
- [ ] Enclosure properly sealed

#### Installation Steps
1. **Mount sensors** at required positions
2. **Install main unit** in protected location
3. **Connect power** (battery + charging if available)
4. **Position antennas** for optimal signal
5. **Test system** in actual environment
6. **Secure all connections** and cables

## 📊 Operation Guide

### Normal Operation

#### System States
1. **Startup**: LED blinking for 5 seconds
2. **GPS Acquisition**: Waiting for satellite lock
3. **Normal Operation**: Continuous data collection
4. **Data Transmission**: Periodic API uploads
5. **Error State**: LED patterns indicate issues

#### LED Indicators
- **LED1 (Pin 12)**:
  - ON: Distance1 > threshold
  - OFF: Distance1 ≤ threshold
  
- **LED2 (Pin 25)**:
  - ON: Distance2 > threshold  
  - OFF: Distance2 ≤ threshold

#### Data Collection Intervals
- **Sensor Reading**: Continuous
- **SD Card Save**: Every 1 second
- **API Transmission**: Every 20 seconds
- **GPS Update**: Every 1 second

### Monitoring Operation

#### Serial Monitor
Connect via USB to see real-time status:
```
Menunggu data GPS yang valid...
GPS Fixed: Lat=-6.123456, Lon=106.123456
Sensor 1: 25.4 cm, Sensor 2: 23.8 cm
Kedalaman: 24.6 cm
Saving to SD: /file_15_07_2025.csv
--- Waktunya Mengirim Data ke API ---
HTTP Response code: 200
```

#### SD Card Data
Files created daily in format: `/file_DD_MM_YYYY.csv`
```csv
timestamp,distance1,distance2,latitude,longitude,depth
2025-07-15T10:30:00,25.4,23.8,-6.123456,106.123456,24.6
2025-07-15T10:30:01,25.2,23.9,-6.123456,106.123456,24.55
```

#### Remote Monitoring
Data sent to API can be monitored via web dashboard (if available).

## 🛠️ Troubleshooting

### Common Issues

#### 1. GPS Not Locking
**Symptoms**: "Menunggu data GPS yang valid..." appears continuously

**Solutions**:
- Move to area with clear sky view
- Wait 5-15 minutes for cold start
- Check GPS antenna connection
- Verify GPS module power (3.3V)

#### 2. GSM Connection Failed
**Symptoms**: "GPRS connection failed" or "Cannot send data"

**Solutions**:
- Check SIM card insertion
- Verify APN settings in config.h
- Ensure adequate signal strength
- Check SIM800L power supply (4V, 2A capability)
- Verify antenna connection

#### 3. SD Card Errors
**Symptoms**: "SD Card initialization failed!"

**Solutions**:
- Format SD card to FAT32
- Check SD card size (32GB max recommended)
- Verify SPI connections
- Test with different SD card
- Check 3.3V power to SD module

#### 4. Sensor Reading Issues
**Symptoms**: Inconsistent or no distance readings

**Solutions**:
- Check sensor power (5V)
- Verify UART connections
- Test sensors individually
- Check for physical obstructions
- Ensure proper mounting alignment

#### 5. Battery Draining Fast
**Symptoms**: System shuts down quickly

**Solutions**:
- Check battery capacity (minimum 2000mAh)
- Verify charging circuit
- Reduce transmission frequency
- Implement sleep mode
- Check for current leaks

### Error Codes & Messages

#### GPS Errors
- `GPS timeout` - No satellites visible
- `GPS invalid data` - Corrupted NMEA sentences
- `GPS no fix` - Insufficient satellites

#### GSM Errors
- `Modem not responding` - AT command timeout
- `Network registration failed` - No cellular coverage
- `GPRS context failed` - APN configuration issue

#### Sensor Errors
- `Sensor checksum error` - Data corruption
- `Sensor timeout` - Communication failure
- `Sensor out of range` - Reading beyond limits

#### Storage Errors
- `SD init failed` - Hardware or formatting issue
- `File write error` - Disk full or corrupted
- `File read error` - Corrupted file system

## 🔧 Maintenance

### Daily Checks
- [ ] System operational (LED indicators)
- [ ] Data being logged (check SD card)
- [ ] API transmission working
- [ ] Battery level adequate

### Weekly Maintenance
- [ ] Download and analyze SD card data
- [ ] Check physical sensor alignment
- [ ] Verify enclosure integrity
- [ ] Clean antenna connections
- [ ] Test GPS lock time

### Monthly Maintenance
- [ ] Full system calibration
- [ ] Battery capacity test
- [ ] Firmware update check
- [ ] Data quality analysis
- [ ] Backup configuration settings

### Annual Maintenance
- [ ] Replace SD card
- [ ] Battery replacement (if needed)
- [ ] Hardware inspection
- [ ] Calibration verification
- [ ] Performance assessment

## 📈 Performance Optimization

### Data Quality Improvement
1. **Sensor Positioning**: Ensure stable mounting
2. **Environmental Factors**: Shield from interference
3. **Calibration**: Regular sensor calibration
4. **Filtering**: Implement data smoothing algorithms

### Battery Life Extension
1. **Sleep Modes**: Implement deep sleep between readings
2. **Transmission Optimization**: Reduce frequency when appropriate
3. **Power Management**: Turn off unused modules
4. **Battery Maintenance**: Regular charging cycles

### Network Optimization
1. **Signal Quality**: Position for best cellular reception
2. **Data Compression**: Minimize payload size
3. **Retry Logic**: Implement smart retry mechanisms
4. **Connection Pooling**: Reuse connections when possible

## 📊 Data Analysis

### Local Data Processing
```python
# Example Python script to analyze SD card data
import pandas as pd
import matplotlib.pyplot as plt

# Read CSV data
df = pd.read_csv('file_15_07_2025.csv')

# Plot distance measurements
plt.figure(figsize=(12, 6))
plt.plot(df['timestamp'], df['distance1'], label='Sensor 1')
plt.plot(df['timestamp'], df['distance2'], label='Sensor 2')
plt.xlabel('Time')
plt.ylabel('Distance (cm)')
plt.title('Ultrasonic Sensor Readings')
plt.legend()
plt.show()

# Calculate statistics
print(f"Average depth: {df['depth'].mean():.2f} cm")
print(f"Min depth: {df['depth'].min():.2f} cm")
print(f"Max depth: {df['depth'].max():.2f} cm")
```

### Data Quality Metrics
- **GPS Accuracy**: HDOP values < 2.0
- **Sensor Consistency**: Standard deviation < 5% of reading
- **Data Completeness**: > 95% successful readings
- **Transmission Success**: > 90% API uploads successful

## 🔐 Security Considerations

### Device Security
- Change default device IDs
- Secure physical access to device
- Regular firmware updates
- Monitor for unauthorized access

### Data Security
- Encrypt sensitive data on SD card
- Use HTTPS for API communication
- Implement authentication tokens
- Regular security audits

### Network Security
- Monitor data usage patterns
- Detect anomalous behavior
- Secure APN configurations
- Regular credential rotation

## 📞 Support & Resources

### Technical Support
- **Email**: support@ggfsystem.com
- **Phone**: [Support Phone Number]
- **Documentation**: [Project Documentation URL]
- **Issue Tracking**: [GitHub Issues URL]

### Training Resources
- **Video Tutorials**: [YouTube Channel]
- **User Manual**: [PDF Download]
- **Best Practices Guide**: [Documentation Link]
- **FAQ**: [FAQ Page URL]

### Community
- **User Forum**: [Forum URL]
- **Developer Community**: [Discord/Slack]
- **Knowledge Base**: [KB URL]

## 📋 Deployment Checklist

### Pre-Deployment
- [ ] Hardware assembled and tested
- [ ] Firmware uploaded and configured
- [ ] Network connectivity verified
- [ ] GPS lock achieved
- [ ] API communication tested
- [ ] SD card logging confirmed
- [ ] Battery charged and tested
- [ ] Enclosure sealed properly

### Deployment
- [ ] Installation location surveyed
- [ ] Mounting hardware installed
- [ ] System powered on
- [ ] Initial data collection verified
- [ ] Remote monitoring confirmed
- [ ] Documentation completed

### Post-Deployment
- [ ] 24-hour operation test
- [ ] Data quality assessment
- [ ] Performance benchmarking
- [ ] User training completed
- [ ] Maintenance schedule established
- [ ] Support contacts provided

---

**Deployment Guide Version**: 1.0
**Last Updated**: July 16, 2025
**Compatible Firmware**: v1.0.0+
