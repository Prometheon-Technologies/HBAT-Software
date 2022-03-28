/*
 Humidity.h - HBAT Humidity library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HUMIDITY_h
#define HUMIDITY_h
#include "defines.hpp"
#include <Adafruit_SHT31.h>

extern Adafruit_SHT31 sht31;
extern Adafruit_SHT31 sht31_2;

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
  void setupSfm3003();
  int loopSFM3003();
  uint8_t crc8(const uint8_t data, uint8_t crc);

  // Variables
  int returnData[2];
  int flow;
  int temperature;
private:
  int _received;
  bool _sensor1;
  bool _sensor2;
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
};

extern Humidity Hum;
#endif