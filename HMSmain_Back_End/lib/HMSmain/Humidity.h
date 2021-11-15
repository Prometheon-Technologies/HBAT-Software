/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef Humidity_h
#define Humidity_h
#include "Arduino.h"

class Humidity
{
  public:
    Humidity();
    void setupSensor();
    float Stackhumidity();
    float Stacktemp();
    float *ReadSensor();
    int hum_relay_On_Off(int time);
  /* private:
    int _pin;
    const int ADC1 = analogRead(_pin); */
};
#endif