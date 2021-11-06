#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include "Humidity.h"

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();
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

Humidity::Humidity()
{
}

void Humidity::setupSensor()
{
  Serial.printf("SHT31 Setup Beginning");
  if (!sht31.begin(0x44))
  { // Set to 0x45 for alternate i2c addr
    Serial.printf("Couldn't find SHT31");
    while (1)
      delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.printf("ENABLED");
  else
    Serial.printf("DISABLED");
}

float *Humidity::ReadSensor()
{
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  float climatedata[2];

  if (!isnan(t))
  { // check if 'is not a number'
    climatedata[0] = t;
    Serial.printf("Temp *C = ");
    Serial.print(t);
    Serial.print("\t\t");
  }
  else
  {
    Serial.printf("Failed to read temperature");
  }

  if (!isnan(h))
  { // check if 'is not a number'
    climatedata[1] = h;
    Serial.print("Hum. % = ");
    Serial.print(h);
  }
  else
  {
    Serial.printf("Failed to read humidity");
  }

  delay(1000);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (++loopCnt == 30)
  {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.printf("ENABLED");
    else
      Serial.printf("DISABLED");

    loopCnt = 0;
  }
  return climatedata;
}