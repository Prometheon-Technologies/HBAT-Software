#include "basicmqtt.hpp"

IPAddress broker_ip;

void callback(char *topic, byte *payload, unsigned int length);
String getBroker();

#if MQTT_SECURE
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_SECURE_PORT, callback, espClient); // Local Mosquitto Connection
#else
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_PORT, callback, espClient); // Local Mosquitto Connection
#endif // MQTT_SECURE

//***********************************************************************************************************************
// * Class Global Variables
// * Please only make changes to the following class variables within the ini file. Do not change them here.
//************************************************************************************************************************

BASEMQTT::BASEMQTT() : _interval(60000),
                       _user_data{0},
                       _previousMillis(0),
                       _user_bytes_received(0),
                       _infoTopic("hbathms/info"),
                       _statusTopic("hbathms/status"),
                       _commandTopic("hbathms/command"),
                       _configTopic("hbathms/config"),
                       _menuTopic("hbathms/menu"),
                       _relayTopics{{"relay1"}, {"relay2"}, {"relay3"}, {"relay4"}, {"relay5"}}
{
    // Constructor
}

BASEMQTT::~BASEMQTT()
{
    // Destructor
}

String getBroker()
{
#pragma message(Feature "mDNS Enabled: " STR(ENABLE_MDNS_SUPPORT))
#if ENABLE_MDNS_SUPPORT
    if (mDNSManager::startMDNS())
    {
        if (mDNSManager::DiscovermDNSBroker())
        {
            Serial.println(F("[mDNS responder started] Setting up Broker..."));
            String BROKER_ADDR = cfg.config.MQTTBroker; // IP address of the MQTT broker - change to your broker IP address or enable MDNS support
            return BROKER_ADDR;
        }
        else
        {
            Serial.println(F("[mDNS responder failed]"));
            String BROKER_ADDR = MQTT_BROKER;
            return BROKER_ADDR;
        }

        return String(MQTT_BROKER);
    }
#else
    return String(MQTT_BROKER);
#endif // ENABLE_MDNS_SUPPORT
}

// Handles messages arrived on subscribed topic(s)
void callback(char *topic, byte *payload, unsigned int length)
{
    String result;
    log_i("Message arrived on topic: [%s] ", topic);
    for (int i = 0; i < length; i++)
    {
        log_i("payload is: [%s]", (char)payload[i]);
        result += (char)payload[i];
    }
    log_i("Message: [%s]", result.c_str());

    // Check if the message is for the current topic
    for (auto relay : basemqtt._relayTopics)
    { 
        if (strcmp(topic, relay) == 0)
        {
            if (strcmp(result.c_str(), "ON") == 0)
            {
                log_i("Turning on relay: [%s]", relay);
                cfg.config.relays[relay - basemqtt._relayTopics[0]] = true;
            }
            else if (strcmp(result.c_str(), "OFF") == 0)
            {
                log_i("Turning off the relay: [%s]", relay);
                cfg.config.relays[relay - basemqtt._relayTopics[0]] = false;
            }
            for (int i = 0; i < sizeof(cfg.config.relays_pin) / sizeof(cfg.config.relays_pin[0]); i++)
            {
                Relay.RelayOnOff(cfg.config.relays_pin[i], cfg.config.relays[i]);
            }
        }
    }
}

bool BASEMQTT::begin()
{
    log_i("Setting up MQTT...");

    // Local Mosquitto Connection -- Start
    if (!mqttClient.connect(DEFAULT_HOSTNAME))
    {
        // connection failed
        log_i("Connection failed. MQTT client state is: %d", mqttClient.state());
        cfg.config.MQTTConnectedState = mqttClient.state();
        return false;
    }

    cfg.config.MQTTConnectedState = mqttClient.state();
#if ENABLE_PH_SUPPORT
    // connection succeeded
    log_i("Connection succeeded. Subscribing to the topic [%s]", phsensor._pHTopic);
    mqttClient.subscribe(phsensor._pHTopic);
#endif // ENABLE_PH_SUPPORT
    log_i("Subscribing to the topic [%s]", pump._pumpTopic);
    mqttClient.subscribe(pump._pumpTopic);

    log_i("Successfully subscribed to the topic.");

    _menuTopic = "menuControl/menu";
    _infoTopic = "user/info";
    /* _speakerTopic = SPEAKER_TOPIC;
    _waterlevelTopic = WATER_LEVEL_TOPIC; */
    return true;
}

