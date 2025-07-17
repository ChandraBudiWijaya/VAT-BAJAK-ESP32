#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("");
    Serial.println("========================================");
    Serial.println("ESP32 VAT SUBSOIL MONITOR - SIMPLE TEST");
    Serial.println("========================================");
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    Serial.println("Setup completed successfully!");
    Serial.println("========================================");
    Serial.flush();
}

void loop() {
    static unsigned long timer = 0;
    static int count = 0;
    
    if (millis() - timer >= 2000) {
        timer = millis();
        count++;
        
        Serial.print("Loop #");
        Serial.print(count);
        Serial.print(" - Uptime: ");
        Serial.print(millis()/1000);
        Serial.print("s - Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        
        // Simulasi data sensor sederhana
        if (count % 5 == 0) {
            Serial.println("--- DUMMY SENSOR DATA ---");
            Serial.println("Distance 1: 25.3 cm");
            Serial.println("Distance 2: 28.7 cm");
            Serial.println("GPS: -6.175392, 106.827153");
            Serial.println("Kedalaman: 65.2 cm");
            Serial.println("Status: READY FOR API");
            Serial.println("--- END DATA ---");
        }
        
        Serial.flush();
    }
    
    delay(100);
}
