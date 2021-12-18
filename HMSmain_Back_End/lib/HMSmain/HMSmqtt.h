/*
 HMSmqtt.h - HBAT MQTT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMSMQTT_h
#define HMSMQTT_h
#include <Arduino.h>
/* #include <WiFi.h> */
#include <MQTT.h>
#include <WiFiClientSecure.h>
#include <FrontEnd.h>
#include <AccumulateData.h>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtoul */
#include <mqttconfig.h> /* MQTT data Struct */

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web interface "settings" page
#define MQTT_ENABLED 0
#define MQTT_HOSTNAME "homeassistant.local"
#define MQTT_PORT 1883
#define MQTT_USER "MyUserName"
#define MQTT_PASS ""
#define MQTT_TOPIC_SET "/set" // MQTT Topic to subscribe to for changes(Home Assistant)
#if LED_DEVICE_TYPE == 0
#define MQTT_HOMEASSISTANT_TOPIC "homeassistant/hms/data" // MQTT Topic to Publish to for state and config (Home Assistant)
#define MQTT_TOPIC "hms/data"                             // MQTT Topic to Publish to for state and config (Any MQTT Broker)
#endif
#define MQTT_UNIQUE_IDENTIFIER WiFi.macAddress() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024

#include <PubSubClient.h> // Include the MQTT Library, must be installed via the library manager
#include <ArduinoJson.h>
WiFiClient espClient;
PubSubClient mqttClient(espClient);
#endif
/*###################### MQTT Configuration END ######################*/

class HMSmqtt
{
public:
  // Constructor
  HMSmqtt();
  virtual ~HMSmqtt(void);
  // Initialize the library
  MQTTClient mqtt;
  WiFiClientSecure net;
  void MQTTSetup();
  int MQTTLoop();
  void MQTTPublish(String topic, String payload);
  int CheckWifiState();
  void MQTTConnect();
  int ReConnect();

private:
};
#endif