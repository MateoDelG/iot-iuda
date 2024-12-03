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


void setupWifi();
void setupMqtt();
void MQTTStateVerification();
void publishManager(String msg);
float simulationSensorTemperature();
int simulationSensorHumidity();
String serializeJson(float temperature, int humidity);


void setup()
{
  Serial.begin(115200);                     // Inicia la comunicación serial a 115200 baudios
  Serial.println("Start");                  // Indica el inicio del programa en el monitor serial
  setupWifi();
  setupMqtt();
  
}

void loop()
{
  MQTTStateVerification();

  float temperature = simulationSensorTemperature();
  int humidity = simulationSensorHumidity();
  String message = serializeJson(temperature, humidity);
  publishManager(message);
}


void setupWifi(){
  Serial.println("iniciando wifi");
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi conectado");
}

void setupMqtt(){
  client.setServer(mqtt_server, mqtt_port);
}

void MQTTStateVerification(){
  if(!client.connected()){
    Serial.println("esperando al conexion MQTT");

    while(!client.connected()){
      if(client.connect("ESP32Client")){
        Serial.println("conectado al broker MQTT");
      }
      else{
        Serial.println("fallo la conexion al broker MQTT");
        delay(5000);
      }
    }
  }
}

void publishManager(String msg){
  static unsigned long current_time = 0;
  const int publish_interval = 5000;

  if (millis() > current_time + publish_interval)
  {

  client.publish("iud/prueba_transmision", msg.c_str());

  Serial.println("mensaje publicado: " + msg);

  current_time = millis();
  }

}

String serializeJson(float temperature, int humidity)
{
JsonDocument doc;

doc["sensor"] = "temp001";
doc["type"] = "sensor";

JsonArray values = doc["values"].to<JsonArray>();
values.add(temperature);
values.add(humidity);

String output;

doc.shrinkToFit();  // optional

serializeJson(doc, output);
// Serial.println(output);
return output;
}

float simulationSensorTemperature()
{
  return random(10, 100) + random(0, 10) / 10.0;
}

int simulationSensorHumidity()
{
  return random(5, 60);
}

void stayAlive()
{
  static unsigned long current_time_alive = 0; // Variable para mensajes de keep-alive
  // Imprime un mensaje cada segundo para mostrar que el programa está ejecutándose
  if (millis() > current_time_alive + 1000)
  {
    Serial.println("ejecutando programa");
    current_time_alive = millis();
  }
}