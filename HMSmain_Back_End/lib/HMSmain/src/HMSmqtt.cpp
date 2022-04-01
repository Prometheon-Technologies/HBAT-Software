
#include "HMSmqtt.hpp"

PubSubClient mqttClient(espClient);

long lastReconnectAttempt = 0;

HMSMqtt::HMSMqtt()
{
}

HMSMqtt::~HMSMqtt()
{
}

//############################## MQTT HELPER FUNCTIONS ##############################
void mqttSendStatus(String doc)
{
  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  if (!mqttProcessing)
  {
    char *topic_data;
    const char *json = "/json_data";
    topic_data = appendChartoChar(cfg.config.MQTTTopic, json); // append string two to the result.
    mqttClient.publish(topic_data, buffer, n);
    SERIAL_DEBUG_LNF("Sending MQTT package: %s", doc.c_str());
    mqttProcessing = false;
    free(topic_data);
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  mqttProcessing = true;
  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print(". Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
  }
  Serial.println();

  StaticJsonDocument<1024> doc;
  deserializeJson(doc, message, length);

  SERIAL_DEBUG_LNF("Received MQTT package: %s", doc.as<String>().c_str())

  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair p : obj)
  {
    const char *key = p.key().c_str();
    JsonVariant v = p.value();

    if (strcmp(key, "state") == 0)
    {
      String val = v.as<String>();
      val.toLowerCase();
      if (val == String("on"))
      {
        // setPower(1);
      }
      else if (val == String("off"))
      {
        // setPower(0);
      }
      else if (val == String("toggle"))
      {
        // setPower((power == 1) ? 0 : 1);
      }
    }
  }
  mqttProcessing = false;
  mqttSendStatus(doc.as<String>());
}
//############################## MQTT HELPER FUNCTIONS END ############################HMS

bool HMSMqtt::ReConnect() // TODO:Call this in the Loop pinned to task for core 0
{
  // Loop until we're reconnected
  if (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(cfg.config.MQTTDeviceName))
    {
      Serial.println("connected");
      // Subscribe
      /* char *topic_sub;
      const char *led = "/LED";
      topic_sub = appendChartoChar(cfg.config.MQTTTopic, led); // append string two to the result.
      mqttClient.subscribe(topic_sub); */

      // Publish
      /* char *topic_pub;
      const char *test = "/test";
      topic_pub = appendChartoChar(cfg.config.MQTTTopic, test); // append string two to the result.
      mqttClient.publish(topic_pub, "hello world"); */
      return mqttClient.connected();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      return mqttClient.connected();
    }
  }
  return mqttClient.connected();
}

void HMSMqtt::MessageReceived(char topic[], char payload[])
{
  SERIAL_DEBUG_LN("incoming: " + String(topic) + " - " + String(payload));

  // Note: Do not use the mqtt in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `mqtt.loop()`.
}

bool HMSMqtt::MQTTSetup()
{
  // MQTT client
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
    SERIAL_DEBUG_LN("MQTT setup failed! Please connect your device to a WiFi network.");
    return false;
  }
  else
  {
    auto getPort = [](bool secure)
    {
      return secure ? MQTT_PORT_SECURE : MQTT_PORT;
    };
    auto port = getPort(cfg.config.MQTTSecureState);
    auto host = cfg.config.MQTTBroker;
    mqttClient.setServer(host, port);
    mqttClient.setCallback(callback);
    lastReconnectAttempt = 0;
    return true;
  }
}

void HMSMqtt::MQTTPublish(char topic[], char payload[])
{
  if (MQTTLoop())
  {
    unsigned int len = strlen(payload);
    bool publish = mqttClient.publish(topic, payload, len);
    if (publish != false)
    {
      SERIAL_DEBUG_LN("MQTT publish success!");
    }
    else
    {
      SERIAL_DEBUG_LN("MQTT publish failed!");
    }
  }
  else
  {
    if (Serial.available() > 0)
    {
      SERIAL_DEBUG_LN("MQTT not connected, not publishing. Attempting to reconnect...");
    }
  }
}

