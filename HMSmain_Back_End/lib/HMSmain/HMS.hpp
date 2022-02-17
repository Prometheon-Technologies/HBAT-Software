/*
 HMS.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMS_h
#define HMS_h
#include <defines.hpp>

class HMS
{
public:
  HMS(void);
  virtual ~HMS(void);
  void calibrateAmps();
  void setupSensor();
  int readAmps();
  int ChargeStatus();
  int voltageValues[10];
  float sumArray(float array[], int size);
  float readVoltage(int pinnumber);
  float StackVoltage();
  float *readSensAndCondition();
  String getDeviceID();
  String generateDeviceID();
  char *StringtoChar(String inputString);

private:
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SCANNER)
extern HMS HMSmain;
#endif
#endif