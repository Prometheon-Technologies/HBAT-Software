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
  void SetupSensor();
  float StackHumidity();
  float AverageStackTemp();
  float ReadSensor();
  void HumRelayOnOff(int time, float *stack_humidity);
  // Setup an array of relays to control peripherals. Numbers represent pin numbers.
  const int relays[5] = {45, 38, 36, 35, 48};
  void setup_relays();
  /* private:
    int _pin;
    const int ADC1 = analogRead(_pin); */
};
#endif