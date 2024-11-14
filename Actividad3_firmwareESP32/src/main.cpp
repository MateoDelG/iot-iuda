#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>


TaskHandle_t Task1;
TaskHandle_t Task2;
void Task1code(void *pvParameters);
void Task2code(void *pvParameters);


void WiFiSetup();
void doWiFiManager();

WiFiManager wifiManager;


void setup() {
  Serial.begin(115200);
  WiFiSetup();

  // Configurar el pin de activación del portal
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  // Crear la tarea que se ejecutará en la función Task1code() en el núcleo 0
  xTaskCreatePinnedToCore(
      Task1code,   /* Task function */
      "Task1",     /* Name of task */
      10000,       /* Stack size of task */
      NULL,        /* Parameter of the task */
      1,           /* Priority of the task */
      &Task1,      /* Task handle to keep track of created task */
      0);          /* Pin task to core 0 */
  delay(500);

  // Crear la tarea que se ejecutará en la función Task2code() en el núcleo 1
  xTaskCreatePinnedToCore(
      Task2code,   /* Task function */
      "Task2",     /* Name of task */
      10000,       /* Stack size of task */
      NULL,        /* Parameter of the task */
      1,           /* Priority of the task */
      &Task2,      /* Task handle to keep track of created task */
      1);          /* Pin task to core 1 */
  delay(500);
}

void Task1code(void *pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    doWiFiManager();  // Verifica si el portal debe ejecutarse
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Llamada periódica para evitar bloqueo
  }
}

void Task2code(void *pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    // Puedes agregar aquí otras tareas en paralelo
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void loop() {
  // Deja el loop vacío, ya que estamos usando tareas
}

void WiFiSetup() {
  // Intentar conectarse a una red guardada
  if (!wifiManager.autoConnect("ESP32_AP")) {
    Serial.println("No se pudo conectar a WiFi, iniciando en modo sin conexión");
  }

  // Conexión establecida, iniciar mDNS
  if (MDNS.begin("esp32config")) {  // Cambia "esp32config" con el nombre que prefieras
    Serial.println("mDNS iniciado en esp32config.local");
  } else {
    Serial.println("Error al iniciar mDNS");
  }

  // Publica un servicio HTTP en mDNS (opcional)
  MDNS.addService("http", "tcp", 80);
}

// Función para manejar el portal de configuración de WiFiManager
void doWiFiManager() {
  static unsigned int timeout = 120;  // Tiempo en segundos para cerrar el portal automáticamente
  static unsigned long startTime;
  static bool portalRunning = false;

  // Si el portal está en ejecución, llamar a process() y verificar el tiempo de timeout
  if (portalRunning) {
    wifiManager.process();  // Procesa solicitudes en el servidor web

    // Verificar el tiempo de ejecución del portal
    if ((millis() - startTime) > (timeout * 1000)) {
      Serial.println("Timeout del portal de configuración");
      portalRunning = false;
      wifiManager.stopWebPortal();  // Detiene el servidor web
    }
  }

  // Verificar si se debe iniciar el portal de configuración al presionar el botón
  if (!portalRunning) {
    Serial.println("Iniciando solo el servidor web, sin AP");
    wifiManager.setConfigPortalBlocking(false);  // Asegurar que no sea bloqueante
    wifiManager.startWebPortal();                // Inicia solo el servidor web
    portalRunning = true;
    startTime = millis();  // Registrar el tiempo de inicio
  }
}
