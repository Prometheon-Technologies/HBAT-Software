#include "Humidity.hpp"

// Global Variables
//  Setup an array of _relays to control peripherals. Numbers represent pin numbers.

bool enableHeater = false;
int WindowSize = 5000;
unsigned long windowStartTime;
uint16_t status;
uint8_t loopCnt = 0;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Specify the links and initial tuning parameters

Adafruit_SHT31 sht31;
Adafruit_SHT31 sht31_2;

Humidity::Humidity(void)
{
  flow = returnData[0];
  temperature = returnData[1];
  sht31 = Adafruit_SHT31();
  sht31_2 = Adafruit_SHT31();
  _relays[0] = 45;
  _relays[1] = 38;
  _relays[2] = 36;
  _relays[3] = 35;
  _relays[4] = 48;
  _offset = 32000; // _Offset for the sensor
  _scale = 140.0;  // _Scale factor for Air and N2 is 140.0, O2 is 142.8
}

Humidity::~Humidity(void)
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
  SERIAL_DEBUG_LN("SHT31 Sensors Setup Beginning");
  // Set to 0x45 for alternate i2c address
  if (!sht31.begin(0x44) ^ !sht31_2.begin(0x45))
  {
    SERIAL_DEBUG_LN("Couldn't find SHT31 sensors");
    while (1)
      delay(1);
  }
  SERIAL_DEBUG_LN("SHT31 Sensors Setup Complete");
  delay(1); // delay in between reads for stability

  if (_sensor1 ^ _sensor2)
  {
    SERIAL_DEBUG_LN("Sensors have Heater ENABLED");
  }
  else
  {
    SERIAL_DEBUG_LN("Sensor 1 Heater Disabled");
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
  return (stack_temp[0] + stack_temp[2]) / 2; // Read the _temperature from the sensor and average the two sensors.
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
  float *climatedata = (float *)malloc(sizeof(float) * 4);

  // check if 'is not a number
  if (!isnan(climatedata[0] and climatedata[1]))
  {
    climatedata[0] = sht31.readTemperature();
    climatedata[1] = sht31_2.readTemperature();
    SERIAL_DEBUG_LN("Sensor 1 Temp *C = ");
    Serial.print(climatedata[0]);
    SERIAL_DEBUG_LN("Sensor 2 Temp *C = ");
    Serial.print(climatedata[1]);
  }
  else
  {
    SERIAL_DEBUG_LN("Failed to read _temperature");
  }

  // check if 'is not a number'
  if (!isnan(climatedata[2] and climatedata[3]))
  {
    climatedata[2] = sht31.readHumidity();
    climatedata[3] = sht31_2.readHumidity();
    SERIAL_DEBUG_LN("Sensor 1 Humidity %% = ");
    Serial.print(climatedata[2]);
    SERIAL_DEBUG_LN("Sensor 2 Humidity %% = ");
    Serial.print(climatedata[3]);
  }
  else
  {
    SERIAL_DEBUG_LN("Failed to read humidity");
  }

  delay(1000);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC _temperature increase can be noted when heater is enabled
  // This is needed due to the high operating humidity of the system
  if (++loopCnt == 30)
  {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    sht31_2.heater(enableHeater);
    Serial.print("Heater Enabled State: ");

    if (_sensor1 ^ _sensor2)
    {
      Serial.print("Sensors have Heater ENABLED");
    }
    else
    {
      SERIAL_DEBUG_LN("Sensor 1 Heater Disabled");
    }

    loopCnt = 0;
  }
  return climatedata;
}

/******************************************************************************
 * Function: Control MPX2010DP - K014308 and AD623
 * Description: This function allows for the control of the MPX2010DP - K014308 and AD623
 * Parameters: None
 * Return: None
 ******************************************************************************/
// FIXME: ADD IN CODE TO READ PRESSURE SENSORS
/******************************************************************************
 * Function: Setup _relays
 * Description: Loop through and set all _relays to output and off state
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Humidity::SetupRelays()
{
  // initialize the Relay pins and set them to off state
  for (int i = 0; i <= 5; i++)
  {
    pinMode(_relays[i], OUTPUT);
    digitalWrite(_relays[i], LOW);
  }
}

/******************************************************************************
 * Function: Setup PID Controller
 * Description: This function sets up the PID controller
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Humidity::SetupPID()
{
  // Initialize the relay pins
  SetupRelays();

  windowStartTime = millis();

  // initialize the variables we're linked to
  Setpoint = 80;

  // tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  // turn the PID on
  myPID.SetMode(AUTOMATIC);
}

/******************************************************************************
 * Function: Humidity Related Relay Control
 * Description: Initialise a PID controller to control a relay based on humidity sensor readings
 * Parameters: None
 * Return: None
 * SFM3003 Mass Air _Flow Sensor code to be integrated
 * Below PID Relay code is an example of how to use the PID controller
 * This code should only be used during the Charging phase. Integrate State Machine to use this code
 ******************************************************************************/
void Humidity::HumRelayOnOff()
{
  float climate_data = StackHumidity();
  Input = climate_data;
  myPID.Compute();

  // turn the output pin on/off based on pid output
  unsigned long now = millis();
  if (now - windowStartTime > WindowSize)
  { // time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output > now - windowStartTime)
    digitalWrite(_relays[0], HIGH);
  else
    digitalWrite(_relays[0], LOW);
}

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

void Humidity::setupSfm3003()
{
  Wire.begin();
  int a = 0;
  int b = 0;
  int c = 0;

  /*Wire.requestFrom(0x40, 3); //
   a = Wire.read(); // first received byte stored here
   b = Wire.read(); // second received byte stored here
   c = Wire.read(); // third received byte stored here
   Wire.endTransmission();
   delay(5);

   Wire.requestFrom(0x40, 3); //
   a = Wire.read(); // first received byte stored here
   b = Wire.read(); // second received byte stored here
   c = Wire.read(); // third received byte stored here
   Wire.endTransmission();
   delay(5);*/
}

uint8_t Humidity::crc8(const uint8_t data, uint8_t crc)
{
  crc ^= data;

  for (uint8_t i = 8; i; --i)
  {
    crc = (crc & 0x80)
              ? (crc << 1) ^ 0x31
              : (crc << 1);
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
        SERIAL_DEBUG_LN("Error: wrong CRC");
      }
      SERIAL_DEBUG_LN('p');
      SERIAL_DEBUG_LN(a);
      SERIAL_DEBUG_LN(b);
      SERIAL_DEBUG_LN(crc);
      SERIAL_DEBUG_LN('h');
      a = (a << 8) | b; // combine the two received bytes to a 16bit integer value
      // a >>= 2; // remove the two least significant bits
      int Flow = (a - _offset) / _scale;
      // SERIAL_DEBUG_LN(a); // print the raw data from the sensor to the serial interface
      SERIAL_DEBUG_LN(Flow); // print the calculated _flow to the serial interface
      start_time = current_time;
      returnData[0] = Flow;
    }
  }
  return flow;
}

Humidity Hum;
