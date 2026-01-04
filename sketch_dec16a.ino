#include "DHT.h"
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL63ykqJG4X"
#define BLYNK_TEMPLATE_NAME "Smart Room"
#define BLYNK_AUTH_TOKEN "9_sEYH2exlUbXjbw_XplhfS_e39mvYdZ"  

// WiFi credentials
char ssid[] = "YOHANA LT2";     
char pass[] = "yohana17";  

#define DHTPIN D4
#define FLAME_PIN D5
#define BUZZER_PIN D6
#define SERVO_PIN D7
#define FAN_RELAY_PIN D3
#define DHTTYPE DHT22

// SETTING SERVO ARAH BERLAWANAN
#define SERVO_REVERSE true  // TRUE = arah berlawanan, FALSE = arah normal

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

// Variables
bool fanOn = false;  
unsigned long lastFanToggle = 0;  
bool fireDetected = false;
bool highTempDetected = false;
float currentTemp = 0;
int flameState = HIGH;
bool servoWasActive = false;  // Flag untuk melacak status servo

// Blynk Virtual Pins
#define VIRTUAL_TEMP V0
#define VIRTUAL_FLAME V1
#define VIRTUAL_FAN V3
#define VIRTUAL_STATUS V5
#define VIRTUAL_SERVO V6
#define VIRTUAL_BUZZER V7

// FUNGSI UNTUK MENGATUR SERVO DENGAN ARAH TERBALIK
void setServoAngle(int angle) {
  if (SERVO_REVERSE) {
    // Mapping terbalik: 0° menjadi 180°, 90° menjadi 90°, 180° menjadi 0°
    int reversedAngle;
    
    if (angle == 0) {
      reversedAngle = 180;  // Posisi awal menjadi 180°
    } else if (angle == 90) {
      reversedAngle = 90;   // Tetap 90° (tengah)
    } else if (angle == 180) {
      reversedAngle = 0;    // Posisi maksimum menjadi 0°
    } else {
      reversedAngle = 180 - angle;  // Rumus umum untuk membalik
    }
    
    myServo.write(reversedAngle);
    Serial.print("Servo: " + String(angle) + "°->" + String(reversedAngle) + "° | ");
  } else {
    // Arah normal
    myServo.write(angle);
    Serial.print("Servo: " + String(angle) + "° | ");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== SMART ROOM SYSTEM WITH BLYNK ===");
  Serial.print("Servo mode: ");
  Serial.println(SERVO_REVERSE ? "REVERSE DIRECTION" : "NORMAL DIRECTION");
  
  // Inisialisasi pin
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  
  // State awal
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW); 
  
  // Test fan on startup
  Serial.println("\nTesting fan...");
  digitalWrite(FAN_RELAY_PIN, HIGH);
  delay(3000);
  digitalWrite(FAN_RELAY_PIN, LOW);
  delay(1000);
  
  // Initialize servo
  myServo.attach(SERVO_PIN);
  
  // Posisi awal: 0° (atau 180° jika reverse)
  if (SERVO_REVERSE) {
    myServo.write(180);  // Posisi awal saat reverse = 180°
  } else {
    myServo.write(0);    // Posisi awal normal = 0°
  }
  
  servoWasActive = false;
  delay(500);
  
  // Initialize DHT sensor
  dht.begin();
  delay(2000);
  
  // Koneksi ke Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  Serial.println("\n✅ SYSTEM READY WITH BLYNK!");
  if (SERVO_REVERSE) {
    Serial.println("Servo initial position: 180° (reverse mode)");
  } else {
    Serial.println("Servo initial position: 0° (normal mode)");
  }
}

