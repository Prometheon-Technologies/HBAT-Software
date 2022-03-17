/*
 frontend.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef FRONTEND_h
#define FRONTEND_h
#include "defines.hpp"

class FrontEnd
{
public:
    // Functions
    FrontEnd(void);
    virtual ~FrontEnd(void);
    String json_return_data();
    void SetupServer();
    void ClientLoop();
    void FrontEndLoop();
    void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS);
    void updateCurrentData();
    uint8_t GetChipID();
    void loadConfig();
    void setupFrontEnd();

    // Variables

    String NewMQTTIP;
    String NewMQTTPass;
    String NewMQTTUser;
    String AccumulateSensorjson;
    String temp;
    unsigned long ClientID;
    char clientIP;
    String clientPass;
    String clientUser;
    bool mqttEnabled;

private:
};

extern FrontEnd Front_End;
#endif