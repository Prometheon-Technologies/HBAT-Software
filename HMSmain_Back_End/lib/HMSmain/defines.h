/*
 defines.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#ifndef DEFINES_h
#define DEFINES_h
#define VERSION "0.0.1"
#define VERSION_DATE "2021-12-17"
#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Board setting.
#endif
#include <globaldebug.h>
#include <Arduino.h>
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtoul */
#include <timeObj.h>
#include <TaskScheduler.h>
// IO
#include <Wire.h>
#include <i2cscan.h>
// FrontEnd
#include <FrontEnd.h>
// File System
#include <EEPROM.h>
#include <SPIFFS.h>
// Data stack
#include <AccumulateData.h>
#include <ACS712.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
#include <HMSmqtt.h>
#include <ArduinoJson.h>
// Humidity Sensors
//#include <sfm3003.h>
#include <Adafruit_SHT31.h>
// Temp Sensors
#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// wifi definition
#include <AsyncElegantOTA.h>
#include <Wifi.h>
#include <ESPAsync_WiFiManager.h>
#include <HMSNetwork.h>
#include <ESPmDNS.h>

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "HBAT_HMS" // default hostname
#define ENABLE_MQTT_SUPPORT 0      // allows integration in homeassistant/googlehome/mqtt
#define maxCellCount 10             // max number of cells
#include <config.h>   /* data Struct */

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
extern PubSubClient mqttClient(espClient);

// Variables for MQTT
const char *MQTT_TOPIC = "hms/data/";
const String HOMEASSISTANT_MQTT_HOSTNAME = "homeassistant.local";
const String MQTT_HOSTNAME = "hbat.mqtt.local";
const String MQTT_USER = "MyUserName";
const String MQTT_PASS = "";
const String MQTT_HOMEASSISTANT_TOPIC_SET = "/set";                  // MQTT Topic to subscribe to for changes(Home Assistant)
const String MQTT_HOMEASSISTANT_TOPIC = "homeassistant/HBAT/data";   // MQTT Topic to Publish to for state and config (Home Assistant);
String MQTT_DEVICE_NAME = "HBAT_HMS" + MQTT_UNIQUE_IDENTIFIER; // MQTT Topic to Publish to for state and config (Any MQTT Broker)
static bool mqttProcessing = false;

#endif
/*###################### MQTT Configuration END ######################*/

// define externalized classes
extern HMSmqtt MqttData;
extern AccumulateData accumulatedData;

extern Scanner scanner;

// Timer objects
extern timeObj ReadTimer(5000);
extern timeObj ReadTimer_10(10000);
extern timeObj ReadTimer_90(90000);
extern timeObj ReadTimer_2(5000);
extern timeObj ReadTimer_10_2(10000);
extern timeObj ReadTimer_90_2(90000);
extern timeObj ReadTimer_3(5000);
extern timeObj ReadTimer_10_3(10000);
extern timeObj ReadTimer_90_3(90000);
extern timeObj ReadTimer_4(5000);
extern timeObj ReadTimer_10_4(10000);
extern timeObj ReadTimer_90_4(90000);

//Custom Objects
extern HMS HMSmain;
extern Humidity Hum;
extern CellTemp Cell_Temp;
extern StaticJsonDocument<1000> doc;
extern Adafruit_SHT31 sht31;
extern Adafruit_SHT31 sht31_2;
extern FrontEnd Front_End;
extern HMSNetwork Network;
extern IPAddress mqttServer;
extern AsyncWebServer webServer(80);
#if !( USING_ESP32_S2 || USING_ESP32_C3 )
DNSServer dnsServer;
#endif

// Tasks for the Task Scheduler
extern TaskHandle_t runserver;
extern TaskHandle_t accumulatedata;

// Variables
const int ledPin = 2;
const char* mqtt_mDNS_clientId = Front_End.StringtoChar(MQTT_HOSTNAME);
char mDNS_hostname[4] = {'h','b' ,'a' ,'t'};

int mqttPort;

int period = 500;
unsigned long time_now = 0;
bool Charge_State;
//Wifi Variables
// Set these to your desired credentials.
char *ssid = "HBAT_HMS";
char *password = "hbathbat";
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

// IO
#define LED_BUILTIN 2

// Globally available functions


#endif
