/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HUMIDITY_h
#define HUMIDITY_h
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
  bool enableHeater = false;
  uint8_t loopCnt = 0;

  Adafruit_SHT31 sht31 = Adafruit_SHT31();

  Adafruit_SHT31 sht31_2 = Adafruit_SHT31();

  byte degree[8] =
      {
          0b00011,
          0b00011,
          0b00000,
          0b00000,
          0b00000,
          0b00000,
          0b00000,
          0b00000};

private:
  bool sensor1 = sht31.isHeaterEnabled();
  bool sensor2 = sht31_2.isHeaterEnabled();
};
#endif