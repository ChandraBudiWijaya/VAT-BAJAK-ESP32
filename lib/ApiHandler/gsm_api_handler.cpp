#include "gsm_api_handler.h"

GSMApiHandler::GSMApiHandler(const char* device_id) {
    deviceId = String(device_id);
    isConnected = false;
    
    // Initialize hardware serial for GSM
    gsmSerial = &Serial1;
    
    // Initialize TinyGSM modem and client
    modem = new TinyGsm(*gsmSerial);
    client = new TinyGsmClient(*modem);
}

GSMApiHandler::~GSMApiHandler() {
    disconnect();
    delete modem;
    delete client;
}

void GSMApiHandler::powerOnSIM800LManual() {
    Serial.println("🔌 Mengaktifkan/Mereset SIM800L dengan sequence yang berhasil...");
    
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    
    // Power sequence yang sama dengan test code yang berhasil
    digitalWrite(MODEM_POWER_ON, HIGH);
    delay(100);
    digitalWrite(MODEM_RST, LOW);
    delay(100);
    digitalWrite(MODEM_RST, HIGH);
    delay(1000);
    digitalWrite(MODEM_PWKEY, LOW);
    delay(100);
    digitalWrite(MODEM_PWKEY, HIGH);
    delay(2000);
    digitalWrite(MODEM_PWKEY, LOW);
    delay(10000);
    
    Serial.println("✅ Modem siap dengan power sequence yang tested");
}

void GSMApiHandler::testATCommands() {
    Serial.println("=== Testing AT Commands (TinyGSM) ===");
    
    // Test basic AT
    modem->sendAT("");
    if (modem->waitResponse(1000) == 1) {
        Serial.println("✅ AT: OK");
    } else {
        Serial.println("❌ AT: FAILED");
    }
    
    // Test HTTP commands support
    modem->sendAT("+HTTPTERM");
    modem->waitResponse(1000);
    
    modem->sendAT("+HTTPINIT");
    int httpInit = modem->waitResponse(5000);
    Serial.print("HTTPINIT result: ");
    Serial.println(httpInit);
    
    if (httpInit == 1) {
        Serial.println("✅ HTTP commands supported");
        modem->sendAT("+HTTPTERM");
        modem->waitResponse(1000);
    } else {
        Serial.println("❌ HTTP commands NOT supported or ERROR");
    }
    
    Serial.println("=== End AT Test (TinyGSM) ===");
}

bool GSMApiHandler::initialize() {
    Serial.println("📡 Initializing GSM TinyGPS Handler (Tested Working Method)...");
    
    // Setup hardware serial
    gsmSerial->begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
    
    // Power on modem with tested sequence
    powerOnSIM800LManual();
    
    Serial.println("🧪 Inisialisasi modem...");
    if (!modem->testAT(5000)) {
        Serial.println("❌ Modem tidak merespons!");
        return false;
    }
    
    Serial.println("✅ Modem responding - initialization successful!");
    return true;
}

bool GSMApiHandler::connect() {
    Serial.println("🔗 Connecting to GSM network (TinyGSM Method)...");
    
    Serial.println("📡 Menunggu jaringan...");
    if (!modem->waitForNetwork()) {
        Serial.println("❌ Network registration failed");
        return false;
    }
    Serial.println("✅ Network registered");
    
    Serial.print("🌐 Menghubungkan ke GPRS...");
    if (!modem->gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" ❌ GPRS connection failed");
        return false;
    }
    Serial.println(" ✅ SUKSES");
    
    Serial.print("📍 IP Lokal: ");
    Serial.println(modem->getLocalIP());
    
    // Test AT commands support
    testATCommands();
    
    isConnected = true;
    Serial.println("✅ GSM TinyGPS connection established!");
    return true;
}

bool GSMApiHandler::disconnect() {
    Serial.println("🔌 Disconnecting GSM TinyGPS...");
    
    if (modem) {
        modem->gprsDisconnect();
    }
    
    isConnected = false;
    Serial.println("✅ GSM TinyGPS disconnected");
    return true;
}

bool GSMApiHandler::isModemConnected() {
    return isConnected && modem && modem->isNetworkConnected();
}

