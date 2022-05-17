/*
 Humidity.hpp - HBAT Humidity library
 Copyright (c) 2021 ZanzyTHEbar
 */

#ifndef HUMIDITY_HPP
#define HUMIDITY_HPP
#include <defines.hpp>
#include <SensirionI2CSfmSf06.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

class Humidity
{
public:
  // Constructor
  Humidity();
  virtual ~Humidity();
  // Initialize the library
  int setupSensor();
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
  void sfm3003Setup();
  void sfm3003Loop();
  /* int loopSFM3003();
  uint8_t crc8(const uint8_t data, uint8_t crc); */

  // Variables
private:
};
extern Humidity humidity;
#endif