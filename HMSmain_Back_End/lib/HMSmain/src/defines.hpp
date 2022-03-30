/*
 defines.hpp - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
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
#include "globaldebug.hpp"
#include "config.hpp"

// IO
#include <Wire.h>
#include "i2cscan.hpp"

// FrontEnd
#include "FrontEnd.hpp"

// Data stack
#include "HMS.hpp"
#include "celltemp.hpp"
#include "Humidity.hpp"
// Humidity Sensors
//#include <sfm3003.hpp>

// Temp Sensors

// wifi definitions
#include "HMSnetwork.hpp"

// Accumulate Data
#include "accumulatedata.hpp"

// Timed tasks
#include "Relays.hpp"
#include "timedtasks.hpp"

#ifdef DEFAULT_HOSTNAME
#pragma message DEFAULT_HOSTNAME
#endif

#ifdef PRODUCTION
#pragma message STR(PRODUCTION)
#endif

#define maxCellCount 10 // max number of cells

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
#pragma message STR(ENABLE_MQTT_SUPPORT)
// these are deafault settings which can be changed in the web interface "settings" page
#define MQTT_ENABLED 1
#define MQTT_SECURE_ENABLED 0
#define MQTT_PORT 1883
#define MQTT_PORT_SECURE 8883
#define MQTT_UNIQUE_IDENTIFIER HMSmain.generateDeviceID() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)

#define MQTT_MAX_TRANSFER_SIZE 1024
// MQTT includes
#include "HMSmqtt.hpp"

// Variables for MQTT
extern const char *MQTT_TOPIC;
extern const String HOMEASSISTANT_MQTT_HOSTNAME;
extern const String MQTT_HOSTNAME;
extern const String MQTT_USER;
extern const String MQTT_PASS;
extern const String MQTT_HOMEASSISTANT_TOPIC_SET; // MQTT Topic to subscribe to for changes(Home Assistant)
extern const String MQTT_HOMEASSISTANT_TOPIC;     // MQTT Topic to Publish to for state and config (Home Assistant);
extern String MQTT_DEVICE_NAME;                   // MQTT Topic to Publish to for state and config (Any MQTT Broker)
extern bool mqttProcessing;
#endif
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
// Temporary function to ensure that the correct number of cells are being read - this will be removed when the cell count is dynamically allocated

// Globally available functions
char *StringtoChar(String inputString);
char *appendChartoChar(const char *hostname, const char *def_host);
void my_delay(volatile long delay_time);

#endif
