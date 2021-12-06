/*
 HMS.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMS_h
#define HMS_h
#include <Arduino.h>
#include <Wire.h>
#include <ACS712.h>
#include <Adafruit_SHT31.h>

class HMS
{
public:
  HMS(void);
  virtual ~HMS(void);
  float *readSensAndCondition();
  void calibrateAmps();
  String readAmps();
  float readVoltage(int pinnumber);
  void setupSensor();
  int voltageValues[10];

private:
};
#endif