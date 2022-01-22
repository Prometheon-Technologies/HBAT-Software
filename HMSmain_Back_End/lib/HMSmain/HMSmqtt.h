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
  HMSmqtt(void);
  virtual ~HMSmqtt(void);
  // Initialize the library
  //void mqttSendStatus();
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  void MQTTSetup();
  int MQTTLoop();
  void RunMqttService();
  void MessageReceived(char topic[], char payload[]);
  void MQTTPublish(char topic[], char payload[]);
  int CheckWifiState();
  int MQTTConnect();
  int ReConnect();
  //void callback(char *topic, byte *message, unsigned int length);

private:
};
#endif