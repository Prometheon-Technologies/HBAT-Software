#include <FrontEnd.h>
#include <timeObj.h>
#include <i2cscan.h>

Scanner scanner;
timeObj ReadTimer(5000);
AccumulateData StackData;
FrontEnd Front_End;

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

void setup()
{
    Front_End.SetupServer();
    StackData.SetupMainLoop();
    Hum.SetupPID();
}

void ScanI2CBus()
{
    scanner.SetupScan();
    scanner.BeginScan();
}

void loop()
{
    Front_End.ClientLoop();

    if (ReadTimer.ding())
    {
        for (;;)
        {
            StackData.AccumulateDataMainLoop();
        }
        ReadTimer.start();
    }

    /* // HumRelayOnOff(5000);
    for (;;)
    {
        // Read the data from the stack
        StackData.ReadData();

        // Check if the data is valid
        if (StackData.CheckData())
        {
            // Check if the data is a humidity reading
            if (StackData.CheckHumidity())
            {
                // Humidity Related Relay Control
                HumRelayOnOff(5000);
            }
        }
    } */
}