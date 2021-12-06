/*
 JediTimer.h - Library for timing events.
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#if defined(ESP8266)
#define ledPin 2
#define Serial_Baud 9600
#pragma message "This board is an ESP8266"
#elif defined(ESP32)
#pragma message "This board is an ESP32"
#define ledPin 2
#define Serial_Baud 115200
#else
#error "This ain't a ESP8266 or ESP32, most likely an Arduino board"
#define ledPin LED_BUILTIN
#define Serial_Baud 9600
#endif

#ifndef JEDITIMER_h
#define JEDITIMER_h
#include <Arduino.h>

class JediTimer
{
public:
    JediTimer(void);
    virtual ~JediTimer(void);
    void RunTimer();
    void LoopTimer();

private:
    word delayBetweenTimings = 1000;
};
#endif