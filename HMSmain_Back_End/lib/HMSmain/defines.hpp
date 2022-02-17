/*
 defines.hpp - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#ifndef DEFINES_hpp
#define DEFINES_hpp
#define VERSION "0.0.1"
#define VERSION_DATE "2021-12-17"
#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Board setting.
#endif
#include <Arduino.h>
#include <globaldebug.hpp>
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtoul */
#include <timedtasks.hpp>

// IO
#include <Wire.h>
#include <i2cscan.hpp>
// FrontEnd
#include <FrontEnd.hpp>
// File System
#include <SPIFFS.h>
// Data stack
#include <AccumulateData.hpp>
#include <ACS712.h>
#include <HMS.hpp>
#include <Humidity.hpp>
#include <celltemp.hpp>
#include <HMSmqtt.hpp>
#include <ArduinoJson.h>
// Humidity Sensors
//#include <sfm3003.hpp>
#include <Adafruit_SHT31.h>

// Temp Sensors

#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// wifi definition
#include <AsyncElegantOTA.h>
#include <Wifi.h>
#include <ESPAsync_WiFiManager.h>
#include <HMSnetwork.hpp>
#include <ESPmDNS.h>

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden

#define LED_BUILTIN 2
#define DEFAULT_HOSTNAME "HBAT_HMS" // default hostname
#define ENABLE_MQTT_SUPPORT 0       // allows integration in homeassistant/googlehome/mqtt
#define maxCellCount 10             // max number of cells
#include <config.hpp>               /* data Struct */

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web interface "settings" page
#define MQTT_ENABLED 1
#define MQTT_SECURE_ENABLED 0
#define MQTT_PORT 1883
#define MQTT_PORT_SECURE 8883
#define MQTT_UNIQUE_IDENTIFIER HMSmain.getDeviceID() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024
// MQTT includes
#include <PubSubClient.h>
extern WiFiClient espClient;
extern PubSubClient mqttClient;

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

// define externalized classes
extern AccumulateData accumulatedData;

extern Scanner scanner;

// Custom Objects
extern HMS HMSmain;
extern Humidity Hum;
extern CellTemp Cell_Temp;
extern StaticJsonDocument<1000> Doc;
extern Adafruit_SHT31 sht31;
extern Adafruit_SHT31 sht31_2;
extern FrontEnd Front_End;
extern DNSServer dnsServer;

// Tasks for the Task Scheduler
/* extern TaskHandle_t runserver;
extern TaskHandle_t accumulatedata; */

// Variables
extern const char *mqtt_mDNS_clientId;
extern char mDNS_hostname[4];

extern int mqttPort;

extern int period;
extern unsigned long time_now;
extern bool Charge_State;
// Wifi Variables
//  Set these to your desired credentials.
extern char *ssid;
extern char *password;
extern bool wifiMangerPortalRunning;
extern bool wifiConnected;

// IO

// Globally available functions
char *StringtoChar(String inputString);
char *MQTTCreateHostName(const char *hostname, const char *def_host);

#endif
