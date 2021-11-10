#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include "Humidity.h"

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

Adafruit_SHT31 sht31_2 = Adafruit_SHT31();

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
  Serial.printf("SHT31 Sensors Setup Beginning");
  if (!sht31.begin(0x44) and !sht31_2.begin(0x45))

  { // Set to 0x45 for alternate i2c addr
    Serial.printf("Couldn't find SHT31");
    while (1)
      delay(1);
  }
  Serial.printf("SHT31 Sensors Setup Complete");
  delay(1);        // delay in between reads for stability

  Serial.print("Heater Enabled State: ");

  switch (sht31.isHeaterEnabled()) {
    case 0:
      Serial.printf("Sensor 1 Heater Disabled");
      break;
    case 1:
      Serial.printf("Sensor 2 Heater ENABLED");
      break;
  }

  switch (sht31_2.isHeaterEnabled()) {
    case 0:
      Serial.printf("Sensor 1 Heater Disabled");
      break;
    case 1:
      Serial.printf("Sensor 2 Heater ENABLED");
      break;
  }
}

float *Humidity::ReadSensor()
{
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  float t_2 = sht31_2.readTemperature();
  float h_2 = sht31_2.readHumidity();

  float climatedata[4] = {t, h, t_2, h_2};

  if (!isnan(t and t_2))
  { // check if 'is not a number'
    climatedata[0] = t;
    climatedata[2] = t;
    Serial.printf("Sensor 1 Temp *C = ");
    Serial.print(t);
    Serial.printf("Sensor 2 Temp *C = ");
    Serial.print(t_2);
  }
  else
  {
    Serial.printf("Failed to read temperature");
  }

  if (!isnan(h and h_2))
  { // check if 'is not a number'
    climatedata[1] = h;
    climatedata[3] = h_2;
    Serial.printf("Sensor 1 Humidity % = ");
    Serial.print(h);
    Serial.printf("Sensor 2 Humidity % = ");
    Serial.print(h_2);
  }
  else
  {
    Serial.printf("Failed to read humidity");
  }

  delay(1000);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  //This is needed due to the high operating humidity of the system
  if (++loopCnt == 30)
  {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    sht31_2.heater(enableHeater);
    Serial.print("Heater Enabled State: ");

    switch (sht31.isHeaterEnabled()) {
      case 0:
        Serial.printf("Sensor 1 Heater Disabled");
        break;
      case 1:
        Serial.printf("Sensor 1 Heater ENABLED");
        break;
    }

    switch (sht31_2.isHeaterEnabled()) {
      case 0:
        Serial.printf("Sensor 1 Heater Disabled");
        break;
      case 1:
        Serial.printf("Sensor 1 Heater ENABLED");
        break;
    }

    loopCnt = 0;
  }
  return climatedata;
}