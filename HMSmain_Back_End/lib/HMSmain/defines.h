/*
 defines.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#ifndef DEFINES_h
#define DEFINES_h
#include <Arduino.h>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtoul */
#include <timeObj.h>
//IO
#include <Wire.h>
#include <i2cscan.h>
//FrontEnd
#include <FrontEnd.h>
//File System
#include <EEPROM.h>
#include <SPIFFS.h>
//Data stack
#include <AccumulateData.h>
#include <ACS712.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
#include <HMSmqtt.h>
#include <ArduinoJson.h>
//Humidity Sensors
#include <sfm3003.h>
#include <Adafruit_SHT31.h>
//Temp Sensors
#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// wifi definition
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager/tree/development
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <Wifi.h>
#include <WiFiClientSecure.h>

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "HBAT_HMS" // default hostname
#define ENABLE_MULTICAST_DNS // allows to access the UI via "http://<HOSTNAME>.local/"
#define ENABLE_MQTT_SUPPORT               // allows integration in homeassistant/googlehome/mqtt
/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web interface "settings" page
#define MQTT_ENABLED 0
#define MQTT_HOSTNAME "homeassistant.local"
#define MQTT_PORT 1883
#define MQTT_USER "MyUserName"
#define MQTT_PASS ""
#define MQTT_TOPIC_SET "/set" // MQTT Topic to subscribe to for changes(Home Assistant)
#define MQTT_HOMEASSISTANT_TOPIC "homeassistant/HBAT/data" // MQTT Topic to Publish to for state and config (Home Assistant)
#define MQTT_TOPIC "hms/data"  
#define MQTT_DEVICE_NAME "HBAT_HMS"                           // MQTT Topic to Publish to for state and config (Any MQTT Broker)
#define MQTT_UNIQUE_IDENTIFIER WiFi.macAddress() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024
//MQTT
#include <MQTT.h>
#include <mqttconfig.h> /* MQTT data Struct */
#include <PubSubClient.h> // Include the MQTT Library, must be installed via the library manager
extern WiFiClient espClient;
extern PubSubClient mqttClient(espClient);
extern HMSmqtt MqttData;
#endif
/*###################### MQTT Configuration END ######################*/
//define Debug MACROS
#define PURR_DEBUG 1
#if PURR_DEBUG != 0
#define SERIAL_DEBUG_ADD(s) Serial.print(s);
#define SERIAL_DEBUG_ADDF(format, ...) Serial.printf(format, __VA_ARGS__);
#define SERIAL_DEBUG_EOL Serial.print("\n");
#define SERIAL_DEBUG_BOL Serial.printf("DEBUG [%lu]: ", millis());
#define SERIAL_DEBUG_LN(s) SERIAL_DEBUG_BOL SERIAL_DEBUG_ADD(s) SERIAL_DEBUG_EOL
#define SERIAL_DEBUG_LNF(format, ...) SERIAL_DEBUG_BOL SERIAL_DEBUG_ADDF(format, __VA_ARGS__) SERIAL_DEBUG_EOL
#else
#define SERIAL_DEBUG_ADD(s) \
    do                      \
    {                       \
    } while (0);
#define SERIAL_DEBUG_ADDF(format, ...) \
    do                                 \
    {                                  \
    } while (0);
#define SERIAL_DEBUG_EOL \
    do                   \
    {                    \
    } while (0);
#define SERIAL_DEBUG_BOL \
    do                   \
    {                    \
    } while (0);
#define SERIAL_DEBUG_LN(s) \
    do                     \
    {                      \
    } while (0);
#define SERIAL_DEBUG_LNF(format, ...) \
    do                                \
    {                                 \
    } while (0);
#endif
// define externalised classes
extern AccumulateData accumulatedData;
extern Scanner i2cscanner;
extern TaskHandle_t runserver;
extern TaskHandle_t accumulatedata;
extern Scanner scanner;
extern WiFiManager wifiManager;
extern timeObj ReadTimer(5000);
extern HMS HMSmain;
extern Humidity Hum;
extern CellTemp Cell_Temp;
extern StaticJsonDocument<1000> doc;

//Variables

bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

// IO


#define LED_BUILTIN 2

#endif
