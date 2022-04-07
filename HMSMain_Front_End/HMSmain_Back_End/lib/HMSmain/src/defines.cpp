#include "defines.hpp"

// Variables for MQTT

#ifdef ENABLE_MQTT_SUPPORT
const char *MQTT_TOPIC = "hms/data/";
const String HOMEASSISTANT_MQTT_HOSTNAME = "homeassistant.local";
const String MQTT_HOSTNAME = "hbat.mqtt";
const String MQTT_USER = "MyUserName";
const String MQTT_PASS = "";
const String MQTT_HOMEASSISTANT_TOPIC_SET = "/set";                  // MQTT Topic to subscribe to for changes(Home Assistant)
const String MQTT_HOMEASSISTANT_TOPIC = "homeassistant/HBAT/data";   // MQTT Topic to Publish to for state and config (Home Assistant);
String MQTT_DEVICE_NAME = "HBAT_HMS" + MQTT_UNIQUE_IDENTIFIER; // MQTT Topic to Publish to for state and config (Any MQTT Broker)
static bool mqttProcessing = false;
#endif
/*###################### MQTT Configuration END ######################*/

// define externalized classes

//Custom Objects
StaticJsonDocument<1000> Doc;
Adafruit_SHT31 sht31;
Adafruit_SHT31 sht31_2;

/* // Tasks for the Task Scheduler
TaskHandle_t runserver;
TaskHandle_t accumulatedata; */

// Variables
const char* mqtt_mDNS_clientId = StringtoChar(DEFAULT_HOSTNAME);
char mDNS_hostname[4] = {'h','b' ,'a' ,'t'};

int period = 500;
unsigned long time_now = 0;
bool Charge_State;
//Wifi Variables
bool wifiMangerPortalRunning = false;
bool wifiConnected = false;

// Globally available functions
char *StringtoChar(String inputString)
{
  char *outputString;
  outputString = NULL;
  resizeBuff(inputString.length() + 1, &outputString);
  strcpy(outputString, inputString.c_str());
  return outputString;
}

char* appendChartoChar(const char* hostname, const char* def_host)
{
  // create hostname
  int numBytes = strlen(hostname ) + strlen(def_host) + 1; // +1 for the null terminator | allocate a buffer of the required size
  char* hostname_str = NULL;
  resizeBuff(numBytes, &hostname_str);
  strcpy(hostname_str, hostname);
  strcat(hostname_str, def_host); // append default hostname to hostname
  return hostname_str;
}
