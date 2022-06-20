/*
 Network.h - HBAT library
 Copyright (c) 2021 ZanzyTHEbar
*/

#ifndef HMSNETWORK_hpp
#define HMSNETWORK_hpp

#include <defines.hpp>
//#include "SecureOTA.hpp"
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
    // bool connectToApWithFailToStation();
    
    void SetupWebServer();
    void SetupServer();
    bool CheckNetworkLoop();
    void CheckConnectionLoop_Active();
    void networkRoutes();
    bool LoopWifiScan();

    friend void wifiClear();
    friend void wifiConnect();
    friend void wifiDisconnect();

    // variables
private:
    int CheckWifiState();
    unsigned long _previousMillis;

    const size_t _MAX_FILESIZE; // 2MB
    const char *_HTTP_USERNAME;
    const char *_HTTP_PASSWORD;

    // Timer variables
    const long _interval = 30000; // interval to wait for Wi-Fi connection (milliseconds)
};

extern HMSnetwork network;
#endif
