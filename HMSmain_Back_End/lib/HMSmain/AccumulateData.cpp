#include <AccumulateData.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)
#define debugCalibrateAmps() HMSmain.calibrateAmps()

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugCalibrateAmps()
#endif

#define LED1 37
#define LED2 47

//Variables
double Setpoint, Input, Output;
int WindowSize = 5000;
unsigned long windowStartTime;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Specify the links and initial tuning parameters

HMS HMSmain = HMS();
Humidity Hum = Humidity();
CellTemp Cell_Temp = CellTemp();

AccumulateData::AccumulateData()
{
}

/******************************************************************************
 * Function: Main Sensor Data Structure
 * Description: This is the main Data Structure where all sensor data is accumulated. To be passed into the main program.
 * Parameters: None
 * Return: None
 ******************************************************************************/
struct AccumulateData::data_arrays
{
    float stack_humidity;
    float stack_temp;
    float stack_voltage;
    float cell_temp[10];
    float cell_voltage[10];
};

/******************************************************************************
 * Function: Setup relays
 * Description: Loop through and set all relays to output and off state
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::setup_relays()
{
  // initialize the Relay pins and set them to off state
  for (int i = 0; i < 5; i++)
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
void AccumulateData::SetupPID()
{
  // Initialize the relay pins
  setup_relays();

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
 * Parameters: int time, float pointer to stack humidity
 * Return: None
 * SFM3003 Mass Air Flow Sensor code to be integrated
 * Below PID Relay code is an example of how to use the PID controller
 * This code should only be used durign the Charging phase. Integrate State Machine to use this code
 ******************************************************************************/
void AccumulateData::HumRelayOnOff(int time, float *stack_humidity)
{
  float climate_data = Hum.StackHumidity();
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

/* void led2OnOff(int time)
{
  digitalWrite(LED2, HIGH);
  delay(time);
  digitalWrite(LED2, LOW);
  delay(time);
} */

/* void ledFade(int LED)
{
  int freq = 1000;
  int ledChannel = 0;
  int resolution = 16;
  int dutyCycle = 0;
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED, ledChannel);
  dutyCycle = map(3, 0, 4095, 0, 32767);
  ledcWrite(ledChannel, dutyCycle);
  // increase the LED brightness
  for (int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for (int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }
} */

/* void stack_climate()
{
  float *climatedata = Hum.ReadSensor();
  char climateData[100];
  sprintf(climateData, "%3d, %3d", "%3d, %3d", climatedata[0], climatedata[1], climatedata[2], climatedata[3]);
  debugln(climateData);

  String voltageaverage = "";
  float *readvoltage = HMSmain.readSensAndCondition();
  for (int i = 0; i < sizeof(readvoltage); i++)
  {
    char temp[100];
    sprintf(temp, "%s, %3f", voltageaverage, readvoltage[i]);
    voltageaverage = temp;
  }
} */

/******************************************************************************
 * Function: Setup Main Loop
 * Description: This is the setup function for the main loop of the whole program. Use this to setup the main loop.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::SetupMainLoop()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10); // will pause until serial console opens

    // debug("freeMemory()="+freeMemory());

    debugln();
    delay(1000);

    debugln("\n===================================");
    Hum.SetupSensor();
    HMSmain.setupSensor();
    debugf("HMS booting - please wait");
    debugf("Setup Complete");
    delay(100);
}

/******************************************************************************
 * Function: Accumulate Data from sensor arrays
 * Description: This function accumualtes all sensor data and stores it in the main data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::AccumulateDataMainLoop(data_arrays &data)
{
    // Stack level data
    data.stack_humidity = Hum.StackHumidity();
    debugln(data.stack_humidity);
    data.stack_temp = Hum.AverageStackTemp();
    debugln(data.stack_temp);

    // Cell level data
    
    float *cell_temp = Cell_Temp.read_temp_sensor_data();

    for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
    {
        data.cell_temp[i] = cell_temp[i];
        debugln(data.cell_temp[i]);
    }

    float *cell_voltage = HMSmain.readSensAndCondition();

    for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
    {
        data.cell_voltage[i] = cell_voltage[i];
        debugln(data.cell_voltage[i]);
    }

    data.stack_voltage = 0;
    for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
    {
        data.stack_voltage += cell_voltage[i];
        debugln(data.cell_temp[i]);
    }
    delete[] cell_temp, cell_voltage;
    /* free (cell_temp);
    free (cell_voltage); */
}

/******************************************************************************
 * Function: Debug Print Data
 * Description: This function prints all string data to the serial console.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::debugdata(String str)
{
    Serial.print(str);
}
