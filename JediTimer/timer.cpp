/******************************************************************************
 *                  Timer by JediRick
 *
 *              BOARD                 MICROS
 *
 *    rasPi Pico:                     1334
 *    Arduino Nano RP2040 Connect:    1408
 *    Arduino Due:                    4464
 *    Arduino Nano Every:             6854
 *    Arduino Uno:                    6904
 *    pro Micro:                      10560
 *    elegoo Mega 2650:               11224
 ******************************************************************************/
#include <Arduino.h>
#if defined(ESP8266)
#define ledPin 2
#pragma message "This board is an ESP8266"
#elif defined(ESP32)
#pragma message "This board is an ESP32"
#define ledPin 2
#else
#error "This ain't a ESP8266 or ESP32, most likely an Arduino board"
#define ledPin LED_BUILTIN
#endif

word delayBetweenTimings = 1000;

void setup()
{
    delay(1000);
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void loop()
{
    delay(delayBetweenTimings);
    runTimer();
}

void runTimer()
{
    uint32_t startTimer;
    uint32_t endTimer;
    uint32_t elapsed;
    Serial.println("----------------------------------");
    Serial.println("Running Timer");
    startTimer = micros();
    for (int i = 0; i < 1000; i++)
    {
        digitalWrite(ledPin, HIGH);
        digitalWrite(ledPin, LOW);
    }
    endTimer = micros();
    elapsed = endTimer - startTimer;
    Serial.println("Result in micros: " + (String)elapsed);
    Serial.println("----------------------------------");
}