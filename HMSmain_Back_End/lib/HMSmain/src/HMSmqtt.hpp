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
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  bool MQTTSetup();
  int MQTTLoop();
  void RunMqttService();
  void MessageReceived(char topic[], char payload[]);
  void MQTTPublish(char topic[], char payload[]);
  int CheckWifiState();
  int ReConnect();

private:
};

extern HMSMqtt HMSmqtt;
#endif