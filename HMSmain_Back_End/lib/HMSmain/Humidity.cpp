#include <Humidity.h>

Humidity::Humidity()
{
}

/******************************************************************************
 * Function: Setup Humidity Sensors
 * Description: This function is used to initialise the humidity sensors and their respective heaters
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Humidity::SetupSensor()
{
  Serial.println("SHT31 Sensors Setup Beginning");
  // Set to 0x45 for alternate i2c address
  if (!sht31.begin(0x44) ^ !sht31_2.begin(0x45))
  {
    Serial.println("Couldn't find SHT31 sensors");
    while (1)
      delay(1);
  }
  Serial.println("SHT31 Sensors Setup Complete");
  delay(1); // delay in between reads for stability

  if (sensor1 ^ sensor2)
  {
    Serial.println("Sensors have Heater ENABLED");
  }
  else
  {
    Serial.println("Sensor 1 Heater Disabled");
  }
}

/******************************************************************************
 * Function: Average Stack Humidity Temp
 * Description: This function is used to average the Temp of the stack - from the temp sensors built into the Humidity Sensors
 * Parameters: None
 * Return: float
 ******************************************************************************/
float Humidity::AverageStackTemp()
{
  float stack_temp[4];
  for (int i = 0; i < 4; i++)
  {
    stack_temp[i] = *ReadSensor();
  }
  return (stack_temp[0] + stack_temp[2]) / 2; // Read the temperature from the sensor and averge the two sensors.
}

/******************************************************************************
 * Function: Average Stack Humidity
 * Description: This function is used to average the humidity of the stack
 * Parameters: None
 * Return: float
 ******************************************************************************/
float Humidity::StackHumidity()
{
  float stack_humidity[4];

  for (int i = 0; i < 4; i++)
  {
    stack_humidity[i] = *ReadSensor();
  }
  return (stack_humidity[1] + stack_humidity[3]) / 2;
}

/******************************************************************************
 * Function: Read Humidity Sensors
 * Description: This function is used to read the humidity of the stack sensors
 * Parameters: None
 * Return: float array
 ******************************************************************************/
float *Humidity::ReadSensor()
{
  float *climatedata = new float[4];

  // check if 'is not a number'
  if (!isnan(climatedata[0] and climatedata[1]))
  {
    climatedata[0] = sht31.readTemperature();
    climatedata[1] = sht31_2.readTemperature();
    Serial.println("Sensor 1 Temp *C = ");
    Serial.print(climatedata[0]);
    Serial.println("Sensor 2 Temp *C = ");
    Serial.print(climatedata[1]);
  }
  else
  {
    Serial.println("Failed to read temperature");
  }

  // check if 'is not a number'
  if (!isnan(climatedata[2] and climatedata[3]))
  {
    climatedata[2] = sht31.readHumidity();
    climatedata[3] = sht31_2.readHumidity();
    Serial.println("Sensor 1 Humidity %% = ");
    Serial.print(climatedata[2]);
    Serial.println("Sensor 2 Humidity %% = ");
    Serial.print(climatedata[3]);
  }
  else
  {
    Serial.println("Failed to read humidity");
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

    if (sensor1 ^ sensor2)
    {
      Serial.print("Sensors have Heater ENABLED");
    }
    else
    {
      Serial.println("Sensor 1 Heater Disabled");
    }

    loopCnt = 0;
  }
  return climatedata;
}

/******************************************************************************
 * Function: Control MPX2010DP - K014308 and AD623
 * Description: This function allows for manual calibration of the ACS712 sensor by setting the sensitivity to the correct value
 * Parameters: None
 * Return: None
 ******************************************************************************/