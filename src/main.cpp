#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// --- Asignación de Pines ---
#define SS_PIN       21  
#define RST_PIN      22  
#define LED_PIN      2   
#define BUZZER_PIN   5   

// --- Parámetros Individuales Asignados (Sección 1.3) ---
const String CODIGO_PROYECTO = "IOT-C05F59AF8A";
const String TARJETA_AUTORIZADA = "75 F2 DD 13";
const unsigned long TIEMPO_BLOQUEO_MS = 17000;    // 17 segundos de bloqueo
const unsigned long INTERVALO_MUESTREO_MS = 2600; // 2600 ms entre lecturas
const int MAX_INTENTOS_FALLIDOS = 3;

// --- Objetos y Variables Globales ---
MFRC522 rfid(SS_PIN, RST_PIN);
int intentosFallidos = 0;

void setup() {
  // Inicialización serial a 115200 baudios
  Serial.begin(115200);

  // Configuración de pines de entrada y salida
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Estado seguro inicial (actuadores apagados)
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Inicializar comunicación SPI y módulo RFID
  SPI.begin();
  rfid.PCD_Init();

  // Mensaje de arranque obligatorio con el código asignado
  Serial.println("==========================================");
  Serial.print("Sistema RFID listo - Proyecto: ");
  Serial.println(CODIGO_PROYECTO);
  Serial.print("UID Autorizado: ");
  Serial.println(TARJETA_AUTORIZADA);
  Serial.println("==========================================");
}

void loop() {
  // Verificar presencia de tarjeta y leer su serie
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String tarjetaLeida = "";

    // Formatear el UID a texto hexadecimal en mayúsculas
    for (byte i = 0; i < rfid.uid.size; i++) {
      tarjetaLeida += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      tarjetaLeida += String(rfid.uid.uidByte[i], HEX);
    }
    tarjetaLeida.toUpperCase();
    tarjetaLeida.trim();

    // Salida serial clara de la medición
    Serial.print("Tarjeta detectada (UID): ");
    Serial.println(tarjetaLeida);

    // Evaluación de la regla de acceso
    if (tarjetaLeida == TARJETA_AUTORIZADA) {
      Serial.println("-> ACCESO CONCEDIDO");
      intentosFallidos = 0; // Reiniciar contador de intentos fallidos

      // Activar LED y Buzzer durante 1 segundo
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    } else {
      intentosFallidos++;
      Serial.print("-> ACCESO DENEGADO (Intento ");
      Serial.print(intentosFallidos);
      Serial.print("/");
      Serial.print(MAX_INTENTOS_FALLIDOS);
      Serial.println(")");

      // Pitido corto de rechazo
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);

      // Evaluación del bloqueo por 3 rechazos consecutivos
      if (intentosFallidos >= MAX_INTENTOS_FALLIDOS) {
        Serial.println("-> ALERTA: Tres rechazos consecutivos detectados.");
        Serial.print("-> SISTEMA BLOQUEADO TEMPORALMENTE POR ");
        Serial.print(TIEMPO_BLOQUEO_MS / 1000);
        Serial.println(" SEGUNDOS.");

        delay(TIEMPO_BLOQUEO_MS); // Bloqueo activo de 17 segundos

        intentosFallidos = 0; // Reiniciar contador tras expirar el bloqueo
        Serial.println("-> SISTEMA DESBLOQUEADO Y LISTO.");
      }
    }

    // Detener comunicación con la tarjeta actual
    rfid.PICC_HaltA();

    // Intervalo de muestreo asignado antes de la siguiente lectura
    delay(INTERVALO_MUESTREO_MS);
  }
}
