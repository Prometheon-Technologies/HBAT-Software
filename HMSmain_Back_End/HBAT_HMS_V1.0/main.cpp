#include <Arduino.h>
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
    // Initialize the relay pins
    int temp[5] = {45, 38, 36, 35, 48};
    // initialize the Relay pins and set them to off state
    std::copy(temp, temp + sizeof(temp) / sizeof(temp[0]), cfg.config.relays_pin);

    for (int i = 0; i < sizeof(cfg.config.relays_pin); i++)
    {
        pinMode(cfg.config.relays_pin[i], OUTPUT);
        digitalWrite(cfg.config.relays_pin[i], LOW);
    }
    // Wire.begin();
    HMSmain.setupSensor();

    SERIAL_DEBUG_LN("HMS booting - please wait");
    Serial.println("Version: " + String(VERSION) + " ");
    Serial.println("");
    Serial.println("Date: " + String(__DATE__) + " " + String(__TIME__));
    Serial.println("");
    Serial.println("Starting...");
    Cell_Temp.SetupSensors();
    switch (humidity.setupSensor())
    {
    case 0:
        Serial.println("Humidity Sensor Setup Failed - No sensors present");
        break;
    case 1:
        Serial.println("Humidity Sensor Setup Failed - initialised sensor one");
        break;
    case 2:
        Serial.println("Humidity Sensor Setup Failed - initialised sensor two");
        break;
    case 3:
        Serial.println("Humidity Sensor Setup Successful");
        break;
    default:
        Serial.println("Humidity Sensor Setup Failed - Unknown Error");
        break;
    }

    Serial.println("Creating Config File");
    /*
    Relays.SetupPID();
    cfg.CreateDefaultConfig();
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

/* void ScanI2CBus()
{
    Scan.SetupScan();
    Scan.BeginScan();
}
 */
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
