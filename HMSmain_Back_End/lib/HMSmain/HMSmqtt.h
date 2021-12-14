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
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtoul */

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

  // Variables
  int last_mqtt_connect_attempt;
  int last_mqtt_publish_attempt;
  unsigned long lastMillis;
  char clientIP;
  char client;
  unsigned long clientPass;
  unsigned long clientUser;

private:
};
#endif