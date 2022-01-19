/*
 defines.h - Purbright library
 Copyright (c) 2021 Zacariah Austin Heim.
 */
#ifndef DEFINES_h
#define DEFINES_h
#define VERSION "0.0.1"
#define VERSION_DATE "2021-12-17"
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
#include <sfm3003.h>
#include <Adafruit_SHT31.h>
// Temp Sensors
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
#include <HMSNetwork.h>

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "HBAT_HMS" // default hostname
#define ENABLE_MULTICAST_DNS        // allows to access the UI via "http://<HOSTNAME>.local/"
#define ENABLE_MQTT_SUPPORT         // allows integration in homeassistant/googlehome/mqtt
#define maxCellCount 10             // max number of cells

/*######################## MQTT Configuration ########################*/
#ifdef ENABLE_MQTT_SUPPORT
// these are deafault settings which can be changed in the web interface "settings" page
#define MQTT_ENABLED 0 // 0 = disabled, 1 = enabled
#define MQTT_SECURE_ENABLED 0 // 0 = disabled, 1 = enabled
#define MQTT_PORT 1883
#define MQTT_PORT_SECURE 8883
#define MQTT_UNIQUE_IDENTIFIER HMSmain.getDeviceID() // A Unique Identifier for the device in Homeassistant (MAC Address used by default)
#define MQTT_MAX_PACKET_SIZE 1024
#define MQTT_MAX_TRANSFER_SIZE 1024
// MQTT includes
#include <MQTT.h>
#include <mqttconfig.h>   /* MQTT data Struct */
#include <PubSubClient.h> // Include the MQTT Library, must be installed via the library manager
extern WiFiClient espClient;
extern PubSubClient mqttClient(espClient);
extern HMSmqtt MqttData;
extern MQTTClient mqtt;

// Variables for MQTT
const String MQTT_TOPIC = "hms/data/";
const String HOMEASSISTANT_MQTT_HOSTNAME = "homeassistant.local";
const String MQTT_HOSTNAME = "hbat.mqtt.local";
const String MQTT_USER = "MyUserName";
const String MQTT_PASS = "";
const String MQTT_TOPIC = "hms/data/";
const String MQTT_HOMEASSISTANT_TOPIC_SET = "/set";                  // MQTT Topic to subscribe to for changes(Home Assistant)
const String MQTT_HOMEASSISTANT_TOPIC = "homeassistant/HBAT/data";   // MQTT Topic to Publish to for state and config (Home Assistant);
const String MQTT_DEVICE_NAME = "HBAT_HMS" + MQTT_UNIQUE_IDENTIFIER; // MQTT Topic to Publish to for state and config (Any MQTT Broker)

#endif
/*###################### MQTT Configuration END ######################*/

// define externalized classes
extern AccumulateData accumulatedData;

extern Scanner scanner;
extern WiFiManager wifiManager;
extern timeObj ReadTimer(5000);
extern HMS HMSmain;
extern Humidity Hum;
extern CellTemp Cell_Temp;
extern StaticJsonDocument<1000> doc;
extern Adafruit_SHT31 sht31;
extern Adafruit_SHT31 sht31_2;
extern WiFiClientSecure net;
extern WebServer server(80);
extern FrontEnd Front_End;
extern HMSNetwork Network;

// Tasks for the Task Scheduler
extern Scheduler runner;
extern Task tcontrolTasks();
extern Task tbeginWiFiIfNeeded(1000, 10, &tbeginWiFiIfNeededCallback);
extern Task tconnectMQTTClientIfNeeded(1000, TASK_FOREVER, &tconnectMQTTClientIfNeededCallback);
extern Task tallOn(1000, TASK_EXTRA_STACK_SIZE, &tallOnCallback);
extern Task tallOff(1000, TASK_EXTRA_STACK_SIZE, &tallOnCallback);
extern TaskHandle_t runserver;
extern TaskHandle_t accumulatedata;

// Variables

