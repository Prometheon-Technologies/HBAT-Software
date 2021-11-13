#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include "Humidity.h"
#include <PID_v1.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#endif

// Setup an array of relays to control peripherals. Numbers represent pin numbers.
const int relays[9] = {45, 38, 36, 35, 48};

// Define Variables we'll be connecting to
double Setpoint, Input, Output;

// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;

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
  pinMode(relays[0, 1, 2, 3, 4, 5, 6, 7, 8, 9], OUTPUT);

  windowStartTime = millis();

  // initialize the variables we're linked to
  Setpoint = 80;

  // tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  // turn the PID on
  myPID.SetMode(AUTOMATIC);
  Serial.printf("SHT31 Sensors Setup Beginning");
  if (!sht31.begin(0x44) and !sht31_2.begin(0x45))

  { // Set to 0x45 for alternate i2c addr
    Serial.printf("Couldn't find SHT31");
    while (1)
      delay(1);
  }
  Serial.printf("SHT31 Sensors Setup Complete");
  delay(1); // delay in between reads for stability

  Serial.print("Heater Enabled State: ");

  switch (sht31.isHeaterEnabled())
  {
  case 0:
    Serial.printf("Sensor 1 Heater Disabled");
    break;
  case 1:
    Serial.printf("Sensor 2 Heater ENABLED");
    break;
  }

  switch (sht31_2.isHeaterEnabled())
  {
  case 0:
    Serial.printf("Sensor 1 Heater Disabled");
    break;
  case 1:
    Serial.printf("Sensor 2 Heater ENABLED");
    break;
  }
}

float Stacktemp()
{
  float temp[4] = ReadSensor(); 
  return (temp.climatedata[0] + temp.climatedata[2]) / 2;
}

float Stackhumidity()
{
  float humidity[4] = ReadSensor(); 
  return (humidity.climatedata[1] + humidity.climatedata[3]) / 2;
}

float *ReadSensor()
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
  // This is needed due to the high operating humidity of the system
  if (++loopCnt == 30)
  {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    sht31_2.heater(enableHeater);
    Serial.print("Heater Enabled State: ");

    switch (sht31.isHeaterEnabled())
    {
    case 0:
      Serial.printf("Sensor 1 Heater Disabled");
      break;
    case 1:
      Serial.printf("Sensor 1 Heater ENABLED");
      break;
    }

    switch (sht31_2.isHeaterEnabled())
    {
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

int Humidity::hum_relay_On_Off(int time)
{
  float climate_data = Stackhumidity();
  Input = climate_data;
  myPID.Compute();

  // turn the output pin on/off based on pid output

  unsigned long now = millis();
  if (now - windowStartTime > WindowSize)
  { // time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output > now - windowStartTime)
    digitalWrite(relays[0, 1], HIGH);
  else
    digitalWrite(relays[0, 1], LOW);
}