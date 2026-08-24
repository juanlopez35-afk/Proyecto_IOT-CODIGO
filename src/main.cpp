#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  21  
#define RST_PIN 22  
#define LED_PIN 2   
#define BUZZER_PIN 5 

MFRC522 rfid(SS_PIN, RST_PIN);
String tarjetaAutorizada = "75 F2 DD 13";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Sistema RFID listo. Acerca una tarjeta...");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      String tarjetaLeida = "";
      for (byte i = 0; i < rfid.uid.size; i++) {
        tarjetaLeida += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        tarjetaLeida += String(rfid.uid.uidByte[i], HEX);
      }
      tarjetaLeida.toUpperCase();
      tarjetaLeida.trim();

      Serial.print("Tarjeta leida: ");
      Serial.println(tarjetaLeida);

      if (tarjetaLeida == tarjetaAutorizada) {
        Serial.println("-> ACCESO CONCEDIDO");
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
      } else {
        Serial.println("-> ACCESO DENEGADO");
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
      }
      rfid.PICC_HaltA();
    }
  }
}
