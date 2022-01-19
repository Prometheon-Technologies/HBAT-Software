/*
 HMSNetwork.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef HMSNetwork_h
#define HMSNetwork_h

#include <defines.h>

class HMSNetwork
{
public:
    //constructors
    HMSNetwork();
    virtual ~HMSNetwork();
    //Functions
    void HMSNetworkSetup();
    void HMSNetworkLoop();
    void RunService();
    void MessageReceived(String &topic, String &payload);
    String getRebootString();
    void handleReboot();
    void ClientLoop();
    void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS);

    //variables
private:

};

#endif
