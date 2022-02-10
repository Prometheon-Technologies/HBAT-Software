#include <defines.hpp>

// define EEPROM settings
// https://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-für-parameter-verwenden
// define debugging MACROS
#define DEFAULT_HOSTNAME "HBAT_HMS" // default hostname
#define ENABLE_MQTT_SUPPORT 0      // allows integration in homeassistant/googlehome/mqtt
#define maxCellCount 10             // max number of cells
#include <config.hpp>   /* data Struct */

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
WiFiClient espClient;
PubSubClient mqttClient(espClient);

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
HMSmqtt MqttData;
AccumulateData accumulatedData;

Scanner scanner;

//Custom Objects
HMS HMSmain;
Humidity Hum;
CellTemp Cell_Temp;
StaticJsonDocument<1000> Doc;
Adafruit_SHT31 sht31;
Adafruit_SHT31 sht31_2;
FrontEnd Front_End;
HMSnetwork Network;
IPAddress mqttServer;
AsyncWebServer webServer(80);
DNSServer dnsServer;

/* // Tasks for the Task Scheduler
TaskHandle_t runserver;
TaskHandle_t accumulatedata; */

// Variables
const char* mqtt_mDNS_clientId = StringtoChar(MQTT_HOSTNAME);
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

char *StringtoChar(String inputString)
{
  char *outputString;
  outputString = NULL;
  resizeBuff(inputString.length() + 1, &outputString);
  strcpy(outputString, inputString.c_str());
  return outputString;
}