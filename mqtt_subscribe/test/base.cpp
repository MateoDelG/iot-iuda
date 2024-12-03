#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

const char *ssid = "Delga";
const char *password = "Delga1213";

const char *mqtt_server = "broker.mqtt.cool";
#define mqtt_port 1883

int id = 0;
int percentage_control = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi();
void setupMqtt();
void MQTTStateVerification();
void deserialization(char message[]);

void setup() {
  Serial.begin(115200);    // Inicia la comunicación serial a 115200 baudios
  Serial.println("Start"); // Indica el inicio del programa en el monitor serial
  setupWifi();
  setupMqtt();
}
void loop() { MQTTStateVerification(); }

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");

  // Crear un buffer para el mensaje
  char message[length + 1]; // +1 para el carácter nulo de terminación

  // Copiar el payload al buffer y añadir carácter nulo al final
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0'; // Asegurarse de que la cadena esté terminada en NULL

  // Imprimir el mensaje convertido de payload a una cadena de caracteres
  Serial.println(message);

  // Pasar el mensaje a la función deserialization
  deserialization(message);
}

void setupWifi() {
  Serial.println("iniciando wifi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi conectado");
}

void setupMqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
{"id": "12345678", "pwm": "89"}
void MQTTStateVerification() {
  static bool subscribed = false;
  while (!client.connected() || !subscribed) {
    if (client.connect("ESP32Client")) {
      subscribed = client.subscribe("ae4/pwm");
    } else {
      delay(5000);
    }
  }
  client.loop();
}

void deserialization(char message[]) {
  const int MAX_INPUT_LENGTH = 200;

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, message, MAX_INPUT_LENGTH);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  id = doc["id"];
  percentage_control = doc["pwm"];

  if(id == 12345678){

    Serial.println("id: " + String(id) + " // percent_control: " + String(percentage_control));
  }

}