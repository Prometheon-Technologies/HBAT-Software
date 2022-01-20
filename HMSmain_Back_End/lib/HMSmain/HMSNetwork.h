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
    int CheckWifiState();
    void connectToApWithFailToStation();
    void CheckNetworkLoop();
    int DiscovermDNSBroker();
    void SetupmDNSServer();
    void SetupmDNSLoop();

    //variables
private:

};

#endif
