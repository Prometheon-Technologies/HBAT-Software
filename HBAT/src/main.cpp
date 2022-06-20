#include <defines.hpp>

#if ENABLE_MDNS_SUPPORT
mDNSManager::MDNSHandler mdnsHandler = mDNSManager::MDNSHandler(&stateManager, &cfg);
#endif // ENABLE_MDNS_SUPPORT

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
    ledManager.setupLED();

    Serial.println(F("Setting up the program, standby..."));
    // Setup the main loop

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
#if ENABLE_MDNS_SUPPORT
    cfg.attach(&mdnsHandler);
#endif // ENABLE_MDNS_SUPPORT

    Serial.println(F("Starting Webserver"));
    network.SetupServer();
    Serial.println(F("Setting up WiFi"));

#if ENABLE_MQTT_SUPPORT
    Serial.println(F("Setting up MQTT"));
    basemqtt.loadMQTTConfig();
#endif // ENABLE_MQTT_SUPPORT

#if ENABLE_MDNS_SUPPORT
    mdnsHandler.startMDNS();
#if ENABLE_MQTT_SUPPORT
    if (mdnsHandler.DiscovermDNSBroker())
    {
        Serial.println(F("[mDNS responder started] Setting up Broker..."));
    }
    else
    {
        Serial.println(F("[mDNS responder failed]"));
    }
#endif // ENABLE_MQTT_SUPPORT                // ENABLE_MDNS_SUPPORT
#endif // ENABLE_MDNS_SUPPORT

#if ENABLE_MQTT_SUPPORT
    basemqtt.begin();
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

    ledManager.onOff(true);
    ota.SetupOTA(cfg);

    Serial.print(F("\n===================================\n"));
    Serial.println(F("Setup Complete"));
    my_delay(1L);
    // network.LoopWifiScan();
}

void loop()
{
    timedTasks.checkNetwork();
    ota.HandleOTAUpdate();
    ledManager.displayStatus();
#if ENABLE_I2C_SCANNER
    timedTasks.ScanI2CBus();
#endif // ENABLE_I2C_SCANNER
    timedTasks.accumulateSensorData();
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
    if (stateManager.getCurrentState() == ProgramStates::DeviceState::WiFiState::WiFiConnected)
    {
#if ENABLE_HASS
        hassmqtt.mqttLoop();
#else
        basemqtt.mqttLoop();
#endif // ENABLE_HASS
    }
#endif // ENABLE_MQTT_SUPPORT

#if ENABLE_MDNS_SUPPORT
    //! TODO: put mDNS loop here
#endif // ENABLE_MDNS_SUPPORT
}