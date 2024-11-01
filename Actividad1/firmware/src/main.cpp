#include <Arduino.h>
#include "color_alarm.h"
#include "temperature.h"

float refreshTemperature();
void controlAlarms();

void setup()
{
  Serial.begin(115200);
  IUDA::ColorAlarm::setupColorAlarm();
  IUDA::Temperature::setupTemperature();
}

void loop()
{
  IUDA::ColorAlarm::programRunning();
  controlAlarms();
}

float refreshTemperature()
{
  static float temperature = IUDA::Temperature::getTemperature();
  static unsigned long current_time = millis();
  const int refresh_time = 1000;

  if (millis() - current_time <= refresh_time)
  {
    return temperature;
  }

  temperature = IUDA::Temperature::getTemperature();
  current_time = millis();

  return temperature;
}

void controlAlarms()
{
  static int normal_temperature = 0;
  static int max_temperature_state = 1;
  static int min_temperature_state = 2;
  static int actual_state = normal_temperature;

  const int max_temperature = 27;
  const int min_temperature = 25;

  float temperature = refreshTemperature();

  if (temperature >= max_temperature)
  {
    if (actual_state != max_temperature_state)
    {
      IUDA::ColorAlarm::maxTemperature();
      actual_state = max_temperature_state;
    }
  }
  else if (temperature <= min_temperature)
  {
    if (actual_state != min_temperature_state)
    {
      IUDA::ColorAlarm::minTemperature();
      actual_state = min_temperature_state;
    }
  }
  else
  {
    if (actual_state != normal_temperature)
    {
      IUDA::ColorAlarm::turnOff();
      actual_state = normal_temperature;
    }
  }
}
