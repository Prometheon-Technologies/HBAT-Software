#include "Humidity.hpp"

// Global Variables
//  Setup an array of _relays to control peripherals. Numbers represent pin numbers.

int status = 0;
int loopCnt = 0;

Adafruit_SHT31 sht31;
Adafruit_SHT31 sht31_2;

#if USE_SFM3003
SensirionI2CSfmSf06 sfmSf06;
#endif // USE_SFM3003

bool enableHeater = false;

int _offset = 32000; // _Offset for the sensor
int HUMIDITY_SENSORS_ACTIVE = 0;

float _scale = 140.0; // _Scale factor for Air and N2 is 140.0, O2 is 142.8

Humidity::Hum result;

Humidity::Humidity()
{
  // Constructor
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
int Humidity::setupSensor()
{
  log_i("SHT31 Sensors Setup Beginning....");
  // Set to 0x45 for alternate i2c address
  if (!sht31.begin(0x44) && !sht31_2.begin(0x45))
  {
    log_i("Couldn't find SHT31 sensors");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring or the addresses and try again");
    HUMIDITY_SENSORS_ACTIVE = 0;
    return 0;
  }
  else if (!sht31.begin(0x44) && sht31_2.begin(0x45))
  {
    log_i("Couldn't find SHT31 sensor #1");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring and try again");
    HUMIDITY_SENSORS_ACTIVE = 1;
    return 1;
  }
  else if (!sht31_2.begin(0x45) && sht31.begin(0x44))
  {
    log_i("Couldn't find SHT31 sensor #2");
    log_i("SHT31 Sensors Setup did not complete successfully, check your wiring and try again");
    HUMIDITY_SENSORS_ACTIVE = 2;
    return 2;
  }
  else
  {
    log_i("SHT31 Sensors Setup Complete");
    HUMIDITY_SENSORS_ACTIVE = 3;
    return 3;
  }
  my_delay(2L); // delay in between reads for stability
}

bool Humidity::checkHeaterEnabled()
{
  switch (HUMIDITY_SENSORS_ACTIVE)
  {
  case 0:
    log_d("No humidity sensors active");
    return false;
    break;
  case 1:
  {
    bool _sensor1 = sht31.isHeaterEnabled();
    bool heaterenabled = false;
    if (loopCnt >= 30)
    {
      enableHeater = !enableHeater;
      sht31.heater(enableHeater);
      log_i("Heater Enabled State: %s", enableHeater ? "ENABLED" : "DISABLED");
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
      loopCnt = 0;
    }
    loopCnt++;
    return heaterenabled;
    break;
  }
  case 2:
  {
    bool _sensor2 = sht31_2.isHeaterEnabled();
    bool heaterenabled = false;
    if (loopCnt >= 30)
    {
      enableHeater = !enableHeater;
      sht31_2.heater(enableHeater);
      log_i("Heater Enabled State: %s", enableHeater ? "ENABLED" : "DISABLED");
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
      loopCnt = 0;
    }
    loopCnt++;
    return heaterenabled;
    break;
  }
  case 3:
  {
    bool _sensor1 = sht31.isHeaterEnabled();
    bool _sensor2 = sht31_2.isHeaterEnabled();
    bool heaterenabled = false;
    if (loopCnt >= 30)
    {
      enableHeater = !enableHeater;
      sht31.heater(enableHeater);
      sht31_2.heater(enableHeater);
      log_i("Heater Enabled State: %s\t\t", enableHeater ? "ENABLED" : "DISABLED");
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
      loopCnt = 0;
    }
    loopCnt++;
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
  switch (HUMIDITY_SENSORS_ACTIVE)
  {
  case 0:
  {
    result = {0, 0, 0, 0};
    float stack_humidity = 0;
    return stack_humidity; // return 0 if no sensors are active
    break;
  }

  case 1:
  {
    float stack_temp = result.temp;
    return stack_temp; // Only one sensor - return the value of that sensor
    break;
  }

  case 2:
  {
    float stack_temp = result.temp_2;
    return stack_temp; // Only one sensor - return the value of that sensor
    break;
  }

  case 3:
  {
    float stack_temp = result.temp + result.temp_2;
    return stack_temp / 2; // Read the _temperature from the sensor and average the two sensors.
    break;
  }

  default:
  {
    break;
  }
  }
  return 0;
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
  switch (HUMIDITY_SENSORS_ACTIVE)
  {
  case 0:
  {
    result = {0, 0, 0, 0};
    float stack_humidity = 0;
    return stack_humidity; // return 0 if no sensors are active
    break;
  }

  case 1:
  {
    float stack_humidity = result.humidity;
    return stack_humidity; // Only one sensor - return the value of that sensor
    break;
  }

  case 2:
  {
    float stack_humidity = result.humidity_2;
    return stack_humidity; // Only one sensor - return the value of that sensor
    break;
  }

  case 3:
  {
    float stack_humidity = result.humidity + result.humidity_2;
    return stack_humidity / 2; // Read the _humidity from the sensor and average the two sensors.
    break;
  }

  default:
  {
    break;
  }
  }
  return 0;
}

/******************************************************************************
 * Function: Read Humidity Sensors
 * Description: This function is used to read the humidity of the stack sensors
 * Parameters: None
 * Return: float array
 ******************************************************************************/
Humidity::Hum Humidity::ReadSensor()
{
  switch (HUMIDITY_SENSORS_ACTIVE)
  {
  case 0:
  {
    result = {0, 0, 0, 0};
    log_d("No Humidity Sensors Active");
    return result;
    break;
  }
  case 1:
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
    result = {temp, hum, 0, 0};
    return result;
    break;
  }
  case 2:
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
    result = {0, 0, temp_2, hum_2};
    return result;
    break;
  }
  case 3:
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
    result = {0, 0, 0, 0};
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
  uint16_t status = 0;

  // Read Measurement
  error = sfmSf06.readMeasurementData(flow, temperature, status);
  my_delay(1L);
  if (!error)
  {
    log_i("%.3f\t%.3f", flow, temperature);
    result.flow = flow;
    result.temperature = temperature;
    return result;
  }
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




/* uint8_t Humidity::crc8(const uint8_t result, uint8_t crc)
{
  crc ^= result;

  for (uint8_t i = 8; i; --i)
  {
    crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
  }
  return crc;
}

int Humidity::loopSFM3003()
{
  auto device = 0x28;
  unsigned long timed_event = 500;
  unsigned long current_time = millis(); // millis() function
  unsigned long start_time = current_time;
  // delay(500); // blocking delay, not needed
  if (current_time - start_time >= timed_event)
  {
    Wire.beginTransmission(byte(device)); // transmit to device (0x28)
    Wire.write(byte(0x10));               //
    Wire.write(byte(0x00));               //
    Wire.endTransmission();
    Wire.requestFrom(device, 3); // read 3 bytes from device with address 0x28
    while (Wire.available())
    {                            // slave may send less than requested
      uint16_t a = Wire.read();  // first received byte stored here. The variable "uint16_t" can hold 2 bytes, this will be relevant later
      uint8_t b = Wire.read();   // second received byte stored here
      uint8_t crc = Wire.read(); // crc value stored here
      uint8_t mycrc = 0xFF;      // initialize crc variable
      mycrc = crc8(a, mycrc);    // let first byte through CRC calculation
      mycrc = crc8(b, mycrc);    // and the second byte too
      if (mycrc != crc)
      { // check if the calculated and the received CRC byte matches
        log_i("Error: wrong CRC");
      }
      log_i("p %d %d %d h", a, b, crc);
      a = (a << 8) | b; // combine the two received bytes to a 16bit integer value
      // a >>= 2; // remove the two least significant bits
      int Flow = (a - _offset) / _scale;
      // log_i(a); // print the raw result from the sensor to the serial interface
      log_i("%d", Flow); // print the calculated flow to the serial interface
      start_time = current_time;
      return Flow;
    }
  }
  return 0;
} */

/* int Humidity::SetupSFM3003()
{
  const char *driver_version = sfm_common_get_driver_version();
  if (driver_version)
  {
    printf("SFM driver version %s\n", driver_version);
  }
  else
  {
    printf("fatal: Getting driver version failed\n");
    return -1;
  }

  sensirion_i2c_init();

  int16_t error = sensirion_i2c_general_call_reset();
  if (error)
  {
    printf("General call reset failed\n");
  }

  sensirion_sleep_usec(SFM3003_SOFT_RESET_TIME_US);

  while (sfm3003_probe())
  {
    printf("SFM sensor probing failed\n");
  }

  uint32_t product_number = 0;
  uint8_t serial_number[8] = {};
  error = sfm_common_read_product_identifier(SFM3003_I2C_ADDRESS,
                                             &product_number, &serial_number);
  if (error)
  {
    printf("Failed to read product identifier\n");
  }
  else
  {
    printf("product: 0x%08x, serial: 0x", product_number);
    for (size_t i = 0; i < 8; ++i)
    {
      printf("%x", serial_number[i]);
    }
    printf("\n");
  }
  return 0;
}

int Humidity::SFM3003()
{
  SfmConfig sfm3003 = sfm3003_create();
  int16_t error = sensirion_i2c_general_call_reset();
  error = sfm_common_start_continuous_measurement(
      &sfm3003, SFM3003_CMD_START_CONTINUOUS_MEASUREMENT_AIR);
  if (error)
  {
    printf("Failed to start measurement\n");
  }

  sensirion_sleep_usec(SFM3003_MEASUREMENT_INITIALIZATION_TIME_US);

  for (;;)
  {
    int16_t _flow_raw;
    int16_t _temperature_raw;
    error = sfm_common_read_measurement_raw(&sfm3003, &_flow_raw,
                                            &_temperature_raw, &status);
    if (error)
    {
      printf("Error while reading measurement\n");
    }
    else
    {
      // Convert the raw values to physical values in Standard Liter/Minute
      error = sfm_common_convert__flow_float(&sfm3003, _flow_raw, &_flow);
      if (error)
      {
        printf("Error while converting _flow\n");
      }
      _temperature = sfm_common_convert__temperature_float(_temperature_raw);
      printf(" _Flow: %.3f (%4i) _Temperature: %.2f (%4i) Status: %04x\n",
             _flow, _flow_raw, _temperature, _temperature_raw, status);
    }
  }
  sensirion_i2c_release();
  return 0;
} */

/* void Humidity::setupSfm3003()
{

  int a = 0;
  int b = 0;
  int c = 0;

  Wire.requestFrom(0x28, 3); //
   a = Wire.read(); // first received byte stored here
   b = Wire.read(); // second received byte stored here
   c = Wire.read(); // third received byte stored here
   Wire.endTransmission();
   delay(5);

   Wire.requestFrom(0x28, 3); //
   a = Wire.read(); // first received byte stored here
   b = Wire.read(); // second received byte stored here
   c = Wire.read(); // third received byte stored here
   Wire.endTransmission();
   delay(5);
} */

Humidity humidity;
