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
  int voltageValues[10];
  int sumArray(int array[], int size);
  int readVoltage(adc1_channel_t pinnumber);
  int StackVoltage();
  int *readSensAndCondition();
  String generateDeviceID();

private:
};

extern HMS HMSmain;

#endif