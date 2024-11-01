#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "color_alarm.h"

#define COLOR_ALARM_PIN 23 //LEDS pin
#define NUMPIXELS 6 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, COLOR_ALARM_PIN, NEO_GRB + NEO_KHZ800);

namespace IUDA{
    namespace ColorAlarm{
        void setupColorAlarm(){
            pixels.begin();          
        }

        void turnOff(){
            for(int i=0; i<NUMPIXELS; i++){
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            }
            Serial.println("COLOR ALARM OFF");
            pixels.show();
        }

        void maxTemperature(){
            for(int i=0; i<NUMPIXELS-1; i++){
                pixels.setPixelColor(i, pixels.Color(255, 0, 0));
            }
            pixels.show();
            Serial.println("MAX TEMPERATURE");
        }

        void minTemperature(){
            for(int i=0; i<NUMPIXELS-1; i++){
                pixels.setPixelColor(i, pixels.Color(0, 0, 255));
            }
            pixels.show();
            Serial.println("MIN TEMPERATURE");
        }

        void programRunning(){
            static bool led_state = false;
            const int last_led = NUMPIXELS;
            static unsigned long current_time = millis();
            const int blink_delay = 2000;

            if(millis() - current_time <= blink_delay){
                return;
            }

            if(led_state){
                pixels.setPixelColor(last_led, pixels.Color(100, 100, 100));
                pixels.show();
                led_state = false;
                Serial.println("LED STATE ON");
            }else{
                pixels.setPixelColor(last_led, pixels.Color(0, 0, 0));
                pixels.show();
                led_state = true;
                Serial.println("LED STATE OFF");
            }
            current_time = millis();
        }
    
    }
}