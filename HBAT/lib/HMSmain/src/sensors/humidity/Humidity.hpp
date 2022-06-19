/*
 Humidity.hpp - HBAT Humidity library
 Copyright (c) 2021 ZanzyTHEbar
 */

#ifndef HUMIDITY_HPP
#define HUMIDITY_HPP
#include <defines.hpp>
#if USE_SFM3003
#include <SensirionI2CSfmSf06.h>
#endif // USE_SFM3003
#include <Wire.h>
#include <Adafruit_SHT31.h>

class Humidity
{
public:
  // Constructor
  Humidity();
  virtual ~Humidity();
  // Initialize the library
  byte setupSensor();
  bool checkHeaterEnabled();
  float StackHumidity();
  float AverageStackTemp();

  struct Hum
  {
    float temp;
    float humidity;
    float temp_2;
    float humidity_2;
    float flow;
    float temperature;
  };

  Hum result;

  Hum ReadSensor();
  
#if USE_SFM3003
  void sfm3003Setup();
  Hum sfm3003Loop();
#endif // USE_SFM3003

private:
  // Variables
  byte _status;
  byte _loopCnt;
  bool _enableHeater;
  int _offset; // _Offset for the sensor
  float _scale; // _Scale factor for Air and N2 is 140.0, O2 is 142.8
};
extern Humidity humidity;
#endif