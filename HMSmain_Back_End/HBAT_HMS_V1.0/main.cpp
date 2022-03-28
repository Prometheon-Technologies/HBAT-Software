#include <defines.hpp>

/******************************************************************************
 * Function: Setup Main Loop
 * Description: This is the setup function for the main loop of the whole program. Use this to setup the main loop.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Setting up the program, standby...");
    // Setup the main loop

    timedTasks.SetupTimers();
    // Initialize the relay pins
    /* int temp[5] = {45, 38, 36, 35, 48};
    // initialize the Relay pins and set them to off state
    std::copy(temp, temp + sizeof(temp) / sizeof(temp[0]), cfg.config.relays_pin);

    config.relays_pin[0] = 45;
    config.relays_pin[1] = 38;
    config.relays_pin[2] = 36;
    config.relays_pin[3] = 35;
    config.relays_pin[4] = 48;

    for (int i = 0; i < sizeof(cfg.config.relays_pin); i++)
    {
        pinMode(cfg.config.relays_pin[i], OUTPUT);
        digitalWrite(cfg.config.relays_pin[i], LOW);
    }
    Hum.setupSfm3003();
    Hum.SetupSensor();
    HMSmain.setupSensor();
    Cell_Temp.SetupSensors();
    Hum.SetupPID();
    SERIAL_DEBUG_LN("HMS booting - please wait");
    Serial.println("Version: " + String(VERSION) + " " + String(__DATE__) + " " + String(__TIME__));
    Serial.println("");
    cfg.CreateDefaultConfig();
    Serial.println("Starting...");
    // Setup the network stack
    // Setup the Wifi Manager
    Serial.println("Reading Config File");
    Serial.println("Config File Read");
    Serial.println("Setting up WiFi");
    SERIAL_DEBUG_LN(F("Starting Webserver"))
    //network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
    network.SetupServer();
    // network.SetupWebServer();// Setup the server
    network.loadConfig();
    //network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt
    HMSmqtt.MQTTSetup();
    SERIAL_DEBUG_LN("INFO: HTTP web server started");
    SERIAL_DEBUG_LN("\n===================================");
    Front_End.SetupServer();
    SERIAL_DEBUG_LN("Setup Complete");
    delay(100); */
}

void ScanI2CBus()
{
    Scan.SetupScan();
    Scan.BeginScan();
}

void loop()
{
    /* timedTasks.Run_Check_DataJSON_5();
    timedTasks.Run_NetworkCheck_Background_every_10_Seconds();
    delay(100);
    if (WiFi.status() == WL_CONNECTED)
    {
        network.SetupmDNSLoop();
        HMSmqtt.MQTTLoop();
        // Front_End.Loop();
        HMSmqtt.RunMqttService();
    } */
}
