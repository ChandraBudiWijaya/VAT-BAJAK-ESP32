#include <Arduino.h>
#include "../../include/config.h"
#include "indicators.h"

void setup_leds() {
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    
    // Turn off LEDs initially
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    
    Serial.print("LED1 pin: ");
    Serial.println(LED1_PIN);
    Serial.print("LED2 pin: ");
    Serial.println(LED2_PIN);
}

void update_leds(float d1, float d2) {
    // LED1 untuk Distance1 (Hidrolik atas)
    if (d1 >= 22 && d1 <= 24) { // Rentang ideal
        digitalWrite(LED1_PIN, HIGH);
        Serial.print("  🟢 LED1 ON - Hidrolik atas OK (");
        Serial.print(d1);
        Serial.println(" cm, target: ~23 cm)");
    } else {
        digitalWrite(LED1_PIN, LOW);
        Serial.print("  🔴 LED1 OFF - Hidrolik atas (");
        Serial.print(d1);
        Serial.println(" cm, target: 23-24 cm)");
    }

    // LED2 untuk Distance2 (Hidrolik bawah)
    if (d2 >= 31.0 && d2 < 38.0) { // Rentang ideal
        digitalWrite(LED2_PIN, HIGH);
        Serial.print("  🟢 LED2 ON - Hidrolik bawah OK (");
        Serial.print(d2);
        Serial.println(" cm, target: 31-38 cm)");
    } else {
        digitalWrite(LED2_PIN, LOW);
        Serial.print("  🔴 LED2 OFF - Hidrolik bawah (");
        Serial.print(d2);
        Serial.println(" cm, target: 31-38 cm)");
    }
}