#include <Arduino.h>
#include <TinyGsmClient.h>
#include <config.h>
#include "api_handler.h"

// Inisialisasi modem
HardwareSerial modemSerial(1); // Gunakan Serial1 untuk modem
TinyGsm modem(modemSerial);
TinyGsmClient client(modem);

void setup_modem() {
    Serial.println("Initializing modem...");
    // Atur pin untuk modem
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    digitalWrite(MODEM_PWKEY, LOW);
    digitalWrite(MODEM_RST, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);
    
    modemSerial.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);
    
    // Restart modem
    Serial.println("Restarting modem...");
    modem.restart();
    Serial.println("Modem Name: " + modem.getModemName());
}

bool connect_gprs() {
    if (modem.isNetworkConnected()) {
        Serial.println("Network connected");
        if (!modem.isGprsConnected()) {
            Serial.print("Connecting to GPRS: ");
            Serial.println(APN);
            if (modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
                Serial.println("GPRS Connected");
                return true;
            } else {
                Serial.println("GPRS connection failed");
                return false;
            }
        }
        return true; // Already connected
    }
    Serial.println("Network not connected");
    return false;
}

void sendDataToAPI(float dist1, float dist2, float lat, float lon, float alt, const char* timestamp) {
    if (!connect_gprs()) {
        Serial.println("Cannot send data, no GPRS connection.");
        return;
    }

    Serial.println("Preparing to send data via HTTP POST...");
    
    // Membuat payload JSON (SAMA PERSIS DENGAN FORMAT SEBELUMNYA)
    String payload = "{\"type\":\"sensor\","
                     "\"deviceId\":\"" + String(DEVICE_ID) + "\","
                     "\"gps\":{"
                       "\"lat\":" + String(lat, 7) + ","
                       "\"lon\":" + String(lon, 7) + ","
                       "\"alt\":" + String(alt, 2) + ","
                       "\"sog\":0,"
                       "\"cog\":0"
                     "},"
                     "\"ultrasonic\":{"
                       "\"dist1\":" + String(dist1, 1) + ","
                       "\"dist2\":" + String(dist2, 1) +
                     "},"
                     "\"timestamp\":\"" + String(timestamp) + "\""
                   "}";

    Serial.println("Payload: " + payload);

    // Parse URL untuk mendapatkan host dan path
    String url = String(API_URL);
    int hostStart = url.indexOf("://") + 3;
    int pathStart = url.indexOf("/", hostStart);
    String host = url.substring(hostStart, pathStart);
    String path = url.substring(pathStart);
    
    Serial.println("Connecting to: " + host);
    
    // Koneksi ke server
    if (client.connect(host.c_str(), 443)) { // HTTPS port
        Serial.println("Connected to server");
        
        // Buat HTTP POST request
        client.println("POST " + path + " HTTP/1.1");
        client.println("Host: " + host);
        client.println("Content-Type: application/json");
        client.println("Content-Length: " + String(payload.length()));
        client.println("Connection: close");
        client.println();
        client.print(payload);
        
        // Baca response
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println(">>> Client Timeout !");
                client.stop();
                return;
            }
        }
        
        // Baca dan tampilkan response
        while (client.available()) {
            String line = client.readStringUntil('\r');
            Serial.print(line);
        }
        
        client.stop();
        Serial.println("\nHTTP POST completed");
    } else {
        Serial.println("Connection to server failed");
    }
}