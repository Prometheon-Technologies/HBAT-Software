#include <defines.h>

void setup()
{
    setupTasks();
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
}

void ScanI2CBus()
{
    scanner.SetupScan();
    scanner.BeginScan();
}

void loop() {
    runner.execute();
}





