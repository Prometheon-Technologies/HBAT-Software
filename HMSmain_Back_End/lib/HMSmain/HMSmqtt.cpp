#include <HMSmqtt.h>

HMSmqtt::HMSmqtt()
{
  MQTTSetup();
}

HMSmqtt::~HMSmqtt(void)
{
}

int HMSmqtt::MQTTConnect()
{

  int wifi_status = Network.CheckWifiState();
  if (wifi_status == 1)
  {
    if (cfg.MQTTSecureState != 0)
    {
      SERIAL_DEBUG_LN("connecting to MQTT Broker...");
      while (!)
      {
        Serial.print(". ");
        delay(1000);
      }
      SERIAL_DEBUG_LN("connected using a secure connection!");
      return 1;
    }
    else
    {
      while (!)
      {
        Serial.print(". ");
        delay(1000);
      }
      SERIAL_DEBUG_LN("connected using an insecure connection!");
      return 1;
    }
  }
  else
  {
    SERIAL_DEBUG_LN("MQTT connection failed!");
    return 0;
  }
}

int HMSmqtt::ReConnect()
{
  // reconnect failover
  if (!mqttClient.connected())
  {
    MQTTConnect();
    return 0;
  }
  return 1;
}

void HMSmqtt::MessageReceived(String &topic, String &payload)
{
  SERIAL_DEBUG_LN("incoming: " + topic + " - " + payload);

  // Note: Do not use the mqtt in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `mqtt.loop()`.
}

void HMSmqtt::MQTTSetup()
{
  // MQTT client
  if (Network.CheckWifiState() != 0)
  {
    auto getPort = [](bool secure)
    {
      return secure ? MQTT_PORT_SECURE : MQTT_PORT;
    };
    auto port = getPort(cfg.MQTTSecureState);
    char host = cfg.MQTTServer;
    mqttClient.setServer(host, port);
    mqttClient.setCallback(callback);
  }
  else
  {
    SERIAL_DEBUG_LN("MQTT setup failed! Please connect your device to a WiFi network.");
  }
}

void HMSmqtt::MQTTPublish(String topic, String payload)
{
  if (MQTTLoop() == 1)
  {
    mqttClient.publish(topic, payload);
  }
  else
  {
    if (Serial.available() > 0)
    {
      SERIAL_DEBUG_LN("MQTT not connected, not publishing. Attempting to reconnect...");
    }
    mqttClient.publish(topic, payload);
    MQTTLoop();
  }
}

int HMSmqtt::MQTTLoop()
{
  mqttClient.loop();
  if (ReConnect() != 1)
  {
    // reconnect if connection dropped
    ReConnect();
    return 0;
  }
  else
  {
    return 1;
    delay(10); // <- fixes some issues with WiFi stability
  }
}

void HMSmqtt::RunMqttService()
{
  static unsigned int loop_counter = 0;
  static bool mqttConnected = false;

  if (cfg.MQTTEnabled == 1)
    MQTTLoop();
  if (loop_counter % 10 == 0)
  {
    if (cfg.MQTTEnabled == 1)
    {
      if (!mqttConnected)
      {
        SERIAL_DEBUG_LN("Trying to connect to MQTT")
        mqttConnected = mqttClient.connect(cfg.MQTTDeviceName);
      }
      if (mqttConnected)
      {
        SERIAL_DEBUG_LN("MQTT Connected!")

        MQTTPublish(cfg.MQTTTopic, "ON");
      }
      else
      {
        SERIAL_DEBUG_LN("MQTT Not Connected!")
      }
    }
  }
  else
    mqttConnected = false;
  // create a time object to execute the below code every 10 seconds

  if (ReadTimer_10.ding())
  {
    if (!mqttClient.connected() && cfg.MQTTEnabled != 0)
    {
      mqttConnected = false;
    }
    if (!mqttConnected && cfg.MQTTEnabled != 0)
    {
      mqttConnected = true;
      SERIAL_DEBUG_BOL
      SERIAL_DEBUG_ADD("Connecting to MQTT...");
      if (MQTTLoop())
      {
        SERIAL_DEBUG_ADD("connected\n");

        SERIAL_DEBUG_LN("Subscribing to MQTT Topics");
        char mqttSetTopicC[129];
        strlcpy(mqttSetTopicC, cfg.MQTTTopic, sizeof(mqttSetTopicC));
        strlcat(mqttSetTopicC, cfg.MQTTSetTopic, sizeof(mqttSetTopicC));
        mqttClient.subscribe(mqttSetTopicC);

        char mqttSetTopicS[66];
        strcpy(mqttSetTopicS, "~");
        strlcat(mqttSetTopicS, cfg.MQTTSetTopic, sizeof(mqttSetTopicS));

        DynamicJsonDocument JSONencoder(4096);
        JSONencoder["~"] = cfg.MQTTTopic,
        JSONencoder["name"] = cfg.MQTTDeviceName,
        JSONencoder["dev"]["ids"] = MQTT_UNIQUE_IDENTIFIER,
        JSONencoder["dev"]["mf"] = "DaOfficialWizard",
        JSONencoder["dev"]["mdl"] = VERSION,
        JSONencoder["dev"]["name"] = cfg.MQTTDeviceName,
        JSONencoder["stat_t"] = "~",
        JSONencoder["cmd_t"] = mqttSetTopicS,
        JSONencoder[""] = true,
        JSONencoder[""] = true,
        JSONencoder[""] = true,
        JSONencoder["uniq_id"] = MQTT_UNIQUE_IDENTIFIER,
        JSONencoder["schema"] = "json";

        JsonArray effect_list = JSONencoder.createNestedArray("effect_list");
        for (uint8_t i = 0; i < patternCount; i++)
        {
          effect_list.add(patterns[i].name);
        }
        size_t n = measureJson(JSONencoder);
        char mqttConfigTopic[85];
        strlcat(mqttConfigTopic, cfg.MQTTTopic, sizeof(mqttConfigTopic));
        strcat(mqttConfigTopic, "/config");
        if (mqttClient.beginPublish(mqttConfigTopic, n, true) == true)
        {
          SERIAL_DEBUG_LN("Configuration Publishing Begun")
          if (serializeJson(JSONencoder, mqttClient) == n)
          {
            SERIAL_DEBUG_LN("Configuration Sent")
          }
          if (mqttClient.endPublish() == true)
          {
            SERIAL_DEBUG_LN("Configuration Publishing Finished")
            MqttData.mqttSendStatus();
            SERIAL_DEBUG_LN("Sending Initial Status")
          }
        }
        else
        {
          SERIAL_DEBUG_LN("Error sending Configuration");
        }
      }
      else
      {
        SERIAL_DEBUG_ADDF("failed with state %s\n", mqttClient.state());
      }
    }
  }

  ReadTimer_10.start();
  if (ReadTimer_90.ding())
  {
    MqttData.mqttSendStatus();
  }
  ReadTimer_90.start();
  loop_counter++;
}

