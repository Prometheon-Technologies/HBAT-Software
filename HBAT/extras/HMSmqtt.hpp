/*
 HMSMqtt.h - HBAT MQTT library
 Copyright (c) 2021 ZanzyTHEbar
 */
#pragma once
#ifndef HMSMQTT_HPP
#define HMSMQTT_HPP
#include <PubSubClient.h>
#include <defines.hpp>

extern PubSubClient mqttClient;
class HMSMqtt
{
public:
  // Constructor
  HMSMqtt();
  virtual ~HMSMqtt();
  
  // Initialize the library
  void loadMQTTConfig();
  bool MQTTSetup();
  bool MQTTLoop();
  void RunMqttService();
  void MessageReceived(char topic[], char payload[]);
  void MQTTPublish(char topic[], char payload[]);
  bool ReConnect();

private:
};

extern HMSMqtt HMSmqtt;
#endif