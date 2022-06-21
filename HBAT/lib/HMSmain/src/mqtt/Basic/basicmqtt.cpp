#include "basicmqtt.hpp"

IPAddress broker_ip;

void callback(char *topic, byte *payload, unsigned int length);
char *getBroker();

#if MQTT_SECURE
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_SECURE_PORT, callback, espClient); // Local Secure Mosquitto Connection
#else
PubSubClient mqttClient(broker_ip.fromString(getBroker()), MQTT_PORT, callback, espClient); // Local Mosquitto Connection
#endif // MQTT_SECURE

//***********************************************************************************************************************
// * MQTT Class
// * @brief Base Class for handling MQTT - without support for HASS
//************************************************************************************************************************

BASEMQTT::BASEMQTT() : _interval(60000),
                       _user_data{0},
                       _previousMillis(0),
                       _user_bytes_received(0),
                       _infoTopic("hbathms/info"),
                       _statusTopic("hbathms/status"),
                       _commandTopic("hbathms/command"),
                       _configTopic("hbathms/config"),
                       _relayTopics{{"relay1"}, {"relay2"}, {"relay3"}, {"relay4"}, {"relay5"}},
                       _mqttControlTopic("hbathms/mqtt/control") {}

BASEMQTT::~BASEMQTT() {}

char *getBroker()
{
#pragma message(Feature "mDNS Enabled: " STR(ENABLE_MDNS_SUPPORT))
#if ENABLE_MDNS_SUPPORT
    if (mDNSManager::startMDNS())
    {
        if (mDNSManager::DiscovermDNSBroker())
        {
            Serial.println(F("[mDNS responder started] Setting up Broker..."));
            char *BROKER_ADDR = cfg.config.MQTTBroker;
            return BROKER_ADDR;
        }
        else
        {
            Serial.println(F("[mDNS responder failed]"));
            char *BROKER_ADDR = MQTT_BROKER;
            return BROKER_ADDR;
        }

        return String(MQTT_BROKER);
    }
#else
    return MQTT_BROKER;
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
        }
    }

    if (strcmp(topic, basemqtt._mqttControlTopic) == 0)
    {
        if (strcmp(result.c_str(), "OFF") == 0)
        {
            log_i("Turning off MQTT");
            cfg.config.MQTTEnabled = false;
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
    log_i("MQTT client state is: %d", mqttClient.state());
    for (auto relay : _relayTopics)
    {
        mqttClient.subscribe(relay);
        log_i("Subscribing to the topic [%s]", relay);
    }

    mqttClient.subscribe(_mqttControlTopic);
    log_i("Subscribing to the topic [%s]", _mqttControlTopic);

    log_i("Successfully subscribed to all topics.");

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
            // Subscribe to topics
            for (auto relay : _relayTopics)
            {
                mqttClient.subscribe(relay);
                log_i("Subscribing to the topic [%s]", relay);
            }

            mqttClient.subscribe(_mqttControlTopic);
            log_i("Subscribing to the topic [%s]", _mqttControlTopic);
        }
        else
        {
            log_i("failed, rc= %d", mqttClient.state());
            log_i("trying again in 5 seconds");
            // Wait 5 seconds before retrying
            my_delay(5L);
        }
    }
}

void BASEMQTT::mqttLoop()
{
    if (cfg.config.MQTTEnabled)
    {
        my_delay(1L);

        if (!mqttClient.connected())
        {
            mqttReconnect();
        }
        else
        {
            mqttClient.loop();
            //callback;

            unsigned long currentMillis = millis();
            if (currentMillis - _previousMillis >= _interval)
            {
                _previousMillis = currentMillis;
                cfg.config.data_json = true;
                my_delay(1L);
                String temp = cfg.config.data_json_string;
                mqttClient.publish(_infoTopic, temp.c_str(), true);
                temp = "";
                log_i("Published to topic [%s]", _infoTopic);

                ProgramStates::BatteryChargeState::ChargeState chargeState = accumulatedata.ChargeStatus();
                mqttClient.publish(_statusTopic, String(chargeState).c_str(), true);
            }
        }
    }
}

BASEMQTT basemqtt;
