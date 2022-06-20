/*
 basemqtt.hpp - HBAT HMS MQTT library
 Copyright (c) 2022 ZanzyTHEbar
 */
#pragma once
#ifndef BASEMQTT_HPP
#define BASEMQTT_HPP
#include <defines.hpp>
#include <PubSubClient.h>

class BASEMQTT : public IPAddress
{
public:
    // Constructor
    BASEMQTT();
    virtual ~BASEMQTT();

    bool begin();
    void loadMQTTConfig();
    void mqttLoop();
    void mqttReconnect();

    // Friends
    friend void callback(char *topic, byte *payload, unsigned int length);

private:
    // Private functions

    // Private variables
    const long _interval;
    unsigned long _previousMillis;
    uint8_t _user_bytes_received;
    char _user_data[100];

    char *_infoTopic;
    char *_statusTopic;
    char *_commandTopic;
    char *_configTopic;
    char _relayTopics[5][20];
    char *_mqttControlTopic;
};

extern BASEMQTT basemqtt;
#endif // BASEMQTT_HPP