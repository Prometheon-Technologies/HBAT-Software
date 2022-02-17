#include <defines.hpp>

void setup()
{
    cfg.CreateDefaultConfig();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    timedTasks.SetupTimers();
    Hum.SetupSensor();
    HMSmain.setupSensor();
    Cell_Temp.SetupSensors();
    Hum.SetupPID();
    SERIAL_DEBUG_LN("HMS booting - please wait");
    Serial.println("Version: " + String(VERSION) + " " + String(VERSION_DATE));
    Serial.println("");
    Serial.println("Starting...");
    // Setup the network stack
    // Setup the Wifi Manager
    Serial.println("Reading Config File");
    Serial.println("Config File Read");
    Serial.println("Setting up WiFi");
    SERIAL_DEBUG_LN(F("Starting Webserver"))
    network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
    network.SetupServer();
    // network.SetupWebServer();// Setup the server
    network.loadConfig();
    network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt
    HMSmqtt.MQTTSetup();
    SERIAL_DEBUG_LN("INFO: HTTP web server started");
    SERIAL_DEBUG_LN("\n===================================");
    Front_End.SetupServer();
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
    timedTasks.Run_Check_DataJSON_5();
    timedTasks.Run_NetworkCheck_Background_every_10_Seconds();
    delay(100);
    if (WiFi.status() == WL_CONNECTED)
    {
        network.SetupmDNSLoop();
        HMSmqtt.MQTTLoop();
        //Front_End.Loop();
        HMSmqtt.RunMqttService();
    }
}
