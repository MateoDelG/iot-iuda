#include <Arduino.h>
#include <UbidotsEsp32Mqtt.h>

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-mWiBI8JCk9GDPzsoptkJQ6CNUGRtCa";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "ESP-IUD";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "12345678";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "test1";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "temp1"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 10000; // Update rate in milliseconds

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
}

void loop()
{

  static unsigned long current_time = 0;

  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if ((millis() - current_time) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float value = random(0, 10);
    ubidots.add(VARIABLE_LABEL, value); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    Serial.println("Published " + String(value) + " to " + DEVICE_LABEL + "/" + VARIABLE_LABEL);
    current_time = millis();
  }
  ubidots.loop();

}