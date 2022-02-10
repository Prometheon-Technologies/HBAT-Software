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
  int SFM3003();
  int SetupSFM3003();

  // Variables
  float returnData[2];
  float flow;
  float temperature;
  int relays[5];
  
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
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HMSMQTT)
extern Humidity Hum;
#endif
#endif