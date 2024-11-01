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
        /**
         * @brief Initialises the DS18B20 temperature sensor.
         *
         * This function must be called before any other function in this
         * namespace can be used. It initialises the sensor and sets its
         * resolution to 10 bits.
         */
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
