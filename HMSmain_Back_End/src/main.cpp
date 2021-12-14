#include <FrontEnd.h>
#include <timeObj.h>
#include <i2cscan.h>

TaskHandle_t runserver;
TaskHandle_t accumulatedata;
Scanner scanner;
timeObj ReadTimer(5000);
AccumulateData StackData;
HMSmqtt Mqtt_Data = HMSmqtt();
FrontEnd FrontEnd_Data = FrontEnd();

int period = 500;
unsigned long time_now = 0;
bool Charge_State;

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

void TasktoRunNetworkStack(void *parameter)
{
    Serial.print("Webserver running on core ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        FrontEnd_Data.ClientLoop();
        Mqtt_Data.MQTTLoop();
    }
}

void TasktoAccumulateSensorData(void *pvParameters)
{
    Serial.print("Data Accumulation running on core ");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        if (ReadTimer.ding())
        {
            StackData.AccumulateDataMainLoop();
            Hum.SFM3003();
            ReadTimer.start();
        }
    }
}

void setup()
{
    FrontEnd_Data.SetupServer();
    StackData.SetupMainLoop();
    // Hum.SetupPID();
    time_now = millis();

    /* Cores where the task should run */
    xTaskCreatePinnedToCore(
        TasktoRunNetworkStack,  /* Function to implement the task */
        "Task1",    /* Name of the task */
        10000,      /* Stack size in words */
        NULL,       /* Task input parameter */
        2,          /* Priority of the task */
        &runserver, /* Task handle. */
        0);

    if (millis() < time_now + period)
    {
        // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
        xTaskCreatePinnedToCore(
            TasktoAccumulateSensorData,       /* Task function. */
            "Task2",         /* name of task. */
            10000,           /* Stack size of task */
            NULL,            /* parameter of the task */
            1,               /* priority of the task */
            &accumulatedata, /* Task handle to keep track of created task */
            1);              /* pin task to core 1 */
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