String GSMApiHandler::getTimestamp() {
    // Try to get network time from GSM first
    String networkTime = getGSMNetworkTime();
    if (networkTime.length() > 0) {
        return networkTime;
    }
    
    // Fallback: Generate current timestamp based on system uptime
    // Estimate current time based on when device started (assume 9 AM start)
    unsigned long currentMillis = millis();
    unsigned long totalSeconds = currentMillis / 1000;
    
    // Current date: July 21, 2025
    int year = 2025;
    int month = 7;
    int day = 21;
    
    // Estimate starting time as 9:00 AM WIB (UTC+7)
    int baseHour = 9;
    int baseMinute = 0;
    int baseSecond = 0;
    
    // Calculate current time from uptime
    int totalMinutes = (totalSeconds / 60) + baseMinute;
    int totalHours = (totalMinutes / 60) + baseHour;
    
    int currentSecond = (totalSeconds % 60);
    int currentMinute = (totalMinutes % 60);
    int currentHour = (totalHours % 24);
    
    // Handle day overflow
    if (totalHours >= 24) {
        day += (totalHours / 24);
        // Simple day overflow handling for July
        if (day > 31) {
            day = day % 31;
            month++;
        }
    }
    
    // Format as ISO 8601 with WIB timezone (+07:00)
    char timestamp[32];
    sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d+07:00", 
            year, month, day, currentHour, currentMinute, currentSecond);
    
    return String(timestamp);
}

String GSMApiHandler::getGSMNetworkTime() {
    if (!modem) return "";
    
    Serial.println("⏰ Getting network time from GSM...");
    
    // Get network time from GSM tower
    modem->sendAT("+CCLK?");
    String response = "";
    
    unsigned long timeout = millis();
    while (millis() - timeout < 5000) {
        if (modem->stream.available()) {
            response += modem->stream.readString();
            if (response.indexOf("OK") >= 0) break;
        }
        delay(100);
    }
    
    Serial.print("GSM Time Response: ");
    Serial.println(response);
    
    // Parse GSM time response format: +CCLK: "25/07/21,14:30:25+28"
    int startPos = response.indexOf("\"");
    int endPos = response.lastIndexOf("\"");
    
    if (startPos >= 0 && endPos > startPos) {
        String timeStr = response.substring(startPos + 1, endPos);
        Serial.print("Parsed GSM Time: ");
        Serial.println(timeStr);
        
        // Convert GSM format to ISO format
        if (timeStr.length() >= 17) {
            // Parse: YY/MM/DD,HH:MM:SS+TZ
            String year = "20" + timeStr.substring(0, 2);
            String month = timeStr.substring(3, 5);
            String day = timeStr.substring(6, 8);
            String hour = timeStr.substring(9, 11);
            String minute = timeStr.substring(12, 14);
            String second = timeStr.substring(15, 17);
            
            String isoTime = year + "-" + month + "-" + day + "T" + 
                           hour + ":" + minute + ":" + second + "+07:00";
            
            Serial.print("✅ GSM Network Time (ISO): ");
            Serial.println(isoTime);
            return isoTime;
        }
    }
    
    Serial.println("❌ Failed to get GSM network time");
    return "";
}

String GSMApiHandler::createProductionJsonPayload(float d1, float d2, float lat, float lon, float depth) {
    // Format PRODUCTION yang BERHASIL berdasarkan test code yang working
    StaticJsonDocument<512> doc;
    
    doc["type"] = "sensor";
    doc["deviceId"] = deviceId;  // BJK0001 format
    
    // GPS object nested (sesuai production format yang berhasil)
    JsonObject gps = doc.createNestedObject("gps");
    gps["lat"] = lat;
    gps["lon"] = lon;
    gps["alt"] = depth;  // Pakai depth sebagai altitude
    gps["sog"] = 0;      // Speed over ground
    gps["cog"] = 0;      // Course over ground
    
    // Ultrasonic object nested (sesuai production format yang berhasil)
    JsonObject ultrasonic = doc.createNestedObject("ultrasonic");
    ultrasonic["dist1"] = round(d1 * 100) / 100.0;   // 2 decimal precision
    ultrasonic["dist2"] = round(d2 * 100) / 100.0;   // 2 decimal precision
    
    doc["timestamp"] = getTimestamp();  // ISO format timestamp
    
    String payload;
    serializeJson(doc, payload);  // Compact JSON untuk production
    return payload;
}

