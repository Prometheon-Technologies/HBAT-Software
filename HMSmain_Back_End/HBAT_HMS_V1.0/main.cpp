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

    SERIAL_DEBUG_LN("Setting up the program, standby...");
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
    SERIAL_DEBUG_LN("Starting...");
    Cell_Temp.SetupSensors();
    switch (humidity.setupSensor())
    {
    case 0:
        SERIAL_DEBUG_LN("Humidity Sensor Setup Failed - No sensors present");
        break;
    case 1:
        SERIAL_DEBUG_LN("Humidity Sensor Setup Failed - initialised sensor one");
        break;
    case 2:
        SERIAL_DEBUG_LN("Humidity Sensor Setup Failed - initialised sensor two");
        break;
    case 3:
        SERIAL_DEBUG_LN("Humidity Sensor Setup Successful");
        break;
    default:
        SERIAL_DEBUG_LN("Humidity Sensor Setup Failed - Unknown Error");
        break;
    }

    SERIAL_DEBUG_LN("");
    Relay.SetupPID();
    // Setup the network stack
    // Setup the Wifi Manager
    SERIAL_DEBUG_LN("Setting up WiFi");
    if (ENABLE_MQTT_SUPPORT)
    {
        SERIAL_DEBUG_LN("Setting up MQTT");
        network.loadMQTTConfig();
    }

    network.SetupWebServer();
    SERIAL_DEBUG_LN(F("Starting Webserver"));
    network.SetupServer();

    if (ENABLE_MQTT_SUPPORT)
    {
        HMSmqtt.MQTTSetup();
    }
    if (ENABLE_MDNS_SUPPORT)
    {
        network.SetupmDNSServer();    // setup the mDNS server for the future web-front-end
        network.SetupWebServer();     // Setup the server
        network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt
    }

    SERIAL_DEBUG_LN("");
    if (network.SetupNetworkStack())
    {
        SERIAL_DEBUG_LN("Network Stack Setup Successful");
        SERIAL_DEBUG_LN("INFO: HTTP web server started");
    }
    else
    {
        SERIAL_DEBUG_LN("Network Stack Setup Failed - Activating Access Point Mode");
    }
    SERIAL_DEBUG_LN("\n===================================");
    SERIAL_DEBUG_LN("Setup Complete");
    delay(100);
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