#include <FrontEnd.h>
#include <PID_v1.h>
#include <timeObj.h>
#define MAXNUMOFRELAYS 5

timeObj ReadTimer(5000);
AccumulateData StackData;
FrontEnd Front_End;

// Variables
// Setup an array of relays to control peripherals. Numbers represent pin numbers.
const int relays[MAXNUMOFRELAYS] = {45, 38, 36, 35, 48};
int received;
double Setpoint, Input, Output;
int WindowSize = 5000;
unsigned long windowStartTime;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Specify the links and initial tuning parameters

// if (input_voltage < 0.50 && input_voltage >= 0.00 )
//{
// digitalWrite(2, HIGH);
// delay (30);
// digitalWrite(2, LOW);
// delay (30);
// }
// else if (input_voltage < 1.00 && input_voltage >= 0.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 1.50 && input_voltage >= 1.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 2.00 && input_voltage >= 1.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 2.50 && input_voltage >= 2.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 3.00 && input_voltage >= 2.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 3.50 && input_voltage >= 3.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 4.00 && input_voltage >= 3.50)
//{
//  DO STUFF
// }
// else if (input_voltage < 4.50 && input_voltage >= 4.00)
//{
//  DO STUFF
// }
// else if (input_voltage < 5.00 && input_voltage >= 4.50)
//{
//  DO STUFF
// }

/******************************************************************************
 * Function: Debug Print Data
 * Description: This function prints all string data to the serial console.
 * Parameters: None
 * Return: None
 ******************************************************************************/
/* void debugdata(String value)
{
    debugln(value);
} */

/******************************************************************************
 * Function: Setup relays
 * Description: Loop through and set all relays to output and off state
 * Parameters: None
 * Return: None
 ******************************************************************************/
void SetupRelays()
{
    // initialize the Relay pins and set them to off state
    for (int i = 0; i < MAXNUMOFRELAYS; i++)
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
void SetupPID()
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
void HumRelayOnOff(int time)
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

void setup()
{
    StackData.SetupMainLoop();
    Front_End.SetupServer();
    SetupPID();
}

void loop()
{
    Front_End.ClientLoop();

    if (ReadTimer.ding())
    {
        StackData.AccumulateDataMainLoop();
        ReadTimer.start(); 
    }
}