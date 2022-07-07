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
  String generateDeviceID();

  struct Cell_Voltages_t
  {
#if !PRODUCTION
    float cell_voltage[5];
#else
    float cell_voltage[10];
#endif // !PRODUCTION
  };

  Cell_Voltages_t cell_voltages;

  Cell_Voltages_t *getCell_Voltages() { return &this->cell_voltages; }
  Cell_Voltages_t readSensAndCondition();

private:
  bool _mux_enabled_voltage;
  bool _mux_enabled_amps;
  int _power_mux_pin_amps;
  int _power_mux_pin_voltage;
  bool _POWER_MUX_ENABLED_AMPS;
  bool _POWER_MUX_ENABLED_VOLTAGE;
  // byte _voltage_pins[100];
};

extern HMS HMSmain;

#endif