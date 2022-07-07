#include "Humidity.hpp"

// Global Variables
//  Setup an array of _relays to control peripherals. Numbers represent pin numbers.

Adafruit_SHT31 sht31;
Adafruit_SHT31 sht31_2;

#if USE_SFM3003
SensirionI2CSfmSf06 sfmSf06;
#endif // USE_SFM3003

Humidity::Hum result;

Humidity::Humidity() : _status(0),
                       _loopCnt(0),
                       _enableHeater(false),
                       _offset(32000),
                       _scale(140.0) // _Scale factor for Air and N2 is 140.0, O2 is 142.8
{
}

Humidity::~Humidity()
{
}

/******************************************************************************
 * Function: Setup Humidity Sensors
 * Description: This function is used to initialise the humidity sensors and their respective heaters
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Humidity::setupSensor()
{
  log_i("SHT31 Sensors Setup Beginning....");
  // Set to 0x45 for alternate i2c address
  if (!sht31.begin(0x44) && !sht31_2.begin(0x45))
  {
    log_i("Couldn't find SHT31 sensors");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring or the addresses and try again");
    stateManager.setState(ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_NONE);
  }
  else if (!sht31.begin(0x44) && sht31_2.begin(0x45))
  {
    log_i("Couldn't find SHT31 sensor #1");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring and try again");
    stateManager.setState(ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_1);
  }
  else if (!sht31_2.begin(0x45) && sht31.begin(0x44))
  {
    log_i("Couldn't find SHT31 sensor #2");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring and try again");
    stateManager.setState(ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_2);
  }
  else
  {
    log_i("SHT31 Sensors Setup Complete");
    stateManager.setState(ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_BOTH);
  }
  my_delay(2L); // delay in between reads for stability
}

bool Humidity::checkHeaterEnabled()
{
  switch (stateManager.getCurrentSensorState())
  {
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_NONE:
    log_d("No humidity sensors active");
    return false;
    break;
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_1:
  {
    bool _sensor1 = sht31.isHeaterEnabled();
    bool heaterenabled = false;
    if (_loopCnt >= 30)
    {
      _enableHeater = !_enableHeater;
      sht31.heater(_enableHeater);
      log_i("Heater Enabled State: %s", _enableHeater ? "ENABLED" : "DISABLED");
      if (_sensor1)
      {
        log_i("Sensor 1 Heater Heater ENABLED");
        heaterenabled = true;
      }
      else
      {
        log_i("Sensor 1 Heater Disabled");
        heaterenabled = false;
      }
      _loopCnt = 0;
    }
    _loopCnt++;
    return heaterenabled;
    break;
  }
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_2:
  {
    bool _sensor2 = sht31_2.isHeaterEnabled();
    bool heaterenabled = false;
    if (_loopCnt >= 30)
    {
      _enableHeater = !_enableHeater;
      sht31_2.heater(_enableHeater);
      log_i("Heater Enabled State: %s", _enableHeater ? "ENABLED" : "DISABLED");
      if (_sensor2)
      {
        log_i("Sensor 2 Heater ENABLED");
        heaterenabled = true;
      }
      else
      {
        log_i("Sensor 2 Heater Disabled");
        heaterenabled = false;
      }
      _loopCnt = 0;
    }
    _loopCnt++;
    return heaterenabled;
    break;
  }
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_BOTH:
  {
    bool _sensor1 = sht31.isHeaterEnabled();
    bool _sensor2 = sht31_2.isHeaterEnabled();
    bool heaterenabled = false;
    if (_loopCnt >= 30)
    {
      _enableHeater = !_enableHeater;
      sht31.heater(_enableHeater);
      sht31_2.heater(_enableHeater);
      log_i("Heater Enabled State: %s\t\t", _enableHeater ? "ENABLED" : "DISABLED");
      if (_sensor1 != _sensor2)
      {
        log_i("Sensors have Heater ENABLED");
        heaterenabled = true;
      }
      else
      {
        log_i("Sensors have Heater Disabled");
        heaterenabled = false;
      }
      _loopCnt = 0;
    }
    _loopCnt++;
    return heaterenabled;
    break;
  }
  default: // Should never get here
    return false;
    break;
  }
}

/******************************************************************************
 * Function: Average Stack Humidity Temp
 * Description: This function is used to average the Temp of the stack - from the temp sensors built into the Humidity Sensors
 * Parameters: None
 * Return: float
 * Note: This function MUST be called after the ReadSensor function
 ******************************************************************************/