void BASEMQTT::loadMQTTConfig()
{
    log_i("Checking if hostname is set and valid");
    size_t size = sizeof(cfg.config.hostname);
    if (!cfg.isValidHostname(cfg.config.hostname, size - 1))
    {
        heapStr(&cfg.config.hostname, DEFAULT_HOSTNAME);
        cfg.setConfigChanged();
    }

    String MQTT_CLIENT_ID = generateDeviceID();
    const char *mqtt_user = MQTT_USER;
    const char *mqtt_pass = MQTT_PASS;
    char *mqtt_client_id = StringtoChar(MQTT_CLIENT_ID);
    heapStr(&cfg.config.MQTTUser, mqtt_user);
    heapStr(&cfg.config.MQTTPass, mqtt_pass);
    heapStr(&cfg.config.MQTTClientID, mqtt_client_id);
    WiFi.setHostname(cfg.config.hostname); // define hostname
    cfg.setConfigChanged();
    free(mqtt_client_id);

    log_i("Loaded config: hostname %s, MQTT enable relay %s, MQTT host %s, MQTT port %d, MQTT user %s, MQTT pass %s, MQTT topic %s, MQTT set topic %s, MQTT device name %s",
          cfg.config.hostname,
          cfg.config.MQTTBroker,
          cfg.config.MQTTPort,
          cfg.config.MQTTUser,
          cfg.config.MQTTPass,
          cfg.config.MQTTDeviceName);

    log_i("Loaded config: hostname %s", cfg.config.hostname);
}

void BASEMQTT::mqttReconnect()
{
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
        log_i("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(DEFAULT_HOSTNAME))
        {
            log_i("Connected to MQTT broker.");
            // Subscribe
#if ENABLE_PH_SUPPORT
            mqttClient.subscribe(phsensor._pHTopic);
#endif // ENABLE_PH_SUPPORT
        }
        else
        {
            log_i("failed, rc= %d", mqttClient.state());
            log_i(" try again in 5 seconds");
            // Wait 15 seconds before retrying
            my_delay(15L);
        }
    }
}

void BASEMQTT::mqttLoop()
{
    my_delay(1L);

    if (!mqttClient.connected())
    {
        mqttReconnect();
    }
    else
    {
        mqttClient.loop();
        callback;

        unsigned long currentMillis = millis();
        if (currentMillis - _previousMillis >= _interval)
        {
            _previousMillis = currentMillis;

            if (Serial.available() > 0)
            {
                _user_bytes_received = Serial.readBytesUntil(13, _user_data, sizeof(_user_data));
            }

            if (_user_bytes_received)
            {
                phsensor.parse_cmd(_user_data);
                _user_bytes_received = 0;
                memset(_user_data, 0, sizeof(_user_data));
            }
#if ENABLE_PH_SUPPORT
            log_i("Sending message to topic: %s", phsensor._pHOutTopic);
#endif // ENABLE_PH_SUPPORT
            float newpH = cfg.config.pH;
            String timeStamp = networkntp.getTimeStamp();
            log_i("pH: %s", String(newpH).c_str());
#if ENABLE_PH_SUPPORT
            mqttClient.publish(phsensor._pHOutTopic, String(newpH).c_str(), true);
            mqttClient.publish(phsensor._pHOutTopic, timeStamp.c_str(), true);
#endif // ENABLE_PH_SUPPORT
        }
    }
}

BASEMQTT basemqtt;
