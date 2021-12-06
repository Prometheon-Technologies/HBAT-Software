#include <FrontEnd.h>
#include <timeObj.h>
#include <i2cscan.h>

TaskHandle_t server;
TaskHandle_t accumulatedata;
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
    // Hum.SetupPID();
    xTaskCreatePinnedToCore(
        Task1code, /* Function to implement the task */
        "Task1",   /* Name of the task */
        10000,     /* Stack size in words */
        NULL,      /* Task input parameter */
        0,         /* Priority of the task */
        &server,   /* Task handle. */
        0);        /* Core where the task should run */

    delay(500);

    // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
        Task2code,       /* Task function. */
        "Task2",         /* name of task. */
        10000,           /* Stack size of task */
        NULL,            /* parameter of the task */
        1,               /* priority of the task */
        &accumulatedata, /* Task handle to keep track of created task */
        1);              /* pin task to core 1 */
    delay(500);
}

void Task1code(void *parameter)
{
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        Front_End.ClientLoop();
    }
}

void Task2code(void *pvParameters)
{
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        if (ReadTimer.ding())
        {
            StackData.AccumulateDataMainLoop();
            ReadTimer.start();
        }
    }
}

void ScanI2CBus()
{
    scanner.SetupScan();
    scanner.BeginScan();
}

void loop()
{
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