float Humidity::AverageStackTemp()
{
  switch (stateManager.getCurrentSensorState())
  {
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_NONE:
  {
    result = {0.0, 0.0, 0.0, 0.0};
    float stack_humidity = 0.0;
    return stack_humidity; // return 0 if no sensors are active
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_1:
  {
    float stack_temp = result.temp;
    return stack_temp; // Only one sensor - return the value of that sensor
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_2:
  {
    float stack_temp = result.temp_2;
    return stack_temp; // Only one sensor - return the value of that sensor
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_BOTH:
  {
    float stack_temp = result.temp + result.temp_2;
    return stack_temp / 2.0; // Read the _temperature from the sensor and average the two sensors.
    break;
  }

  default:
  {
    break;
  }
  }
  return 0.0;
}

/******************************************************************************
 * Function: Average Stack Humidity
 * Description: This function is used to average the humidity of the stack
 * Parameters: None
 * Return: float
 * Note: This function MUST be called after the ReadSensor function
 ******************************************************************************/
float Humidity::StackHumidity()
{
  switch (stateManager.getCurrentSensorState())
  {
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_NONE:
  {
    result = {0.0, 0.0, 0.0, 0.0};
    float stack_humidity = 0.0;
    return stack_humidity; // return 0 if no sensors are active
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_1:
  {
    float stack_humidity = result.humidity;
    return stack_humidity; // Only one sensor - return the value of that sensor
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_2:
  {
    float stack_humidity = result.humidity_2;
    return stack_humidity; // Only one sensor - return the value of that sensor
    break;
  }

  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_BOTH:
  {
    float stack_humidity = result.humidity + result.humidity_2;
    return stack_humidity / 2.0; // Read the _humidity from the sensor and average the two sensors.
    break;
  }

  default:
  {
    break;
  }
  }
  return 0.0;
}

/******************************************************************************
 * Function: Read Humidity Sensors
 * Description: This function is used to read the humidity of the stack sensors
 * Parameters: None
 * Return: float array
 ******************************************************************************/
Humidity::Hum Humidity::ReadSensor()
{
  switch (stateManager.getCurrentSensorState())
  {
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_NONE:
  {
    result = {0.0, 0.0, 0.0, 0.0};
    log_d("No Humidity Sensors Active");
    return result;
    break;
  }
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_1:
  {
    float temp = sht31.readTemperature();
    float hum = sht31.readHumidity();
    // check if 'is not a number
    if (!isnan(temp))
    { // check if 'is not a number'
      log_i("Temp *C = %.3f\t\t", temp);
    }
    else
    {
      log_i("Failed to read temperature");
    }

    if (!isnan(hum))
    { // check if 'is not a number'
      log_i("Hum. % = %.3f\t\t", hum);
    }
    else
    {
      log_i("Failed to read humidity");
    }
    my_delay(0.1L); // delay in between reads for stability

    // Toggle heater enabled state every 30 seconds
    // An ~3.0 degC _temperature increase can be noted when heater is enabled
    // This is needed due to the high operating humidity of the system
    checkHeaterEnabled();
    result = {temp, hum, 0.0, 0.0};
    return result;
    break;
  }
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_2:
  {
    float temp_2 = sht31_2.readTemperature();
    float hum_2 = sht31_2.readHumidity();
    if (!isnan(temp_2))
    { // check if 'is not a number'
      log_i("Temp *C = %.3f\t\t", temp_2);
    }
    else
    {
      log_i("Failed to read temperature");
    }

    if (!isnan(hum_2))
    { // check if 'is not a number'
      log_i("Hum. % = %.3f\t\t", hum_2);
    }
    else
    {
      log_i("Failed to read humidity");
    }
    my_delay(0.1L); // delay in between reads for stability

    // Toggle heater enabled state every 30 seconds
    // An ~3.0 degC _temperature increase can be noted when heater is enabled
    // This is needed due to the high operating humidity of the system
    checkHeaterEnabled();
    result = {0.0, 0.0, temp_2, hum_2};
    return result;
    break;
  }
  case ProgramStates::EnabledHumiditySensors::EnabledSensors::HUM_SHT31_BOTH:
  {
    float temp_1 = sht31.readTemperature();
    float temp_2 = sht31_2.readTemperature();
    if (!isnan(temp_1))
    { // check if 'is not a number'
      log_i("Temp 1 *C = %.3f\t\t", temp_1);
    }
    else
    {
      log_i("Failed to read temperature");
    }

    if (!isnan(temp_2))
    { // check if 'is not a number'
      log_i("Temp 2 *C = %.3f", temp_2);
    }
    else
    {
      log_i("Failed to read humidity");
    }

    // check if 'is not a number'
    float hum_1 = sht31.readHumidity();
    float hum_2 = sht31_2.readHumidity();
    if (!isnan(hum_1))
    { // check if 'is not a number'
      log_i("Temp 1 *C = %.3f\t\t", hum_1);
    }
    else
    {
      log_i("Failed to read temperature");
    }

    if (!isnan(hum_2))
    { // check if 'is not a number'
      log_i("Temp 2 *C = %.3f", hum_2);
    }
    else
    {
      log_i("Failed to read humidity");
    }
    my_delay(0.1L); // delay in between reads for stability
    result = {temp_1, hum_1, temp_2, hum_2};
    return result;
    break;
  }
  default: // Should never get here
    result = {0.0, 0.0, 0.0, 0.0};
    return result;
    break;
  }
}

#if USE_SFM3003
void Humidity::sfm3003Setup()
{
  uint16_t error;
  char errorMessage[256];

  /**
   * select the proper i2c address for your sensor
   * see datasheet of your sensor
   *
   */
  sfmSf06.begin(Wire, ADDR_SFM3003_300_CE);

  error = sfmSf06.stopContinuousMeasurement();

  if (error)
  {
    errorToString(error, errorMessage, 256);
    log_i("Error trying to execute stopContinuousMeasurement(): %s", errorMessage);
  }

  uint32_t productIdentifier;
  uint8_t serialNumber[8];
  uint8_t serialNumberSize = 8;

  error = sfmSf06.readProductIdentifier(productIdentifier, serialNumber, serialNumberSize);

  if (error)
  {
    errorToString(error, errorMessage, 256);
    log_i("Error trying to execute readProductIdentifier(): %s", errorMessage);
  }
  else
  {
    log_i("Product Identifer: %d", productIdentifier);
    log_i("Serial Number:");
    log_i("0x");
    for (size_t i = 0; i < serialNumberSize; i++)
    {
      uint8_t value = serialNumber[i];
      log_i("%d", value < 16 ? "0" : "");
      log_i("%d", value, HEX);
    }
  }

  sfmSf06.startO2ContinuousMeasurement();
  log_i("flow\ttemperature");
}

Humidity::Hum Humidity::sfm3003Loop()
{
  uint16_t error;
  char errorMessage[256];
  float flow = 0;
  float temperature = 0;
  uint16_t _status = 0;

  // Read Measurement
  error = sfmSf06.readMeasurementData(flow, temperature, _status);
  my_delay(1L);

  if (error)
  {
    result.flow = 0;
    result.temperature = 0;
    log_e("Error trying to execute readMeasurementData(): %s", errorMessage);
    return result;
  }

  log_i("%.3f\t%.3f", flow, temperature);
  result.flow = flow;
  result.temperature = temperature;
  return result;
}
#endif // USE_SFM3003

/******************************************************************************
 * Function: Control MPX2010DP - K014308 and AD623
 * Description: This function allows for the control of the MPX2010DP - K014308 and AD623
 * Parameters: None
 * Return: None
 * ADD IN CODE TO READ PRESSURE SENSORS
 ******************************************************************************/

/* Put pressure sensor code here */

Humidity humidity;
