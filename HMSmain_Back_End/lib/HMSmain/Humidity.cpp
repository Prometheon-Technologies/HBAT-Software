#include <Humidity.hpp>

//Global Variables
// Setup an array of relays to control peripherals. Numbers represent pin numbers.

bool enableHeater = false;
int WindowSize = 5000;
unsigned long windowStartTime;
uint16_t status;
uint8_t loopCnt = 0;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Specify the links and initial tuning parameters

Humidity::Humidity(void)
{
  flow = returnData[0];
  temperature = returnData[1];
  sht31 = Adafruit_SHT31();
  sht31_2 = Adafruit_SHT31();
  relays[0] = 45;
  relays[1] = 38;
  relays[2] = 36;
  relays[3] = 35;
  relays[4] = 48;
  //relays[] = {45, 38, 36, 35, 48};
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
  return (stack_temp[0] + stack_temp[2]) / 2; // Read the temperature from the sensor and average the two sensors.
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
 * Description: This function allows for the control of the MPX2010DP - K014308 and AD623
 * Parameters: None
 * Return: None
 ******************************************************************************/

/******************************************************************************
 * Function: Setup relays
 * Description: Loop through and set all relays to output and off state
 * Parameters: None
 * Return: None
 ******************************************************************************/
void Humidity::SetupRelays()
{
  // initialize the Relay pins and set them to off state
  for (int i = 0; i <= 5; i++)
  {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], LOW);
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
 * SFM3003 Mass Air Flow Sensor code to be integrated
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
    digitalWrite(relays[0], HIGH);
  else
    digitalWrite(relays[0], LOW);
}

int Humidity::SetupSFM3003()
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

  /* Initialize I2C bus */
  sensirion_i2c_init();

  /* Reset all I2C devices */
  int16_t error = sensirion_i2c_general_call_reset();
  if (error)
  {
    printf("General call reset failed\n");
  }
  /* Wait for the SFM3003 to initialize */
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

  /* Wait for the first measurement to be available. Wait for
   * SFM3003_MEASUREMENT_WARM_UP_TIME_US instead for more reliable results */
  sensirion_sleep_usec(SFM3003_MEASUREMENT_INITIALIZATION_TIME_US);

  for (;;)
  {
    int16_t flow_raw;
    int16_t temperature_raw;
    error = sfm_common_read_measurement_raw(&sfm3003, &flow_raw,
                                            &temperature_raw, &status);
    if (error)
    {
      printf("Error while reading measurement\n");
    }
    else
    {
      // Convert the raw values to physical values in Standard Liter/Minute
      error = sfm_common_convert_flow_float(&sfm3003, flow_raw, &flow);
      if (error)
      {
        printf("Error while converting flow\n");
      }
      temperature = sfm_common_convert_temperature_float(temperature_raw);
      printf(" Flow: %.3f (%4i) Temperature: %.2f (%4i) Status: %04x\n",
             flow, flow_raw, temperature, temperature_raw, status);
    }
  }
  sensirion_i2c_release();
  return 0;
}
