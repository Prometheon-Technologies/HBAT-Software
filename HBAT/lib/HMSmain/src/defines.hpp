/*
 defines.hpp - HBAT library
 Copyright (c) 2021 ZanzyTHEbar
 */
#ifndef DEFINES_hpp
#define DEFINES_hpp
#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Board setting.
#endif

#define LED_BUILTIN 2

#include <Arduino.h>
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtoul */
#include <ArduinoJson.h>
#include <MD5.h>
#include <strTools.h>
#include <PID_v1.h>

// File System
#include <SPIFFS.h>

// data Struct
#include "config.hpp"

// IO
#include <Wire.h>
#include "i2cscan.hpp"

// Data stack
#include "HMS.hpp"
#include "celltemp.hpp"
#include "Humidity.hpp"
// Humidity Sensors
//#include <sfm3003.hpp>

// Temp Sensors

// wifi definitions
#include "HMSnetwork.hpp"
#include "mdns.hpp"
#include "OTA.hpp"

// Accumulate Data
#include "accumulatedata.hpp"

// Timed tasks
#include "Relays.hpp"
#include "timedtasks.hpp"

#define STR(x) #x
#define XSTR(x) STR(x)
#define MSG(x)

#ifdef DEFAULT_HOSTNAME
#pragma message DEFAULT_HOSTNAME
#endif

#ifdef PRODUCTION
_Pragma (STR(message (PRODUCTION)))
#endif

#define maxCellCount 10 // max number of cells

// Globally available functions
char *StringtoChar(String inputString);
char *appendChartoChar(const char *hostname, const char *def_host);
void my_delay(volatile long delay_time);
String generateDeviceID();

/*######################## MQTT Configuration ########################*/
#if ENABLE_MQTT_SUPPORT
_Pragma (STR(message (ENABLE_MQTT_SUPPORT)))
// MQTT includes
#include "HMSmqtt.hpp"
#endif // ENABLE_MQTT_SUPPORT
/*###################### MQTT Configuration END ######################*/

// Variables
extern const char *mqtt_mDNS_clientId;
extern char mDNS_hostname[4];

extern int period;
extern unsigned long time_now;
extern bool Charge_State;

// Wifi Variables
extern bool wifiMangerPortalRunning;
extern bool wifiConnected;

#endif