//############################## MQTT HELPER FUNCTIONS ##############################

static bool mqttProcessing = false;
void HMSmqtt::mqttCallback(char *topic, byte *payload, unsigned int length)
{
  mqttProcessing = true;
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  SERIAL_DEBUG_LNF("Received MQTT package: %s", doc.as<String>().c_str());

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
    if (strcmp(key, "brightness") == 0)
    {
      uint8_t val = v.as<uint8_t>();
      // setBrightness(val);
    }
    if (strcmp(key, "autoplay") == 0)
    {
      String val = v.as<String>();
      val.toLowerCase();
      if (val == String("on"))
      {
        // setAutoplay(1);
      }
      else if (val == String("off"))
      {
        // setAutoplay(0);
      }
      else if (val == String("toggle"))
      {
        // setAutoplay((autoplay == 1) ? 0 : 1);
      }
    }
    if (strcmp(key, "speed") == 0)
    {
      int val = v.as<int>();
      // setSpeed(val);
    }
    if (strcmp(key, "hue") == 0)
    {
      uint8_t val = v.as<uint8_t>();
      ////setSolidColorHue(val, false);
    }
    if (strcmp(key, "saturation") == 0)
    {
      uint8_t val = v.as<uint8_t>();
      ////setSolidColorSat(val, false);
    }
    if (strcmp(key, "effect") == 0)
    {
      String val = v.as<String>();
      // setPatternName(val);
    }
    if (strcmp(key, "color") == 0)
    {
      uint8_t cr, cb, cg;
      JsonObject val = v.as<JsonObject>();
      for (JsonPair o : val)
      {
        const char *ckey = o.key().c_str();
        JsonVariant cv = o.value();
        if (strcmp(ckey, "r") == 0)
        {
          cr = cv.as<uint8_t>();
        }
        if (strcmp(ckey, "g") == 0)
        {
          cg = cv.as<uint8_t>();
        }
        if (strcmp(ckey, "b") == 0)
        {
          cb = cv.as<uint8_t>();
        }
      }
      // setSolidColor(cr, cg, cb, false);
    }
  }
  mqttProcessing = false;
  mqttSendStatus();
}

void HMSmqtt::mqttSendStatus()
{
  if (cfg.MQTTEnabled != 1)
    return;

  StaticJsonDocument<128> JSONencoder;
  JSONencoder["state"] = (power == 1 ? "ON" : "OFF"),
  JSONencoder["brightness"] = ,
  JSONencoder["effect"] = ,
  JSONencoder["autoplay"] = ,
  JSONencoder["speed"] = ;
  JSONencoder["hue"] = ;
  JSONencoder["saturation"] = ;

  uint8_t JSONmessage[128];
  size_t n = serializeJson(JSONencoder, JSONmessage);
  if (!mqttProcessing)
  {
    mqttClient.publish(cfg.MQTTTopic, JSONmessage, n, true);
    SERIAL_DEBUG_LNF("Sending MQTT package: %s", JSONencoder.as<String>().c_str())
  }
}
//############################## MQTT HELPER FUNCTIONS END ##############################//


