/*
 HMS.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMS_h
#define HMS_h
#include <ACS712.h>
#include "defines.hpp"

class HMS
{
public:
  HMS();
  virtual ~HMS();
  void calibrateAmps();
  void setupSensor();
  int readAmps();
  int ChargeStatus();
  float sumArray(float array[], int size);
  float readVoltage(int pinnumber);
  float StackVoltage();
  float *readSensAndCondition();
  String generateDeviceID();

private:
};

extern HMS HMSmain;

#endif