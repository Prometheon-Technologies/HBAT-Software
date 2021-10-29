/*
 HMS.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMS_h
#define HMS_h
#include "Arduino.h"

class HMS
{
  public:
    HMS();
    float* readSensAndCondition();
    void calibrateAmps();
    float *readAmps();
    float readVoltage(int pinnumber);
    uint8_t _amppin = 18;
    void setupSensor();
  /* private: */
};
#endif