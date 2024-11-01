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

/**
 * @brief Refreshes the temperature only every 2 seconds.
 *
 * This function is used to limit the amount of times the temperature is
 * refreshed from the DS18B20 sensor. It is refreshed every 2 seconds.
 *
 * @return The current temperature.
 */
float refreshTemperature()
{
  static float temperature = IUDA::Temperature::getTemperature();
  static unsigned long current_time = millis();
  const int refresh_time = 2000;

  if (millis() - current_time <= refresh_time)
  {
    return temperature;
  }

  temperature = IUDA::Temperature::getTemperature();
  Serial.println("Temperature = " + String(temperature));
  current_time = millis();

  return temperature;
}

/**
 * @brief Controls the color alarm based on the current temperature.
 *
 * This function is the main state machine for the color alarm. It
 * controls the color alarm based on the current temperature. The
 * temperatures are defined as follows:
 * - The alarm is turned off if the temperature is between
 *   26 and 27 degrees Celsius.
 * - The alarm is set to the "min temperature" color if the
 *   temperature is below 26 degrees Celsius.
 * - The alarm is set to the "max temperature" color if the
 *   temperature is above 27 degrees Celsius.
 */
void controlAlarms()
{
  static int normal_temperature = 0;
  static int max_temperature_state = 1;
  static int min_temperature_state = 2;
  static int actual_state = normal_temperature;

  const int max_temperature = 27;
  const int min_temperature = 26;

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
