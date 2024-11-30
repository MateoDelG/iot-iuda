#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>


const char *ssid = "Delga";
const char *password = "Delga1213";

const char *mqtt_server = "broker.mqtt.cool";
#define mqtt_port 1883

WiFiClient espClient;
PubSubClient client(espClient);


int id = 0;
int pwm = 0;

void setupWifi();
void setupMqtt();
void callback(char *topic, byte *payload, unsigned int length);
void MQTTStateVerification();
void deserialization(char message[]);

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMqtt();
}

void loop() { MQTTStateVerification(); }

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

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");

  // Crear un buffer para el mensaje
  char message[length + 1]; // +1 para el caracter nulo de terminacion
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0'; // Asegurarse de que la cadena este terminada en NULL

  // Imprimir el mensaje convertido de payload a una cadena de caracteres
  Serial.println(message);

  deserialization(message);
}

void MQTTStateVerification() {
  static bool subscribed = false;
  while (!client.connected() || !subscribed) {
    if (client.connect("12345678")) {
      subscribed = client.subscribe("fan_speed/pwm");
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

  id = doc["id"];   // "12345678"
  pwm = doc["pwm"]; // "89"

  if (id == 12345678) {
    Serial.println("Este mensaje es para mi");
    Serial.print("id: ");
    Serial.println(id);
    Serial.print("pwm: ");
    Serial.println(pwm);
  }else{
    Serial.println("Este mensaje no es para mi");
  }
}