void loop() {
  Blynk.run();  
  
  static unsigned long lastRead = 0;
  
  if (millis() - lastRead >= 1000) {  
    lastRead = millis();
    
    currentTemp = dht.readTemperature(); 
    int flame = digitalRead(FLAME_PIN);  
    
    // Tampilkan di Serial Monitor
    Serial.print("\n[" + String(millis() / 1000) + "s] Temp: ");
    
    if (isnan(currentTemp)) {
      Serial.print("ERROR");  
    } else {
      Serial.print(currentTemp, 1); 
      Serial.print("°C");

      // Kirim data suhu ke Blynk (V0)
      Blynk.virtualWrite(VIRTUAL_TEMP, currentTemp); 

      // ALUR KONTROL FAN
      if (currentTemp > 28.0 && !fanOn) {  
        digitalWrite(FAN_RELAY_PIN, HIGH);
        fanOn = true;
        Serial.print(" | FAN: ON");
        lastFanToggle = millis(); 
        Blynk.virtualWrite(VIRTUAL_FAN, 1);  
      }
    }
    
    // Tampilkan status api
    Serial.print(" | Flame: ");
    Serial.print(flame == LOW ? "No" : "FIRE");
    
    // ALUR KONTROL ALARM DAN SERVO
    fireDetected = (flame == HIGH);
    highTempDetected = (currentTemp > 28.0);
    
    Serial.print(" | Action: ");
    
    if (fireDetected) {
      tone(BUZZER_PIN, 2000, 200);  // Bunyi buzzer saat api terdeteksi
      
      // Servo bergerak ke 90° saat api terdeteksi
      if (!servoWasActive) {
        setServoAngle(270);  // Akan jadi 90° (tengah) baik normal maupun reverse
        servoWasActive = true;
      }
      
      Serial.println("FIRE");
      Blynk.virtualWrite(VIRTUAL_FLAME, 1);  // Kirim status api ke V1
      Blynk.virtualWrite(VIRTUAL_STATUS, "🔥 FIRE DETECTED!");
    } else if (highTempDetected) {
      tone(BUZZER_PIN, 1000, 500);  // Bunyi buzzer saat suhu tinggi
      
      // Servo kembali ke posisi awal jika sebelumnya aktif karena api
      if (servoWasActive) {
        if (SERVO_REVERSE) {
          setServoAngle(0);  // Akan jadi 180° karena reverse
        } else {
          setServoAngle(0);  // Akan jadi 0° karena normal
        }
        servoWasActive = false;
      }
      
      Serial.println("HIGH TEMP");
      Blynk.virtualWrite(VIRTUAL_FLAME, 0);  // Kirim status api ke V1
      Blynk.virtualWrite(VIRTUAL_STATUS, "🌡️ HIGH TEMPERATURE");
    } else {
      noTone(BUZZER_PIN);  // Matikan buzzer
      
      // Servo kembali ke posisi awal jika sebelumnya aktif karena api
      if (servoWasActive) {
        if (SERVO_REVERSE) {
          setServoAngle(0);  // Akan jadi 180° karena reverse
        } else {
          setServoAngle(0);  // Akan jadi 0° karena normal
        }
        servoWasActive = false;
      }
      
      Serial.println("NORMAL");
      Blynk.virtualWrite(VIRTUAL_FLAME, 0);  // Kirim status api ke V1
      Blynk.virtualWrite(VIRTUAL_STATUS, "✅ SYSTEM NORMAL");
    }
  }
}

// FUNGSI TAMBAHAN UNTUK KONTROL MANUAL DARI BLYNK

// Kontrol manual fan dari Blynk
BLYNK_WRITE(VIRTUAL_FAN) {
  int value = param.asInt();
  
  if (value == 1) {
    digitalWrite(FAN_RELAY_PIN, HIGH);  // Menyalakan fan secara manual
    fanOn = true;
    Serial.println("FAN: MANUAL ON (from Blynk)");
  } else {
    digitalWrite(FAN_RELAY_PIN, LOW);  // Mematikan fan secara manual
    fanOn = false;
    Serial.println("FAN: MANUAL OFF (from Blynk)");
  }
}

// Kontrol manual servo dari Blynk
BLYNK_WRITE(VIRTUAL_SERVO) {
  int angle = param.asInt();
  
  // Jika tidak ada api terdeteksi, boleh kontrol manual
  if (digitalRead(FLAME_PIN) == LOW) {
    setServoAngle(angle);  // Gunakan fungsi setServoAngle yang sudah ada
    servoWasActive = (angle != 0);  // Update status servo
    Serial.print("SERVO: Manual control to ");
    Serial.print(angle);
    Serial.println("° (from Blynk)");
    
    // Kirim status ke Blynk
    if (angle == 0) {
      Blynk.virtualWrite(VIRTUAL_STATUS, "✅ Servo: Normal position");
    } else {
      Blynk.virtualWrite(VIRTUAL_STATUS, "🔄 Servo: Manual control");
    }
  } else {
    Serial.println("SERVO: Cannot control manually - FIRE DETECTED!");
    Blynk.virtualWrite(VIRTUAL_STATUS, "⚠️ Cannot control servo - FIRE!");
  }
}

// Kontrol manual buzzer dari Blynk
BLYNK_WRITE(VIRTUAL_BUZZER) {
  int value = param.asInt();
  
  if (value == 1) {
    if (digitalRead(FLAME_PIN) == LOW) {
      tone(BUZZER_PIN, 1000, 1000);
      Serial.println("BUZZER: Manual test (from Blynk)");
      Blynk.virtualWrite(VIRTUAL_STATUS, "🔊 Buzzer test");
    } else {
      Serial.println("BUZZER: Cannot test - FIRE DETECTED!");
    }
  }
}