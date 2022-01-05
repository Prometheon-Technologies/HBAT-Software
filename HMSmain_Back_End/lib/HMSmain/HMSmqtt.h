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
  void mqttSendStatus();
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  void MQTTSetup();
  int MQTTLoop();
  void MessageReceived(String &topic, String &payload);
  void MQTTPublish(String topic, String payload);
  int CheckWifiState();
  int MQTTConnect();
  int ReConnect();

private:
};
#endif