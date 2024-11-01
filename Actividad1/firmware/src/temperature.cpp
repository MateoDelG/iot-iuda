#include <Arduino.h>
#include "DS18B20.h"
#include "temperature.h"

#define ONE_WIRE_BUS 22

OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire);

namespace IUDA
{
    namespace Temperature
    {
        void setupTemperature()
        {
            sensor.begin();
            sensor.setResolution(10);
        }

        float getTemperature()
        {
            sensor.requestTemperatures();
            //  wait until sensor is ready
            while (!sensor.isConversionComplete())
            {
                delay(1);
                // Serial.println("Waiting...");
            }
            // Serial.print("Temp: ");
            // Serial.println(sensor.getTempC());
            return sensor.getTempC();
        }

    }
}
