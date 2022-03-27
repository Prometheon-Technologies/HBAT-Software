/*
 HMSMqtt.h - HBAT MQTT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#pragma once
#ifndef HMSMQTT_HPP
#define HMSMQTT_HPP
#include <PubSubClient.h>
#include "defines.hpp"

extern PubSubClient mqttClient;
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

extern HMSMqtt HMSmqtt;
#endif