/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HUMIDITY_h
#define HUMIDITY_h
#include <defines.hpp>

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
  void setupSfm3003();
  void loopSFM3003();

  // Variables
private:
  int _received;
  bool _sensor1 = sht31.isHeaterEnabled();
  bool _sensor2 = sht31_2.isHeaterEnabled();
  byte _degree[8] =
      {
          0b00011,
          0b00011,
          0b00000,
          0b00000,
          0b00000,
          0b00000,
          0b00000,
          0b00000};
  int _offset;  // Offset for the sensor
  float _scale; // Scale factor for Air and N2 is 140.0, O2 is 142.8
  float _returnData[2];
  float _flow;
  float _temperature;
  int _relays[5];
};
extern Humidity Hum;
#endif