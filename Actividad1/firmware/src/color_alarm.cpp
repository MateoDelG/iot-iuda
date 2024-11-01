#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "color_alarm.h"

#define COLOR_ALARM_PIN 23 //LEDS pin
#define NUMPIXELS 6 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, COLOR_ALARM_PIN, NEO_GRB + NEO_KHZ800);

namespace IUDA{
    namespace ColorAlarm{
        /**
         * @brief Initializes the NeoPixel strip.
         * 
         * This function needs to be called once before any other color alarm
         * functions. It sets up the NeoPixel strip and allocates memory for
         * the pixels. After calling this function the strip is ready to be
         * used.
         */ 
        void setupColorAlarm(){
            pixels.begin();          
        }

        void turnOff(){
            for(int i=0; i<NUMPIXELS-1; i++){
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            }
            pixels.show();
            // pixels.clear();
            Serial.println("[!]COLOR ALARM OFF");
        }

        void maxTemperature(){
            for(int i=0; i<NUMPIXELS-1; i++){
                pixels.setPixelColor(i, pixels.Color(255, 0, 0));
            }
            pixels.show();
            Serial.println("[!] MAX TEMPERATURE");
        }

        void minTemperature(){
            for(int i=0; i<NUMPIXELS-1; i++){
                pixels.setPixelColor(i, pixels.Color(0, 0, 255));
            }
            pixels.show();
            Serial.println("[!] MIN TEMPERATURE");
        }

        /**
         * @brief Blinks the last LED on the strip to show that the program is running.
         * 
         * This function should be called in the loop() function of the Arduino sketch.
         * It will blink the last LED on the strip and print a message to the serial
         * console to indicate that the program is running.
         */ 

        void programRunning(){
            static bool led_state = false;
            const int last_led = NUMPIXELS-1;
            static unsigned long current_time = millis();
            const int blink_delay = 300;

            if(millis() - current_time <= blink_delay){
                return;
            }

            if(led_state){
                pixels.setPixelColor(last_led, pixels.Color(100, 100, 100));
                pixels.show();
                led_state = false;
                // Serial.println("LED STATE ON");
            }else{
                pixels.setPixelColor(last_led, pixels.Color(0, 0, 0));
                pixels.show();
                led_state = true;
                // Serial.println("LED STATE OFF");
            }
            // Serial.println("[!] PROGRAM RUNNING");
            current_time = millis();
        }
    
    }
}