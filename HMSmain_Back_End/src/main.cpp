#include <defines.h>

/******************************************************************************
 * Function: Call back functions for Task Scheduler
 * Description: These functions are called by the Task Scheduler
 * Parameters: None
 * Return: None
 * THIS IS EXAMPLE CODE AND SHOULD BE MODIFIED TO FIT THE NEEDS
 *
 * https://github.com/arkhipenko/TaskScheduler/blob/master/examples/Scheduler_example01/Scheduler_example01.ino
 ******************************************************************************/
// Dual Core Task Pinning Functions
void TasktoRunNetworkStack(void *parameter) // TODO: Implement Network Stack Task
{
    SERIAL_DEBUG_ADD("Webserver running on core ");
    SERIAL_DEBUG_LN(xPortGetCoreID());
    Network.HMSNetworkSetup();
    if (Network.CheckWifiState() == 1)
    {
        Network.SetupmDNSServer();
        Network.DiscovermDNSBroker();
        SERIAL_DEBUG_LN("Webserver running on core ");
        SERIAL_DEBUG_LN(xPortGetCoreID());
        Network.HMSNetworkSetup();
        for (;;) // Front_End.mqttFrontEndCondition == true)
        {
            Network.CheckNetworkLoop();
            Network.SetupmDNSLoop();
            vTaskDelay(100);
            MqttData.MQTTLoop();
        }
    }
    else
    {
        SERIAL_DEBUG_LN("Webserver failed to start on core ");
        SERIAL_DEBUG_LN(xPortGetCoreID());
    }
}

void TasktoAccumulateSensorData(void *pvParameters)
{
    SERIAL_DEBUG_ADD("Data Accumulation running on core ");
    SERIAL_DEBUG_LN(xPortGetCoreID());
    for (;;)
    {
        if (ReadTimer.ding())
        {
            accumulatedData.InitAccumulateDataJson();
            Hum.SFM3003();
            ReadTimer.start();
        }
    }
}

void setup()
{
    SERIAL_DEBUG_LN("INFO: HTTP web server started");

    SERIAL_DEBUG_LN("\n===================================");
    Hum.SetupSensor();
    HMSmain.setupSensor();
    Cell_Temp.SetupSensors();
    
    SERIAL_DEBUG_LN("HMS booting - please wait");
    Front_End.SetupServer();
    accumulatedData.SetupMainLoop();
    // Hum.SetupPID();
    time_now = millis();

    /* Cores where the task should run */
    xTaskCreatePinnedToCore(
        TasktoRunNetworkStack, /* Function to implement the task */
        "Task1",               /* Name of the task */
        10000,                 /* Stack size in words */
        NULL,                  /* Task input parameter */
        2,                     /* Priority of the task */
        &runserver,            /* Task handle. */
        0);

    if (millis() < time_now + period)
    {
        // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
        xTaskCreatePinnedToCore(
            TasktoAccumulateSensorData, /* Task function. */
            "Task2",                    /* name of task. */
            10000,                      /* Stack size of task */
            NULL,                       /* parameter of the task */
            1,                          /* priority of the task */
            &accumulatedata,            /* Task handle to keep track of created task */
            1);                         /* pin task to core 1 */
    }
    SERIAL_DEBUG_LN("Setup Complete");
    delay(100);
}

void ScanI2CBus()
{
    scanner.SetupScan();
    scanner.BeginScan();
}

void loop()
{
}
