#include <HMSmqtt.h>

HMSmqtt::HMSmqtt()
{
  MQTTSetup();
}

HMSmqtt::~HMSmqtt(void)
{
}

int HMSmqtt::CheckWifiState()
{
  int wifiStateCounter = 0;
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". ");
    delay(1000);
    wifiStateCounter++;
    if (wifiStateCounter > 100)
      SERIAL_DEBUG_LN("WiFi not connected");
    return 0;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nconnected!");
    return 1;
  }
  return 0;
}

int HMSmqtt::MQTTConnect()
{
  // float *cell_voltage = (float *)malloc(sizeof(float) * 10);
  const char *clientPass = (char *)malloc(sizeof(char));
  const char *clientUser = (char *)malloc(sizeof(char));
  clientPass = Front_End.NewMQTTPass.c_str();
  clientUser = Front_End.NewMQTTUser.c_str();
  CheckWifiState();
  if (CheckWifiState() == 1)
  {
    String inputString = MQTT_DEVICE_NAME;
    char *clientID = new char[inputString.length() + 1];
    strcpy(clientID, inputString.c_str());
    SERIAL_DEBUG_LN("connecting to MQTT Broker...");
    while (!mqtt.connect(clientID, clientUser, clientPass))
    {
      Serial.print(". ");
      delay(1000);
    }
    SERIAL_DEBUG_LN("connected!");
    return 1;
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
  if (!mqtt.connected())
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
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  //
  // If you're using the same broker for both secure and insecure connections,
  // you can use the same port number.
  //
  // If you're using a different port for secure and insecure connections,
  // you need to use different port numbers.
  //
  // For example, if you're using a secure connection for MQTT and a
  // non-secure connection for websockets, you would use the following
  // broker addresses:
  //
  //   MQTT broker:   "mqtt.mybroker.com"
  //   Websocket:     "ws://mqtt.mybroker.com:1883"
  //
  // If you're using a secure connection for MQTT and a secure connection for
  // websockets, you would use the following broker addresses:
  //
  //   MQTT broker:   "mqtt.mybroker.com"
  //   Websocket:     "wss://mqtt.mybroker.com:443"
  //
  // If you're using a secure connection for MQTT and a non-secure connection
  // for websockets, you would use the following broker addresses:
  //
  //   MQTT broker:   "mqtt.mybroker.com"
  //   Websocket:     "ws://mqtt.mybroker.com:8883"
  //

  auto getPort = [](bool secure)
  {
    return secure ? MQTT_PORT_SECURE : MQTT_PORT;
  };
  auto getHost = [](bool secure)
  {
    return secure ? net : espClient;
  };
  int port = getPort(cfg.MQTTSecure);
  auto host = getHost(cfg.MQTTSecure);
  MQTTConnect();
  mqtt.begin(WiFi.localIP(), port, host); // use net for secure connections or espClient for non-secure connections
}

void HMSmqtt::MQTTPublish(String topic, String payload)
{
  if (MQTTLoop() == 1)
  {
    mqtt.publish(topic, payload);
  }
  else
  {
    SERIAL_DEBUG_LN("MQTT not connected, not publishing. Attempting to reconnect...");
    if (Serial.available() > 0)
    {
      SERIAL_DEBUG_LN("MQTT not connected, not publishing. Attempting to reconnect...");
    }
    MQTTSetup();
    ReConnect();
    mqtt.publish(topic, payload);
    MQTTLoop();
  }
}

int HMSmqtt::MQTTLoop()
{
  mqtt.loop();
  if (!ReConnect() == 0)
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
#ifdef ENABLE_MQTT_SUPPORT
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
        mqttConnected = mqtt.connect(cfg.MQTTDeviceName);
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

  if (loop_counter % 10 == 0)
  {
    if (!mqtt.connected() && cfg.MQTTEnabled != 0)
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
        SERIAL_DEBUG_ADD("connected\n")

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
        JSONencoder["dev"]["mf"] = "Surrbradl08",
        JSONencoder["dev"]["mdl"] = VERSION,
        JSONencoder["dev"]["name"] = cfg.MQTTDeviceName,
        JSONencoder["stat_t"] = "~",
        JSONencoder["cmd_t"] = mqttSetTopicS,
        JSONencoder["brightness"] = true,
        JSONencoder["rgb"] = true,
        JSONencoder["effect"] = true,
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
          SERIAL_DEBUG_LN("Error sending Configuration")
        }
      }
      else
      {
        SERIAL_DEBUG_ADDF("failed with state %s\n", mqttClient.state())
      }
    }
  }

  if (loop_counter % 90 == 0)
  {
    MqttData.mqttSendStatus();
  }
#endif
}

//############################## MQTT HELPER FUNCTIONS ##############################

static bool mqttProcessing = false;
void HMSmqtt::mqttCallback(char *topic, byte *payload, unsigned int length)
{
  mqttProcessing = true;
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

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
  JSONencoder["brightness"] = brightness,
  JSONencoder["effect"] = patterns[currentPatternIndex].name,
  JSONencoder["autoplay"] = autoplay,
  JSONencoder["speed"] = speed;
  JSONencoder["hue"] = getHueMapped((uint8_t)0, (uint8_t)255);
  JSONencoder["saturation"] = getSatMapped((uint8_t)0, (uint8_t)255);

  uint8_t JSONmessage[128];
  size_t n = serializeJson(JSONencoder, JSONmessage);
  if (!mqttProcessing)
  {
    mqttClient.publish(cfg.MQTTTopic, JSONmessage, n, true);
    SERIAL_DEBUG_LNF("Sending MQTT package: %s", JSONencoder.as<String>().c_str())
  }
}

//############################## MQTT HELPER FUNCTIONS END ##############################