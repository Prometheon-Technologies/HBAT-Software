/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef Humidity_h
#define Humidity_h
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

class Humidity
{
public:
  Humidity();
  void SetupSensor();
  float StackHumidity();
  float AverageStackTemp();
  float *ReadSensor();
  /* private:
    int _pin;
    const int ADC1 = analogRead(_pin); */
};
#endif