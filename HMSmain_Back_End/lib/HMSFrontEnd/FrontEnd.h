/*
 frontend.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef FRONTEND_h
#define FRONTEND_h
#include <Arduino.h>
// wifi definition
#include <WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager/tree/development
WiFiManager wifiManager;
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include <HMSmqtt.h>
#include <AccumulateData.h>

class FrontEnd
{
public:
    // Functions
    FrontEnd(void);
    virtual ~FrontEnd(void);
    String json_return_data();
    void SetupServer();
    void ClientLoop();
    void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS);
    uint8_t GetChipID();

    // Variables

    int maxVoltage;
    int maxTemp;

    String NewMQTTIP;
    String NewMQTTPass;
    String NewMQTTUser;
    String json;
    String temp;
    unsigned long ClientID;
    char clientIP;
    String clientPass;
    String clientUser;
    bool mqttFrontEndCondition;

private:
};
#endif