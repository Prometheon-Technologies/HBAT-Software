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

    timedTasks.setupTimers();

    Serial.println("Setting up the program, standby...");
    // Setup the main loop
    // Initialize the relay pins
    int temp[5] = {45, 38, 36, 35, 48};
    // initialize the Relay pins and set them to off state
    std::copy(temp, temp + sizeof(temp) / sizeof(temp[0]), cfg.config.relays_pin);

    // use a c++ ranged for loop to iterate through the relay pins
    for (auto pin : cfg.config.relays_pin)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // C for loop - legacy function
    /* for (int i = 0; i < sizeof(cfg.config.relays_pin) / sizeof(cfg.config.relays_pin[0]); i++)
    {
        pinMode(cfg.config.relays_pin[i], OUTPUT);
        digitalWrite(cfg.config.relays_pin[i], LOW);
    } */

    Wire.begin();
    HMSmain.setupSensor();

    Serial.println("HMS booting - please wait");
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

    Serial.println("");
    Relay.SetupPID();
    // Setup the network stack
    // Setup the Wifi Manager
    network.SetupWebServer();
    Serial.println(F("Starting Webserver"));
    network.SetupServer();
    Serial.println("Setting up WiFi");

    if (ENABLE_MQTT_SUPPORT)
    {
        Serial.println("Setting up MQTT");
        network.loadMQTTConfig();
    }

    if (ENABLE_MDNS_SUPPORT)
    {
        network.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
        if (ENABLE_MQTT_SUPPORT)
        {
            network.DiscovermDNSBroker(); // discover the mDNS broker for mqtt
        }
    }

    if (ENABLE_MQTT_SUPPORT)
    {
        HMSmqtt.MQTTSetup();
    }

    Serial.println("");
    if (network.SetupNetworkStack())
    {
        Serial.println("Network Stack Setup Successful");
        Serial.println("INFO: HTTP web server started");
    }
    else
    {
        Serial.println("Network Stack Setup Failed - Activating Access-Point Mode");
    }

    Serial.print("\n===================================\n");
    Serial.println("Setup Complete");
    my_delay(1000L);
}

void loop()
{
    // Check for the network stack
    timedTasks.ScanI2CBus();
    timedTasks.checkNetwork();
    timedTasks.accumulateSensorData();
    timedTasks.updateCurrentData();

    if (ENABLE_MQTT_SUPPORT)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            HMSmqtt.RunMqttService();
        }
    }

    if (ENABLE_MDNS_SUPPORT)
    {
        network.mDNSLoop();
    }
    my_delay(1000L);
}