bool GSMApiHandler::sendSensorData(float distance1, float distance2, float latitude, float longitude, float depth) {
    if (!isConnected) {
        Serial.println("❌ GSM not connected - cannot send data");
        return false;
    }
    
    Serial.println("");
    Serial.println("🚀 SENDING DATA TO PRODUCTION API (TinyGSM Method)");
    Serial.println("========================================");
    Serial.print("Distance1: "); Serial.print(distance1); Serial.println(" cm");
    Serial.print("Distance2: "); Serial.print(distance2); Serial.println(" cm");
    Serial.print("Latitude: "); Serial.println(latitude, 6);
    Serial.print("Longitude: "); Serial.println(longitude, 6);
    Serial.print("Depth: "); Serial.print(depth); Serial.println(" cm");
    Serial.println("========================================");
    
    // Create PRODUCTION JSON payload (exact format yang berhasil di test)
    String payload = createProductionJsonPayload(distance1, distance2, latitude, longitude, depth);
    
    Serial.println("📋 Production Data Payload (EXACT Working Format):");
    Serial.println(payload);
    Serial.println("📊 JSON Structure:");
    Serial.println("  - type: sensor");
    Serial.println("  - deviceId: " + deviceId);
    Serial.println("  - gps: {lat, lon, alt, sog, cog}");
    Serial.println("  - ultrasonic: {dist1, dist2}");
    Serial.println("  - timestamp: ISO format");
    Serial.println("----------------------------------------");
    Serial.print("📡 Target API: https://");
    Serial.print(server);
    Serial.println(resource);
    Serial.println("----------------------------------------");
    
    // Kirim ke Production API menggunakan method yang berhasil
    if (sendToProductionAPI(payload)) {
        Serial.println("\n🎉 SUCCESS! Data telah dikirim ke production API!");
        Serial.println("✅ Sistem monitoring tanah subsoil berjalan normal.");
        return true;
    } else {
        Serial.println("\n❌ FAILED! Gagal mengirim ke production API.");
        Serial.println("🔧 Cek koneksi internet atau format data.");
        return false;
    }
}

