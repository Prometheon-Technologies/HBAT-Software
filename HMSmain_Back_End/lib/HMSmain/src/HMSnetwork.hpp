/*
 Network.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
*/
#pragma once
#ifndef HMSNETWORK_hpp
#define HMSNETWORK_hpp

#include "defines.hpp"
#include "SecureOTA.hpp"
#include <ESPmDNS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

extern AsyncWebServer server;
extern WiFiClient espClient;

class HMSnetwork
{
public:
    // constructors
    HMSnetwork();
    virtual ~HMSnetwork();
    // Functions
    bool SetupNetworkStack();
    int CheckWifiState();
    bool connectToApWithFailToStation();
    void SetupWebServer();
    int DiscovermDNSBroker();
    void SetupmDNSServer();
    bool mDNSLoop();
    void loadMQTTConfig();
    void SetupServer();
    void CheckNetworkLoop();
    void SetupWifiScan();
    bool LoopWifiScan();
    void setupOTA();
    void loopOTA();

    // variables
private:
    int maxVoltage;
    int maxTemp;
};

extern HMSnetwork network;
#endif
