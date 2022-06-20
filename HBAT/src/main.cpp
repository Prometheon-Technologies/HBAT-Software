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

    Serial.println(F("Setting up the program, standby..."));
    // Setup the main loop
    // Initialize the relay pins
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

    if (Wire.begin())
    {
        Serial.println(F("I2C initialized"));
    }
    else
    {
        Serial.println(F("I2C failed"));
    }

    Serial.println(F("HMS booting - please wait"));
    Serial.println(F("Starting..."));
    HMSmain.setupSensor();
    Cell_Temp.SetupSensors();

    humidity.sfm3003Setup();
    switch (humidity.setupSensor())
    {
    case 0:
        Serial.println(F("Humidity Sensor Setup Failed - No sensors present"));
        break;
    case 1:
        Serial.println(F("Humidity Sensor Setup Failed - initialised sensor one"));
        break;
    case 2:
        Serial.println(F("Humidity Sensor Setup Failed - initialised sensor two"));
        break;
    case 3:
        Serial.println(F("Humidity Sensor Setup Successful"));
        break;
    default:
        Serial.println(F("Humidity Sensor Setup Failed - Unknown Error"));
        break;
    }

    Serial.println("");
    // Setup the network stack
    // Setup the Wifi Manager
    network.SetupWebServer();
    Serial.println(F("Starting Webserver"));
    network.SetupServer();
    Serial.println(F("Setting up WiFi"));

#if ENABLE_MQTT_SUPPORT
    Serial.println(F("Setting up MQTT"));
    HMSmqtt.loadMQTTConfig();
#endif // ENABLE_MQTT_SUPPORT

#if ENABLE_MDNS_SUPPORT
    Mdns.SetupmDNSServer(); // setup the mDNS server for the future web-front-end
#if ENABLE_MQTT_SUPPORT

    Mdns.DiscovermDNSBroker(); // discover the mDNS broker for mqtt
#endif                         // ENABLE_MQTT_SUPPORT
#endif                         // ENABLE_MDNS_SUPPORT

#if ENABLE_MQTT_SUPPORT
    HMSmqtt.MQTTSetup();
#endif // ENABLE_MQTT_SUPPORT

    Serial.println("");
    if (network.SetupNetworkStack())
    {
        Serial.println(F("Network Stack Setup Successful"));
        Serial.println(F("INFO: HTTP web server started"));
    }
    else
    {
        Serial.println(F("Network Stack Setup Failed - Activating Access-Point Mode"));
    }

    Serial.print(F("\n===================================\n"));
    Serial.println(F("Setup Complete"));
    my_delay(1L);
    // network.LoopWifiScan();
}

void loop()
{
    timedTasks.ScanI2CBus();
    timedTasks.accumulateSensorData();
    timedTasks.checkNetwork();
    timedTasks.updateCurrentData();

    if (cfg.config.data_json)
    {
        cfg.config.data_json = false;
        if (accumulatedata.SendData())
        {
            log_d("Data Sent");
        }
        else
        {
            log_d("Data Not Sent");
        }
    }

#if ENABLE_MQTT_SUPPORT
    if (WiFi.status() == WL_CONNECTED)
    {
        HMSmqtt.RunMqttService();
    }
#endif // ENABLE_MQTT_SUPPORT

#if ENABLE_MDNS_SUPPORT
    Mdns.mDNSLoop();
#endif // ENABLE_MDNS_SUPPORT

    my_delay(1L);
}