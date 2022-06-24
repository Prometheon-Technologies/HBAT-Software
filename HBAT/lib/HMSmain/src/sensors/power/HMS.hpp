/*
 HMS.h - HBAT library
 Copyright (c) 2021 ZanzyTHEbar
 */

#ifndef HMS_HPP
#define HMS_HPP
#include <defines.hpp>
#include <ACS712.h>

class HMS
{
public:
  HMS();
  virtual ~HMS();
  void begin();
  void calibrateAmps();
  void setupSensor();
  int readAmps();
  float readVoltage(byte pinnumber);
  double readVoltagePolynomial(byte pinnumber);
  float *readSensAndCondition();
  String generateDeviceID();

private:
  bool _mux_enabled_voltage;
  bool _mux_enabled_amps;
  int _power_mux_pin_amps;
  int _power_mux_pin_voltage;
  bool _POWER_MUX_ENABLED_AMPS;
  bool _POWER_MUX_ENABLED_VOLTAGE;
  byte _voltage_pins[100];
};

extern HMS HMSmain;

#endif