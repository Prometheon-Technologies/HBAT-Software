/*
asyncserver.h - HBAT asyncserver library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef asyncserver_h
#define asyncserver_h
#include "Arduino.h"

class Asyncserver
{
  public:
    Asyncserver();
    void setupSensor();
    float *ReadSensor();
  /* private:
    int _pin;
    const int ADC1 = analogRead(_pin); */
};
#endif