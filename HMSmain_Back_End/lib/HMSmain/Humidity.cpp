#include <Humidity.h>

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

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

Adafruit_SHT31 sht31_2 = Adafruit_SHT31();

struct humidity_array
{
  float stack_humidity;
  float stack_temp;
};

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

void Humidity::SetupSensor()
{
  Serial.printf("SHT31 Sensors Setup Beginning");
  // Set to 0x45 for alternate i2c address
  if (!sht31.begin(0x44) ^ !sht31_2.begin(0x45))
  {
    Serial.printf("Couldn't find SHT31 sensors");
    while (1)
      delay(1);
  }
  Serial.printf("SHT31 Sensors Setup Complete");
  delay(1); // delay in between reads for stability

  Serial.print("Heater Enabled State: ");

  switch (sht31.isHeaterEnabled())
  {
  case false:
    Serial.printf("Sensor 1 Heater Disabled");
    break;
  case true:
    Serial.printf("Sensor 2 Heater ENABLED");
    break;
  }

  switch (sht31_2.isHeaterEnabled())
  {
  case false:
    Serial.printf("Sensor 1 Heater Disabled");
    break;
  case true:
    Serial.printf("Sensor 2 Heater ENABLED");
    break;
  }
}

float Humidity::AverageStackTemp()
{
  float stack_temp[4];
  for (int i = 0; i < 4; i++)
  {
    stack_temp[i] = ReadSensor();
  }
  return (stack_temp[0] + stack_temp[2]) / 2; // Read the temperature from the sensor and averge the two sensors.
}

float Humidity::StackHumidity()
{
  float stack_humidity[4];

  for (int i = 0; i < 4; i++)
  {
    stack_humidity[i] = ReadSensor();
  }
  return (stack_humidity[1] + stack_humidity[3]) / 2;
}

float Humidity::ReadSensor()
{
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  float t_2 = sht31_2.readTemperature();
  float h_2 = sht31_2.readHumidity();

  // float climatedata[4] = {t, h, t_2, h_2};

  if (!isnan(t and t_2))
  { // check if 'is not a number'
    /* climatedata[0] = t;
    climatedata[2] = t; */
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
    /* climatedata[1] = h;
    climatedata[3] = h_2; */
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
    case false:
      Serial.printf("Sensor 1 Heater Disabled");
      break;
    case true:
      Serial.printf("Sensor 1 Heater ENABLED");
      break;
    }

    switch (sht31_2.isHeaterEnabled())
    {
    case false:
      Serial.printf("Sensor 1 Heater Disabled");
      break;
    case true:
      Serial.printf("Sensor 1 Heater ENABLED");
      break;
    }

    loopCnt = 0;
  }
  return t, h, t_2, h_2;
}