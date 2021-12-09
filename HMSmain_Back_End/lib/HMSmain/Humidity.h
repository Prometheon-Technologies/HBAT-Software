/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HUMIDITY_h
#define HUMIDITY_h
#define MAXNUMOFRELAYS 5
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <PID_v1.h>
#include <sfm3003.h>

class Humidity
{
public:
  // Constructor
  Humidity(void);
  virtual ~Humidity(void);
  // Initialize the library
  void SetupSensor();
  float StackHumidity();
  float AverageStackTemp();
  float *ReadSensor();
  void HumRelayOnOff();
  void SetupPID();
  void SetupRelays();
  int SFM3003();
  int SetupSFM3003();
  // Variables
  Adafruit_SHT31 sht31 = Adafruit_SHT31();

  Adafruit_SHT31 sht31_2 = Adafruit_SHT31();

private:
  int received;
  bool sensor1 = sht31.isHeaterEnabled();
  bool sensor2 = sht31_2.isHeaterEnabled();
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
};
#endif