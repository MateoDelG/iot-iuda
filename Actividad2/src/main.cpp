#include <Arduino.h>
#include <UbidotsEsp32Mqtt.h>

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-OMrtBnZ59atggnkZXmROpTilSqM2ga";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Delga";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "Delga1213";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "temp001";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "temp_read001"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 60000; // Update rate in milliseconds

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



#include <Adafruit_LiquidCrystal.h>

int seconds = 0;

Adafruit_LiquidCrystal lcd_1(0);

void setup()
{
  lcd_1.begin(16, 2);

  lcd_1.print("hello world");
}

void loop()
{
  lcd_1.setCursor(0, 1);
  lcd_1.print(seconds);
  lcd_1.setBacklight(1);
  delay(500); // Wait for 500 millisecond(s)
  lcd_1.setBacklight(0);
  delay(500); // Wait for 500 millisecond(s)
  seconds += 1;
  lcd1.clear();
}