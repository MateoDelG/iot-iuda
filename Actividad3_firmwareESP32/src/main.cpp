#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>

// Declaración de las tareas
TaskHandle_t task1;
TaskHandle_t task2;

// Declaración de las funciones de las tareas
void Task1code(void *pvParameters);
void Task2code(void *pvParameters);

// Funciones de configuración WiFi
void WiFiSetup();
void doWiFiManager();

WiFiManager wifiManager;  // Objeto de WiFiManager para manejar la configuración de WiFi

void setup() {
  Serial.begin(115200);  // Inicia la comunicación serial para enviar mensajes a la consola
  WiFiSetup();           // Llama a la función que configura la conexión WiFi

  // Crear y asignar la tarea Task1code() al núcleo 0
  xTaskCreatePinnedToCore(
      Task1code,   /* Función de la tarea */
      "Task1",     /* Nombre de la tarea */
      10000,       /* Tamaño de la pila de la tarea */
      NULL,        /* Parámetro de la tarea */
      1,           /* Prioridad de la tarea */
      &task1,      /* Identificador de la tarea */
      0);          /* Asignar la tarea al núcleo 0 */
  delay(500);     // Pequeña pausa para evitar conflictos

  // Crear y asignar la tarea Task2code() al núcleo 1
  xTaskCreatePinnedToCore(
      Task2code,   /* Función de la tarea */
      "Task2",     /* Nombre de la tarea */
      10000,       /* Tamaño de la pila de la tarea */
      NULL,        /* Parámetro de la tarea */
      1,           /* Prioridad de la tarea */
      &task2,      /* Identificador de la tarea */
      1);          /* Asignar la tarea al núcleo 1 */
  delay(500);     // Pausa para asegurar que ambas tareas inicien correctamente
}

// Función para la primera tarea que se ejecuta en el núcleo 0
void Task1code(void *pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());  // Imprime el núcleo en el que se ejecuta la tarea

  for (;;) {  // Bucle infinito
    doWiFiManager();  // Llama a la función para manejar el portal de configuración de WiFi
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Pausa para evitar sobrecarga
  }
}

// Función para la segunda tarea que se ejecuta en el núcleo 1
void Task2code(void *pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());  // Imprime el núcleo en el que se ejecuta la tarea

  for (;;) {  // Bucle infinito
    // Se pueden agregar más tareas aquí para ejecutarse en paralelo
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Pausa para evitar sobrecarga
  }
}

void loop() {
  // El bucle loop() está vacío, ya que estamos usando tareas que se ejecutan en paralelo
}

// Configura la conexión WiFi y mDNS
void WiFiSetup() {
  // Intentar conectarse a una red WiFi guardada
  if (!wifiManager.autoConnect("ESP32_AP")) {
    Serial.println("No se pudo conectar a WiFi, iniciando en modo sin conexión");
  }

  // Si la conexión es exitosa, iniciar mDNS (permite acceder al dispositivo mediante un nombre local)
  if (MDNS.begin("esp32config")) {  // Cambia "esp32config" por el nombre que desees
    Serial.println("mDNS iniciado en esp32config.local");
  } else {
    Serial.println("Error al iniciar mDNS");
  }

  // Publica un servicio HTTP en mDNS, lo cual es opcional
  MDNS.addService("http", "tcp", 80);
}

// Función para manejar el portal de configuración de WiFiManager
void doWiFiManager() {
  static unsigned int timeout = 120;  // Tiempo límite (segundos) para cerrar el portal automáticamente
  static unsigned long start_time;
  static bool portal_running = false;  // Indicador de si el portal está activo

  // Si el portal está en ejecución, procesa las solicitudes y verifica el tiempo de timeout
  if (portal_running) {
    wifiManager.process();  // Procesa las solicitudes del servidor web

    // Verifica si ha pasado el tiempo límite de actividad
    if ((millis() - start_time) > (timeout * 1000)) {
      Serial.println("Timeout del portal de configuración");
      portal_running = false;
      wifiManager.stopWebPortal();  // Detiene el servidor web de configuración
    }
  }

  // Inicia el portal de configuración si no está activo y se presiona el botón
  if (!portal_running) {
    Serial.println("Iniciando solo el servidor web, sin AP");
    wifiManager.setConfigPortalBlocking(false);  // Asegura que el portal no sea bloqueante
    wifiManager.startWebPortal();                // Inicia solo el servidor web
    portal_running = true;
    start_time = millis();  // Guarda el tiempo de inicio del portal
  }
}
