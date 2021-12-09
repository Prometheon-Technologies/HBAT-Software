/*
 frontend.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef FRONTEND_h
#define FRONTEND_h
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
/* #include <index.html.h> */
#include <AccumulateData.h>

#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

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

    // Variable

private:
};
#endif