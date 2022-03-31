#include "FrontEnd.hpp"

FrontEnd::FrontEnd(void)
{
}

FrontEnd::~FrontEnd(void)
{
}

String FrontEnd::json_return_data()
{
  return "";
}

// ############## functions to update current server settings ###################
void FrontEnd::loadConfig()
{
  SERIAL_DEBUG_LN(F("Loading config"));

  if (!cfg.isValidHostname(cfg.config.hostname, sizeof(cfg.config.hostname)))
  {
    strncpy(cfg.config.hostname, DEFAULT_HOSTNAME, sizeof(DEFAULT_HOSTNAME));
    cfg.setConfigChanged();
  }

#ifdef ENABLE_MQTT_SUPPORT
  // fall back to default settings if hostname is invalid
  char *mqtt_host = (char *)cfg.config.MQTTBroker;
  if (!cfg.isValidHostname(mqtt_host, sizeof(cfg.config.MQTTBroker)))
  {
    cfg.config.MQTTEnabled = MQTT_ENABLED;
    // Convert String data of global definitions into Char pointers for use in strncpy
    char *MQTT_BROKER = StringtoChar(MQTT_HOSTNAME);
    char *mqtt_user = StringtoChar(MQTT_USER);
    char *mqtt_pass = StringtoChar(MQTT_PASS);
    char *mqtt_topic = StringtoChar(MQTT_TOPIC);
    char *mqtt_topic_set = StringtoChar(MQTT_HOMEASSISTANT_TOPIC_SET);
    char *mqtt_device_name = StringtoChar(MQTT_DEVICE_NAME);

    if (MQTT_SECURE_ENABLED != 0)
    {
      heapStr(&cfg.config.MQTTPort, (char *)MQTT_PORT_SECURE);
    }
    else
    {
      heapStr(&cfg.config.MQTTPort, (char *)MQTT_PORT);
    }

    heapStr(&cfg.config.MQTTBroker, MQTT_BROKER);
    heapStr(&cfg.config.MQTTUser, mqtt_user);
    heapStr(&cfg.config.MQTTPass, mqtt_pass);
    heapStr(&cfg.config.MQTTTopic, mqtt_topic);
    heapStr(&cfg.config.MQTTSetTopic, mqtt_topic_set);
    heapStr(&cfg.config.MQTTDeviceName, mqtt_device_name);

    cfg.setConfigChanged();
  }
#endif

  SERIAL_DEBUG_LNF("Loaded config: hostname %s, MQTT enabled %s, MQTT host %s, MQTT port %d, MQTT user %s, MQTT pass %s, MQTT topic %s, MQTT set topic %s, MQTT device name %s",
                   cfg.config.hostname,
                   (cfg.config.MQTTEnabled == MQTT_ENABLED) ? "true" : "false",
                   cfg.config.MQTTBroker,
                   cfg.config.MQTTPort,
                   cfg.config.MQTTUser,
                   cfg.config.MQTTPass,
                   cfg.config.MQTTTopic,
                   cfg.config.MQTTSetTopic,
                   cfg.config.MQTTDeviceName)
}

FrontEnd Front_End;