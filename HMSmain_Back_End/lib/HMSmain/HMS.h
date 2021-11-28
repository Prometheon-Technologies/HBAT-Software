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
#include <Humidity.h>
#include <CellTemp.h>

class HMS
{
public:
  HMS();
  float *readSensAndCondition();
  void calibrateAmps();
  String readAmps();
  float readVoltage(int pinnumber);
  void setupSensor();
  int voltageValues[10];

private:
};
#endif