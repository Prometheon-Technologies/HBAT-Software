#include <defines.hpp>

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
    SERIAL_DEBUG_ADD("Webserver running on core ");
    SERIAL_DEBUG_LN(xPortGetCoreID());
    network.HMSnetworkSetup();
    if (network.CheckWifiState() == 1)
    {
        network.SetupmDNSServer();
        network.DiscovermDNSBroker();
        SERIAL_DEBUG_LN("Webserver running on core ");
        SERIAL_DEBUG_LN(xPortGetCoreID());
        network.HMSnetworkSetup();
        for (;;) // Front_End.mqttFrontEndCondition == true)
        {
            network.ChecknetworkLoop();
            network.SetupmDNSLoop();
            vTaskDelay(100);
            MqttData.MQTTLoop();
        }
    }
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
