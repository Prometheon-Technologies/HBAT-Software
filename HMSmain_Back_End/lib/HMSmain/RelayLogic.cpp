#include <AccumulateData.h>
#include <RelayLogic.h>

HMS HMSmain = HMS();
Humidity Hum = Humidity();
CellTemp Cell_Temp = CellTemp();

// Variables
double Setpoint, Input, Output;
int WindowSize = 5000;
unsigned long windowStartTime;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Specify the links and initial tuning parameters

RelayLogic::RelayLogic()
{
}

/******************************************************************************
 * Function: Setup relays
 * Description: Loop through and set all relays to output and off state
 * Parameters: None
 * Return: None
 ******************************************************************************/
void RelayLogic::SetupRelays()
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
void RelayLogic::SetupPID()
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
 * Parameters: int time, float pointer to stack humidity
 * Return: None
 * SFM3003 Mass Air Flow Sensor code to be integrated
 * Below PID Relay code is an example of how to use the PID controller
 * This code should only be used durign the Charging phase. Integrate State Machine to use this code
 ******************************************************************************/
void RelayLogic::HumRelayOnOff(int time, float *stack_humidity)
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