bool HMSMqtt::MQTTLoop()
{
  if (!mqttClient.connected())
  {
    long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (ReConnect())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // Client connected
    mqttClient.loop();
  }
  return mqttClient.connected();
}

void HMSMqtt::RunMqttService()
{
  bool mqttConnected = false;

  if (!mqttClient.connected() && cfg.config.MQTTEnabled != 0)
  {
    SERIAL_DEBUG_LN("MQTT not connected, attempting to reconnect...")
    MQTTLoop();
  }
  if (mqttClient.connected() && cfg.config.MQTTEnabled != 0)
  {
    SERIAL_DEBUG_BOL
    SERIAL_DEBUG_ADD("Connecting to MQTT...")
    cfg.config.MQTTConnectedState = mqttConnected = true;

    SERIAL_DEBUG_LN("MQTT connected!")

    if (MQTTLoop())
    {
      SERIAL_DEBUG_ADD("connected\n")

      SERIAL_DEBUG_LN("Subscribing to MQTT Topics")

      char *mqttSetTopicS = NULL;
      heapStr(&mqttSetTopicS, "~");
      char *setTopic = NULL;
      heapStr(&setTopic, appendChartoChar(mqttSetTopicS, cfg.config.MQTTTopic));
      free(mqttSetTopicS);
      free(setTopic);

      DynamicJsonDocument JSONencoder(1024);
      JSONencoder["~"] = cfg.config.MQTTTopic,
      JSONencoder["name"] = cfg.config.MQTTDeviceName,
      JSONencoder["dev"]["ids"] = cfg.config.MQTTClientID,
      JSONencoder["dev"]["mf"] = "Prometheon Technologies",
      JSONencoder["dev"]["mdl"] = VERSION,
      JSONencoder["dev"]["name"] = "HMS",
      JSONencoder["stat_t"] = "~",
      JSONencoder["cmd_t"] = setTopic,
      JSONencoder["mDNS"] = ENABLE_MDNS_SUPPORT,
      JSONencoder["I2C_Scanner"] = ENABLE_I2C_SCANNER,
      JSONencoder["MQTT"] = true,
      JSONencoder["MQTT_Secure"] = cfg.config.MQTTSecureState,
      JSONencoder["MQTT_Broker"] = cfg.config.MQTTBroker,
      JSONencoder["MQTT_Port"] = cfg.config.MQTTPort,
      JSONencoder["MQTT_User"] = cfg.config.MQTTUser,
      JSONencoder["Device"] = "HBAT",
      JSONencoder["schema"] = "json";

      size_t n = measureJson(JSONencoder);
      char *mqttConfigTopic = NULL;
      heapStr(&mqttConfigTopic, appendChartoChar(cfg.config.MQTTTopic, "/config"));
      bool publish = mqttClient.beginPublish(mqttConfigTopic, n, true);

      if (publish == true)
      {
        SERIAL_DEBUG_LN("Configuration Publishing Begun");
        if (serializeJson(JSONencoder, mqttClient) == n)
        {
          SERIAL_DEBUG_LN("Configuration Sent");
        }
        if (mqttClient.endPublish())
        {
          SERIAL_DEBUG_LN("Configuration Publishing Finished");
          mqttSendStatus(JSONencoder.as<String>());
          SERIAL_DEBUG_LN("Sending Initial Status");
        }
      }
      else
      {
        SERIAL_DEBUG_LN("Error sending Configuration");
      }
      free(mqttConfigTopic);
    }
    else
    {
      SERIAL_DEBUG_ADDF("failed with state %s\n", mqttClient.state());
    }
  }
  else
  {
    cfg.config.MQTTConnectedState = mqttConnected = false;
  }
}

HMSMqtt HMSmqtt;