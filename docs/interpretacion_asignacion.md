Se requiere un sistema de control de acceso local para validar la presencia de usuarios mediante identificación por radiofrecuencia (RFID). Esta solución puede ser utilizada por administradores de edificios o personal de seguridad para restringir el ingreso a áreas privadas de forma rápida y automatizada.

La entrada del sistema es la lectura del código UID enviado por la tarjeta MFRC522 vía SPI. El ESP32 compara este UID contra el valor autorizado (`75 F2 DD 13`). Como salida, si el código coincide, activa el LED y el buzzer por 1 segundo (Acceso Concedido); si no coincide, activa un pitido corto de 200 ms (Acceso Denegado).

La regla individual exige que la respuesta del sistema sea inmediata al aproximar el tag. El estado seguro reduce el riesgo de accesos no autorizados o lecturas erróneas apagando los actuadores y manteniendo el sensor en espera tras cada ciclo.

**Supuestos:**
1. Se asume que el flujo de usuarios es individual (una tarjeta a la vez).
2. Se asume que la fuente de alimentación del ESP32 permanece estable.

**Limitaciones del prototipo:**
1. El UID está guardado de forma fija en el código (hardcoded), sin base de datos dinámica.
2. No cuenta con conectividad a la red ni registro de eventos (logs) remotos.
