#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  21  
#define RST_PIN 22  
#define LED_PIN 2   
#define BUZZER_PIN 5 

MFRC522 rfid(SS_PIN, RST_PIN);
String tarjetaAutorizada = "75 F2 DD 13";
int intentosFallidos = 0;

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
        intentosFallidos = 0; // Resetea intentos
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
      } else {
        intentosFallidos++;
        Serial.print("-> ACCESO DENEGADO (Intento ");
        Serial.print(intentosFallidos);
        Serial.println("/3)");

        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);

        if (intentosFallidos >= 3) {
          Serial.println("-> SISTEMA BLOQUEADO POR 17 SEGUNDOS");
          delay(17000); // Tiempo de bloqueo asignado en la ficha
          intentosFallidos = 0;
          Serial.println("-> SISTEMA DESBLOQUEADO");
        }
      }
      rfid.PICC_HaltA();
    }
  }
}