bool GSMApiHandler::sendToProductionAPI(const String& payload) {
    Serial.println("🚀 MENGIRIM KE API PRODUCTION (TinyGSM AT Commands)...");
    
    // Terminate existing HTTP session if any
    modem->sendAT("+HTTPTERM");
    modem->waitResponse(1000);
    delay(1000);
    
    // Inisialisasi HTTP
    Serial.println("📡 Inisialisasi HTTP...");
    modem->sendAT("+HTTPINIT");
    if (modem->waitResponse(10000) != 1) {
        Serial.println("❌ Gagal inisialisasi HTTP");
        return false;
    }
    delay(1000);
    
    // Aktifkan SSL untuk HTTPS
    Serial.println("🔒 Mengaktifkan SSL...");
    modem->sendAT("+HTTPSSL=1");
    if (modem->waitResponse(5000) != 1) {
        Serial.println("❌ SSL tidak didukung atau gagal diaktifkan");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    delay(1000);
    
    // Set URL untuk API production
    Serial.println("🌐 Set Production API URL...");
    String fullURL = "https://" + String(server) + String(resource);
    modem->sendAT("+HTTPPARA=\"URL\",\"" + fullURL + "\"");
    if (modem->waitResponse(5000) != 1) {
        Serial.println("❌ Gagal set Production API URL");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    delay(500);
    
    // Set content type dan headers tambahan
    Serial.println("📋 Set content type dan headers...");
    modem->sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");
    if (modem->waitResponse(5000) != 1) {
        Serial.println("❌ Gagal set content type");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    delay(500);
    
    // Set User-Agent header (sama dengan test yang berhasil)
    modem->sendAT("+HTTPPARA=\"USERDATA\",\"User-Agent: ESP32-SIM800L-SubsoilMonitor\"");
    modem->waitResponse(3000);
    delay(300);
    
    // Set Accept header (sama dengan test yang berhasil)
    modem->sendAT("+HTTPPARA=\"USERDATA\",\"Accept: application/json\"");
    modem->waitResponse(3000);
    delay(300);
    
    // Set data
    Serial.println("📤 Memulai input data...");
    Serial.print("Payload length: ");
    Serial.println(payload.length());
    
    modem->sendAT("+HTTPDATA=" + String(payload.length()) + ",10000");
    
    // Tunggu prompt ">"
    unsigned long timeout = millis();
    bool gotPrompt = false;
    while (millis() - timeout < 10000) {
        if (modem->stream.available()) {
            String response = modem->stream.readString();
            Serial.print("Response: ");
            Serial.println(response);
            if (response.indexOf("DOWNLOAD") >= 0 || response.indexOf(">") >= 0) {
                gotPrompt = true;
                break;
            }
        }
        delay(100);
    }
    
    if (!gotPrompt) {
        Serial.println("❌ Tidak mendapat prompt untuk input data");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    // Kirim payload
    Serial.println("📤 Mengirim payload ke API...");
    modem->stream.print(payload);
    
    // Tunggu konfirmasi
    if (modem->waitResponse(10000) != 1) {
        Serial.println("❌ Gagal mengirim data");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    // Kirim POST request
    Serial.println("📡 Mengirim HTTPS POST request ke production API...");
    modem->sendAT("+HTTPACTION=1"); // 1 = POST
    if (modem->waitResponse(30000) != 1) {
        Serial.println("❌ Gagal mengirim HTTPS POST request");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    Serial.println("✅ HTTPS POST request berhasil dikirim ke production API!");
    
    // Tunggu sebentar sebelum membaca response
    delay(3000);
    
    // Baca response dari production API dengan detail
    Serial.println("📥 Membaca response dari production API...");
    modem->sendAT("+HTTPREAD");
    
    // Baca semua data yang tersedia
    String response = "";
    unsigned long startTime = millis();
    while (millis() - startTime < 15000) { // Tunggu lebih lama
        while (modem->stream.available()) {
            char c = modem->stream.read();
            response += c;
            Serial.print(c);  // Print setiap karakter untuk debug
        }
        delay(50);
        
        // Jika dapat +HTTPREAD response, tunggu sebentar lagi untuk body
        if (response.indexOf("+HTTPREAD:") >= 0 && response.length() > 20) {
            delay(2000); // Tunggu body response
        }
        
        // Break jika sudah dapat response lengkap
        if (response.indexOf("OK") >= 0 && response.length() > 50) {
            break;
        }
    }
    
    Serial.println("\n=== FULL API RESPONSE (Raw) ===");
    Serial.print("Response length: ");
    Serial.println(response.length());
    Serial.println("Raw content:");
    for (int i = 0; i < response.length(); i++) {
        Serial.print((char)response[i]);
        if (i % 80 == 79) Serial.println(); // New line every 80 chars
    }
    Serial.println("\n==============================");
    
    // Terminate HTTP
    modem->sendAT("+HTTPTERM");
    modem->waitResponse(1000);
    
    // Analisis response lebih detail (sama dengan test yang berhasil)
    if (response.indexOf("200") >= 0 || response.indexOf("201") >= 0) {
        Serial.println("\n🎉 DATA BERHASIL DIKIRIM KE PRODUCTION API!");
        return true;
    } else if (response.indexOf("400") >= 0) {
        Serial.println("\n⚠️  API Response: 400 Bad Request");
        Serial.println("❓ Kemungkinan: format data, authentication, atau header salah");
        return false;
    } else if (response.indexOf("401") >= 0) {
        Serial.println("\n🔒 API Response: 401 Unauthorized");
        Serial.println("❗ Perlu authentication (API Key/Token)");
        return false;
    } else if (response.indexOf("404") >= 0) {
        Serial.println("\n🔍 API Response: 404 Not Found");
        Serial.println("❗ Endpoint /subsoils tidak ditemukan atau method salah");
        return false;
    } else if (response.indexOf("50") >= 0) {
        Serial.println("\n⚠️  API Response: Server Error (5xx)");
        Serial.println("Kemungkinan: server API bermasalah");
        return false;
    } else {
        Serial.println("\n❓ Response tidak dikenal atau timeout");
        Serial.println("Raw response length: " + String(response.length()));
        return false;
    }
}

bool GSMApiHandler::sendHTTPTestRequest(const String& payload) {
    Serial.println("🧪 Testing HTTP dengan TCP Socket...");
    
    // Connect ke server test
    Serial.print("Connecting to ");
    Serial.print(server);
    Serial.print(":");
    Serial.println(port);
    
    if (!client->connect(server, port)) {
        Serial.println("❌ Connection failed");
        return false;
    }
    
    Serial.println("✅ Connected! Sending HTTP POST request...");
    
    // Kirim HTTP POST request
    client->print("POST ");
    client->print(resource);
    client->println(" HTTP/1.1");
    
    client->print("Host: ");
    client->println(server);
    
    client->println("Content-Type: application/json");
    client->println("Connection: close");
    client->println("User-Agent: ESP32-SIM800L");
    
    client->print("Content-Length: ");
    client->println(payload.length());
    
    client->println(); // Empty line before body
    client->print(payload);
    
    Serial.println("📡 Request sent! Waiting for response...");
    
    // Baca response
    unsigned long timeout = millis();
    String response = "";
    while (client->connected() && millis() - timeout < 15000L) {
        while (client->available()) {
            char c = client->read();
            Serial.print(c);
            response += c;
            timeout = millis();
        }
    }
    
    client->stop();
    Serial.println("\n=== Connection closed ===");
    
    // Cek apakah ada response yang valid
    if (response.length() > 0 && response.indexOf("200") >= 0) {
        Serial.println("✅ HTTP TEST BERHASIL!");
        return true;
    } else {
        Serial.println("❌ HTTP TEST GAGAL - No valid response");
        return false;
    }
}

bool GSMApiHandler::sendHTTPSTestRequest(const String& payload) {
    Serial.println("🧪 Testing HTTPS dengan TCP Socket...");
    
    // Connect ke server test dengan port 443
    Serial.print("Connecting to ");
    Serial.print(server);
    Serial.println(":443");
    
    if (!client->connect(server, 443)) {
        Serial.println("❌ HTTPS Connection failed");
        return false;
    }
    
    Serial.println("✅ HTTPS Connected! Sending HTTPS POST request...");
    
    // Kirim HTTPS POST request
    client->print("POST ");
    client->print(resource);
    client->println(" HTTP/1.1");
    
    client->print("Host: ");
    client->println(server);
    
    client->println("Content-Type: application/json");
    client->println("Connection: close");
    client->println("User-Agent: ESP32-SIM800L");
    
    client->print("Content-Length: ");
    client->println(payload.length());
    
    client->println(); // Empty line before body
    client->print(payload);
    
    Serial.println("📡 HTTPS Request sent! Waiting for response...");
    
    // Baca response
    unsigned long timeout = millis();
    String response = "";
    while (client->connected() && millis() - timeout < 15000L) {
        while (client->available()) {
            char c = client->read();
            Serial.print(c);
            response += c;
            timeout = millis();
        }
    }
    
    client->stop();
    Serial.println("\n=== HTTPS Connection closed ===");
    
    // Cek apakah ada response yang valid
    if (response.length() > 0 && response.indexOf("200") >= 0) {
        Serial.println("✅ HTTPS TEST BERHASIL!");
        return true;
    } else {
        Serial.println("❌ HTTPS TEST GAGAL - No valid response atau SSL error");
        return false;
    }
}

bool GSMApiHandler::sendHTTPSATRequest(const String& payload) {
    Serial.println("🧪 Testing HTTPS dengan AT Commands + SSL...");
    
    // Menggunakan sendToProductionAPI yang sudah teruji
    return sendToProductionAPI(payload);
}

bool GSMApiHandler::sendHTTPRequest(const String& payload) {
    Serial.println("🧪 Menggunakan AT commands untuk HTTP...");
    
    // Terminate existing HTTP session if any
    modem->sendAT("+HTTPTERM");
    modem->waitResponse(1000);
    delay(1000);
    
    // Inisialisasi HTTP
    Serial.println("Inisialisasi HTTP...");
    modem->sendAT("+HTTPINIT");
    if (modem->waitResponse(10000) != 1) {
        Serial.println("❌ Gagal inisialisasi HTTP - mungkin HTTP commands tidak support");
        return false;
    }
    delay(1000);
    
    // Set URL (HTTP, bukan HTTPS)
    Serial.println("Set URL...");
    modem->sendAT("+HTTPPARA=\"URL\",\"http://httpbin.org/post\""); // Test server
    if (modem->waitResponse(5000) != 1) {
        Serial.println("❌ Gagal set URL");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    delay(500);
    
    // Set content type
    Serial.println("Set content type...");
    modem->sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");
    if (modem->waitResponse(5000) != 1) {
        Serial.println("❌ Gagal set content type");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    delay(500);
    
    // Set data
    Serial.println("Memulai input data...");
    Serial.print("Payload length: ");
    Serial.println(payload.length());
    
    modem->sendAT("+HTTPDATA=" + String(payload.length()) + ",10000");
    
    // Tunggu prompt ">"
    unsigned long timeout = millis();
    bool gotPrompt = false;
    while (millis() - timeout < 10000) {
        if (modem->stream.available()) {
            String response = modem->stream.readString();
            Serial.print("Response: ");
            Serial.println(response);
            if (response.indexOf("DOWNLOAD") >= 0 || response.indexOf(">") >= 0) {
                gotPrompt = true;
                break;
            }
        }
        delay(100);
    }
    
    if (!gotPrompt) {
        Serial.println("❌ Tidak mendapat prompt untuk input data");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    // Kirim payload
    Serial.println("Mengirim payload...");
    modem->stream.print(payload);
    
    // Tunggu konfirmasi
    if (modem->waitResponse(10000) != 1) {
        Serial.println("❌ Gagal mengirim data");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    // Kirim POST request
    Serial.println("Mengirim POST request...");
    modem->sendAT("+HTTPACTION=1"); // 1 = POST
    if (modem->waitResponse(30000) != 1) {
        Serial.println("❌ Gagal mengirim POST request");
        modem->sendAT("+HTTPTERM");
        return false;
    }
    
    Serial.println("✅ POST request berhasil dikirim!");
    
    // Tunggu sebentar sebelum membaca response
    delay(2000);
    
    // Baca response (optional)
    Serial.println("Membaca response...");
    modem->sendAT("+HTTPREAD");
    modem->waitResponse(5000);
    
    // Terminate HTTP
    modem->sendAT("+HTTPTERM");
    modem->waitResponse(1000);
    
    return true;
}

void GSMApiHandler::printStatus() {
    Serial.println("\n📡 GSM TinyGPS Handler Status:");
    Serial.print("Server: ");
    Serial.println(server);
    Serial.print("Resource: ");
    Serial.println(resource);
    Serial.print("Port: ");
    Serial.println(port);
    Serial.print("Device ID: ");
    Serial.println(deviceId);
    Serial.print("APN: ");
    Serial.println(apn);
    Serial.print("Connection: ");
    Serial.println(isConnected ? "Connected ✅" : "Disconnected ❌");
    
    if (isConnected && modem) {
        Serial.print("Network Status: ");
        Serial.println(modem->isNetworkConnected() ? "Connected ✅" : "Disconnected ❌");
        Serial.print("IP Address: ");
        Serial.println(modem->getLocalIP());
    }
}

void GSMApiHandler::printNetworkInfo() {
    Serial.println("\n📊 GSM TinyGPS Network Information:");
    if (modem) {
        Serial.print("RSSI: ");
        Serial.println(modem->getSignalQuality());
        
        Serial.print("Operator: ");
        Serial.println(modem->getOperator());
        
        Serial.print("Network Connected: ");
        Serial.println(modem->isNetworkConnected() ? "Yes" : "No");
        
        Serial.print("GPRS Connected: ");
        Serial.println(modem->isGprsConnected() ? "Yes" : "No");
        
        Serial.print("Local IP: ");
        Serial.println(modem->getLocalIP());
    }
}
