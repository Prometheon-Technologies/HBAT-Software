/*
 HMSmqtt.h - HBAT MQTT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMSMQTT_h
#define HMSMQTT_h
#include <defines.h>

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