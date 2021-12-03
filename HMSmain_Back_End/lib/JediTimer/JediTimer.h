/*
 JediTimer.h - Library for timing events.
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef JEDITIMER_h
#define JEDITIMER_h
#include <Arduino.h>

class JediTimer
{
public:
    JediTimer();
    void RunTimer();
    void LoopTimer();

private:
    word delayBetweenTimings = 1000;
};
#endif