int period = 500;
unsigned long time_now = 0;
bool Charge_State;
//Wifi Variables
// Set these to your desired credentials.
const String ssid = "HBAT_HMS";
const String password = "hbathbat";
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

// IO
#define LED_BUILTIN 2

// Globally available functions

/******************************************************************************
 * Function: Call back functions for Task Scheduler
 * Description: These functions are called by the Task Scheduler
 * Parameters: None
 * Return: None
 * THIS IS EXAMPLE CODE AND SHOULD BE MODIFIED TO FIT THE NEEDS
 * 
 * https://github.com/arkhipenko/TaskScheduler/blob/master/examples/Scheduler_example01/Scheduler_example01.ino
 ******************************************************************************/
void tbeginWiFiIfNeededCallback();
void tconnectMQTTClientIfNeededCallback();
void tallOnCallback();

void tbeginWiFiIfNeededCallback() //#TODO: add a timeout
{
    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;
        //runner.removeTask(tbeginWiFiIfNeeded);
    }
    else
    {
        wifiConnected = false;
    }

    SERIAL_DEBUG_ADD("tbeginWiFiIfNeeded: ");
    SERIAL_DEBUG_LN(millis());

    if (tbeginWiFiIfNeeded.isFirstIteration())
    {
        runner.addTask(tallOn);
        tallOn.enable();
        SERIAL_DEBUG_LN("tbeginWiFiIfNeeded: enabled tallOn and added to the chain");
    }

    if (tbeginWiFiIfNeeded.isLastIteration())
    {
        tallOn.disable();
        runner.deleteTask(tallOn);
        tconnectMQTTClientIfNeeded.setInterval(500);
        SERIAL_DEBUG_LN("tbeginWiFiIfNeeded: disable tallOn and delete it from the chain. tconnectMQTTClientIfNeeded interval set to 500");
    }
}

void tconnectMQTTClientIfNeededCallback()//#TODO: Implement MQTTClient task
{
    SERIAL_DEBUG_ADD("tconnectMQTTClientIfNeeded: ");
    SERIAL_DEBUG_LN(millis());
}

void tallOnCallback()
{
    SERIAL_DEBUG_ADD("tallOn: ");
    SERIAL_DEBUG_LN(millis());
}

//Dual Core Task Pinning Functions

void TasktoRunNetworkStack(void *parameter)
{
    SERIAL_DEBUG_ADD("Webserver running on core ");
    SERIAL_DEBUG_LN(xPortGetCoreID());
    for (;;)
    {
        Front_End.ClientLoop();
        if (Front_End.mqttFrontEndCondition == true)
        {
            for (;;)
            {
                MqttData.MQTTLoop();
            }
        }
    }
}

void TasktoAccumulateSensorData(void *pvParameters)
{
    SERIAL_DEBUG_ADD("Data Accumulation running on core ");
    SERIAL_DEBUG_LN(xPortGetCoreID());
    for (;;)
    {
        if (ReadTimer.ding())
        {
            accumulatedData.InitAccumulateDataJson();
            Hum.SFM3003();
            ReadTimer.start();
        }
    }
}

void setupTasks()
{
    runner.init();
    runner.addTask(tbeginWiFiIfNeeded);
    runner.addTask(tconnectMQTTClientIfNeeded);
    delay(100);
    tbeginWiFiIfNeeded.enable();
    tconnectMQTTClientIfNeeded.enable();

    if (Serial.available() > 0)
    {
        SERIAL_DEBUG_LN("Scheduler TEST");

        runner.init();
        SERIAL_DEBUG_LN("Initialized scheduler");
        SERIAL_DEBUG_LN("added tbeginWiFiIfNeeded");
        SERIAL_DEBUG_LN("added tconnectMQTTClientIfNeeded");
        SERIAL_DEBUG_LN("Enabled tbeginWiFiIfNeeded");
        SERIAL_DEBUG_LN("Enabled tconnectMQTTClientIfNeeded");
    }
}

#endif
