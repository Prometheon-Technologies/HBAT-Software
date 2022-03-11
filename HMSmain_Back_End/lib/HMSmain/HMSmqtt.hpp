/*
 HMSMqtt.h - HBAT MQTT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef HMSMQTT_hpp
#define HMSMQTT_hpp
#include "defines.hpp"

extern PubSubClient mqttClient(espClient);
class HMSMqtt
{
public:
  // Constructor
  HMSMqtt(void);
  virtual ~HMSMqtt(void);
  // Initialize the library
  //void mqttSendStatus();
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  bool MQTTSetup();
  int MQTTLoop();
  void RunMqttService();
  void MessageReceived(char topic[], char payload[]);
  void MQTTPublish(char topic[], char payload[]);
  int CheckWifiState();
  int ReConnect();
  /* void mqttSendStatus();
  void callback(char *topic, byte *message, unsigned int length); */

private:
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HMSMQTT)
extern HMSMqtt HMSmqtt;
#endif
#endif