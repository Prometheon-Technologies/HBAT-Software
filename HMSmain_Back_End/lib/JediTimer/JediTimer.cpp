/*
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
 *
 */
#include <JediTimer.h>

JediTimer::JediTimer(void)
{
    delay(1000);
    Serial.begin(Serial_Baud);
    pinMode(ledPin, OUTPUT);
}

JediTimer::~JediTimer(void)
{
}

void JediTimer::LoopTimer()
{
    delay(delayBetweenTimings);
    RunTimer();
}

void JediTimer::RunTimer()
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