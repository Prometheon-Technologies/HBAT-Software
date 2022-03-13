
#include "HMSnetwork.hpp"

IPAddress mqttServer;
AsyncWebServer server(80);
IPAddress localIP;

IPAddress mqttServer;

WiFiClient espClient;

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "password";
const char *mdnsPath = "/mdns.txt";
const char *dhcpcheckPath = "/dhcpcheck.txt";
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *ntptimePath = "/ntptime.txt";
const char *ntptimeoffsetPath = "/ntptimeoffset.txt";
String mdnsdotlocalurl = "HMSBAT";

const uint16_t OTA_CHECK_INTERVAL = 3000; // ms
uint32_t _lastOTACheck = 0;

// char* create_mDNS_hostname = StringtoChar(MQTTCreateHostName(MQTT_HOSTNAME, ".local"));

String SSID;
String PASS;
String ntptime;
String ntptimeoffset;
String mdns;
String dhcpcheck;

String ledState;

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

HMSnetwork::HMSnetwork()
{
  // constructor
  SERIAL_DEBUG_LN("[INFO]: HMSnetwork::HMSnetwork()");
  SERIAL_DEBUG_LN("[INFO]: Creating network object");
  maxVoltage = 24;
  maxTemp = 100;
}

HMSnetwork::~HMSnetwork()
{
  // destructor
  SERIAL_DEBUG_LN("[INFO]: HMSnetwork::~HMSnetwork()");
  SERIAL_DEBUG_LN("[INFO]: Destroying network object");
}

bool HMSnetwork::SetupNetworkStack()
{
  if (!cfg.loadConfig())
  {
    SERIAL_DEBUG_LN("[INFO]: Failed to load config");
  }
  else
  {
    SERIAL_DEBUG_LN("[INFO]: Loaded config");
    // Load values saved in SPIFFS
    SSID = cfg.readFile(SPIFFS, ssidPath);
    SERIAL_DEBUG_LN(SSID);
    PASS = cfg.readFile(SPIFFS, passPath);
    if (!PRODUCTION)
    {
      // print it on the serial monitor
      SERIAL_DEBUG_LN(PASS);
    }

    ntptime = cfg.readFile(SPIFFS, ntptimePath);
    ntptimeoffset = cfg.readFile(SPIFFS, ntptimeoffsetPath);

    mdns = cfg.readFile(SPIFFS, mdnsPath);
    dhcpcheck = cfg.readFile(SPIFFS, dhcpcheckPath);

    SERIAL_DEBUG_LN(mdns);
    SERIAL_DEBUG_LN(dhcpcheck);

    // Save loaded values to config struct
    heapStr(&cfg.config.WIFISSID, StringtoChar(SSID));
    heapStr(&cfg.config.WIFIPASS, StringtoChar(PASS));
    heapStr(&cfg.config.NTPTIME, StringtoChar(ntptime));
    heapStr(&cfg.config.NTPTIMEOFFSET, StringtoChar(ntptimeoffset));
    heapStr(&cfg.config.MDNS, StringtoChar(mdns));
    heapStr(&cfg.config.DHCPCHECK, StringtoChar(dhcpcheck));
  }

  if (cfg.config.WIFISSID[0] == '\0' || cfg.config.WIFIPASS[0] == '\0')
  {
    SERIAL_DEBUG_LN("[INFO]: No SSID or password has been set.");
    SERIAL_DEBUG_LN("[INFO]: Please configure the Wifi Manager by scanning the QR code on your device.");
    SERIAL_DEBUG_LN("");
    return false;
  }
  else
  {
    SERIAL_DEBUG_LN("[INFO]: Configured SSID: ");
    SERIAL_DEBUG_ADD(cfg.config.WIFISSID);
    SERIAL_DEBUG_LN("");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Disconnect from WiFi AP if connected
    if (dhcpcheck == "on")
    {
      SERIAL_DEBUG_LN("[INFO]: DHCP Check is on");
      localIP.fromString(cfg.config.clientIP);

      if (!WiFi.config(localIP, gateway, subnet))
      {
        SERIAL_DEBUG_LN("[INFO]: STA Failed to configure");
        return false;
      }

      WiFi.begin(cfg.config.WIFISSID, cfg.config.WIFIPASS);

      unsigned long currentMillis = millis();
      previousMillis = currentMillis;

      while (WiFi.status() != WL_CONNECTED)
      {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
          SERIAL_DEBUG_LN("[INFO]: WiFi connection timed out.");
          return false;
        }
      }

      SERIAL_DEBUG_LN("[INFO]: Connected to WiFi.");
      SERIAL_DEBUG_ADD("IP address: ");
      SERIAL_DEBUG_LN(WiFi.localIP());
      return true;
    }
  }
}

void HMSnetwork::SetupWebServer()
{
  if (SetupNetworkStack())
  {
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/frontend.html", "text/html"); });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/wifiupdate", HTTP_POST, [&](AsyncWebServerRequest *request)
              {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == "apName") {
            String ssID; 
            ssID = p->value().c_str();
            SERIAL_DEBUG_ADD("SSID set to: ");
            SERIAL_DEBUG_LN(ssID);
            // Write file to save value
            cfg.writeFile(SPIFFS, ssidPath, ssID.c_str());
          }
          // HTTP POST pass value
          if (p->name() == "apPass") {
            String passWord; 
            passWord = p->value().c_str();
            SERIAL_DEBUG_ADD("Password set to: ");
            SERIAL_DEBUG_LN(passWord);
            // Write file to save value
            cfg.writeFile(SPIFFS, passPath, passWord.c_str());
          }
          SERIAL_DEBUG_ADDF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart and connect to your router. To access it go to IP address: " + String(cfg.config.clientIP));
      delay(30000);
      ESP.restart(); });

    // Route to set GPIO state to LOW
    server.on("/toggle", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST Relay Value
          if (p->name() == "pin") {
            String relay = p->value().c_str();
            Serial.print("switching state of pin :");
            Serial.println(relay);
            cfg.config.relays[relay.toInt()] = (cfg.config.relays[relay.toInt()] == true) ? false : true;
          }
          SERIAL_DEBUG_ADDF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "application/json", "toggled"); });

    server.on("/data.json", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
      String json = "";
      json += R"====({)====";

      json += R"====("stack_humidity":)====";
      json += (String)cfg.config.stack_humidity + ",\n";

      json += R"====("stack_temp":)====";
      json += (String)cfg.config.stack_temp + ",\n";

      json += R"====("relays":[)====";
      json += (String)cfg.config.relays[0] + "," + (String)cfg.config.relays[1] + "," + (String)cfg.config.relays[2] + "," + (String)cfg.config.relays[3] + "," + (String)cfg.config.relays[4] + "],\n";

      json += R"====("stack_voltage":)====";
      json += (String)cfg.config.stack_voltage + ",\n";

      json += R"====("GraphData":[)====";
      json += "\n";
      for (int i = 0; i < 10; i++)
      {
        delay(0);
        json += R"====({"label": "🌡 )====" + (String)i + "\",\n";
        json += R"====("type": "temp",)====" + (String) "\n";
        json += R"====("value": )====" + (String)cfg.config.cell_temp[i] + (String) ",\n";
        json += R"====("maxValue": )====" + (String)maxTemp;
        json += R"====(})====" + (String) "\n";
        json += R"====(,)====";

        json += R"====({"label": "⚡ )====" + (String)i + "\",\n";
        json += R"====("type": "volt",)====" + (String) "\n";
        json += R"====("value": )====" + (String)cfg.config.cell_voltage[i] + (String) ",\n";
        json += R"====("maxValue": )====" + (String)maxVoltage;
        json += R"====(})====" + (String) "\n";

        if (i < 9)
        {
          json += R"====(,)====";
        }
      }
      json += R"====(])====";
      json += R"====(})====";
      json = "";
      request->send(200, "application/json", json); });
    server.begin();
    Serial.println("HBAT HMS server started");
  }
  else
  {
    // TODO: Route for root to  "Please Scan QR code" - Route for Wifi Manager /HBAThmswifi page
    // TODO: There should be a reset mode that will reset the device to factory settings and restart the device.
    // TODO: Should be a physical reset button on the PCB itself - not a touch button - hold for 5 seconds to reset. Flash LED to indicate reset per second.
    // Connect to Wi-Fi HMSnetwork with SSID and password
    SERIAL_DEBUG_LN("[INFO]: Setting Access Point...");

    char *macAddr = StringtoChar(WiFi.macAddress());

    unsigned char *hash = MD5::make_hash(macAddr);

    // generate the digest (hex encoding) of our hash
    char *md5str = MD5::make_digest(hash, 16);

    if (!PRODUCTION)
    {
      // print it on the serial monitor
      SERIAL_DEBUG_LN("[INFO]: MD5 HASH of MAC ADDRESS: ");
      SERIAL_DEBUG_ADD(md5str);
      SERIAL_DEBUG_LN("");
    }
    
    // NULL sets an open Access Point
    WiFi.softAP("HMS-WIFI", md5str); // MAC address is used as password for the AP - Unique to each device - MD5 hash of MAC address

    // Give the Memory back to the System if you run the md5 Hash generation in a loop
    free(md5str);
    // free dynamically allocated 16 byte hash from make_hash()
    free(hash);

    IPAddress IP = WiFi.softAPIP();
    SERIAL_DEBUG_ADD("[INFO]: AP IP address: ");
    SERIAL_DEBUG_LN(IP);

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wifimanager.html", "text/html"); });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_POST, [&](AsyncWebServerRequest *request)
              {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            String ssID; 
            ssID = p->value().c_str();
            SERIAL_DEBUG_ADD("SSID set to: ");
            SERIAL_DEBUG_LN(ssID);
            // Write file to save value
            cfg.writeFile(SPIFFS, ssidPath, ssID.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            String passWord; 
            passWord = p->value().c_str();
            SERIAL_DEBUG_ADD("Password set to: ");
            SERIAL_DEBUG_LN(passWord);
            // Write file to save value
            cfg.writeFile(SPIFFS, passPath, passWord.c_str());
          }
          SERIAL_DEBUG_ADDF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + String(cfg.config.clientIP));
      delay(3000);
      ESP.restart(); });
    server.begin();
  }
}

// ######################## server functions #########################
int HMSnetwork::CheckWifiState()
{
  // check if there is a WiFi connection
  int wifiStateCounter = 0;
  SERIAL_DEBUG_ADD("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    SERIAL_DEBUG_ADD(". ");
    wifiStateCounter++;
    if (wifiStateCounter > 100)
      SERIAL_DEBUG_LN("[INFO]: WiFi not connected");
    return 0;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_LN("[INFO]: \nconnected!");
    return 1;
  }
  return 0;
}

/******************************************************************************
 * Function: Check HMSnetwork Connection Loop
 * Description: This function checks the HMSnetwork connection and reconnects if necessary - is called in the loop() function every 5 seconds
 * Parameters: None
 * Return: None
 ******************************************************************************/
void HMSnetwork::CheckNetworkLoop()
{
  // run current function every 5 seconds
  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnected = false;
    SERIAL_DEBUG_LN(F("Wifi is not connected"));
  }
  else
  {
    wifiConnected = true;
    SERIAL_DEBUG_LN(F("Wifi is connected"));
    SERIAL_DEBUG_ADD("[INFO]: WiFi Connected! Open http://");
    SERIAL_DEBUG_ADD(WiFi.localIP());
    SERIAL_DEBUG_LN("[INFO]:  in your browser");
  }
}

int HMSnetwork::DiscovermDNSBroker()
{
  // check if there is a WiFi connection
  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_LN("[INFO]: \nconnected!");

    // ######################## Multicast DNS #########################

    SERIAL_DEBUG_ADD("Setting up mDNS: ");
    if (!MDNS.begin(mqtt_mDNS_clientId))
    {
      SERIAL_DEBUG_LN("[INFO]: [Fail]");
    }
    else
    {
      SERIAL_DEBUG_LN("[INFO]: [OK]");
      SERIAL_DEBUG_ADD("Querying MQTT broker: ");

      int n = MDNS.queryService("mqtt", "tcp");

      if (n == 0)
      {
        // No service found
        SERIAL_DEBUG_LN("[INFO]: [Fail]");
        return 0;
      }
      else
      {
        int mqttPort;
        // Found one or more MQTT service - use the first one.
        SERIAL_DEBUG_LN("[INFO]: [OK]");
        mqttServer = MDNS.IP(0);
        mqttPort = MDNS.port(0);
        heapStr(&(cfg.config.MQTTBroker), mqttServer.toString().c_str());
        int number = mqttPort;
        char charValue = number + '0';
        SERIAL_DEBUG_ADDF("The port is:%c", charValue);
        heapStr(&(cfg.config.MQTTPort), &charValue);
        SERIAL_DEBUG_ADD("MQTT broker found at: ");
        SERIAL_DEBUG_ADD(mqttServer);
        SERIAL_DEBUG_ADD(cfg.config.MQTTBroker);
        SERIAL_DEBUG_ADD(":");
        SERIAL_DEBUG_LN(mqttPort);
        return 1;
      }
    }
    return 1;
  }
  return 0;
}

// ######################## Call this OR SetupWebServer - not both #########################
void HMSnetwork::SetupmDNSServer()
{
  // ######################## Multicast DNS #########################
  SERIAL_DEBUG_ADD("Setting up mDNS: ");
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi HMSnetwork
  if (!MDNS.begin(mdnsdotlocalurl.c_str()))
  {
    SERIAL_DEBUG_LN("[INFO]: Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  SERIAL_DEBUG_LN("[INFO]: mDNS responder started");

  // Start TCP (HTTP) server
  SetupWebServer();
  SERIAL_DEBUG_LN("[INFO]: TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

bool HMSnetwork::SetupmDNSLoop()
{
  // ######################## Multicast DNS #########################
  // Check if a new espClient has connected
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!espClient)
    {
      return false;
    }
    // Wait for data from espClient to become available
    while (!espClient.available())
    {
      delay(1);
    }

    // Read the first line of HTTP request
    String req = espClient.readStringUntil('\r');

    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1)
    {
      SERIAL_DEBUG_ADD("Invalid request: ");
      SERIAL_DEBUG_LN(req);
      return false;
    }
    req = req.substring(addr_start + 1, addr_end);
    SERIAL_DEBUG_ADD("Request: ");
    SERIAL_DEBUG_LN(req);

    String s;
    if (req == "/")
    {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
      s += ipStr;
      s += "</html>\r\n\r\n";
      SERIAL_DEBUG_LN("[INFO]: Sending 200");
    }
    else
    {
      s = "HTTP/1.1 404 Not Found\r\n\r\n";
      SERIAL_DEBUG_LN("[INFO]: Sending 404");
    }
    espClient.print(s);

    espClient.stop();
    SERIAL_DEBUG_LN("[INFO]: Done with espClient");
  }
  return false;
}

// BACKUP LEGACY FUNCTION
/******************************************************************************
 * Function: Connect to Wifi with fail to AP mode if no connection
 * Description: This connection will attempt to create a WiFi connection with the given SSID and password. If the connection fails, it will attempt to connect to a default Wifi AP.
 * Parameters: None
 * Return: None
 ******************************************************************************/
bool HMSnetwork::connectToApWithFailToStation()
{
  WiFi.persistent(true);
  SERIAL_DEBUG_LN("[INFO]: Configuring access point...");
  SERIAL_DEBUG_ADD("SSID:");
  SERIAL_DEBUG_LN(cfg.config.WIFISSID);
  SERIAL_DEBUG_ADD("PASS:");
  SERIAL_DEBUG_LN(cfg.config.WIFIPASS);

  WiFi.mode(WIFI_STA);
  if (cfg.config.WIFISSID[0] == '\0')
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(cfg.config.WIFISSID, cfg.config.WIFIPASS);
  }

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    numberOfAttempts++;
    delay(200);
    SERIAL_DEBUG_ADD(".");
    if (numberOfAttempts > 100)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      SERIAL_DEBUG_ADD("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      WiFi.softAP(WIFI_SSID, WIFI_PASS);
      SERIAL_DEBUG_LN(WiFi.softAPIP());
      return false;
      break;
    }
  }
  SERIAL_DEBUG_LN("[INFO]: ");
  SERIAL_DEBUG_ADD("Connected! IP address: ");
  SERIAL_DEBUG_LN(WiFi.localIP());
  return true;
}

// ############## functions to update current server settings ###################
/**
 * @brief Check if the current hostname is the same as the one in the config file
 * Call in the Setup BEFORE the WiFi.begin()
 * @param None
 * @return None
 */
void HMSnetwork::loadConfig()
{
  SERIAL_DEBUG_LN(F("Checking if hostname is set and valid"));
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  size_t size = sizeof(cfg.config.hostname);
  if (!cfg.isValidHostname(cfg.config.hostname, size - 1))
  {
    strncpy(cfg.config.hostname, DEFAULT_HOSTNAME, size - 1);
    cfg.setConfigChanged();
  }
  WiFi.setHostname(cfg.config.hostname); // define hostname

  if (cfg.config.MQTTEnabled)
  {
    SERIAL_DEBUG_LN(F("MQTT Enabled"));
    SERIAL_DEBUG_LN(F("MQTT Broker: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTBroker);
    SERIAL_DEBUG_LN(F("MQTT Port: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTPort);
    SERIAL_DEBUG_LN(F("MQTT User: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTUser);
    SERIAL_DEBUG_LN(F("MQTT Pass: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTPass);
    SERIAL_DEBUG_LN(F("MQTT Topic: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTTopic);
    SERIAL_DEBUG_LN(F("MQTT Set Topic: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTSetTopic);
    SERIAL_DEBUG_LN(F("MQTT Device Name: "));
    SERIAL_DEBUG_LN(cfg.config.MQTTDeviceName);
  }
  else
  {
    SERIAL_DEBUG_LN(F("MQTT Disabled"));
  }
  SERIAL_DEBUG_LNF("Loaded config: hostname %s", cfg.config.hostname);
}

void HMSnetwork::SetupServer()
{
  SERIAL_DEBUG_EOL;
  SERIAL_DEBUG_LN(F("System Information:"));
  SERIAL_DEBUG_EOL;
  SERIAL_DEBUG_LNF("PLatformI0 compile time: %s (%s)", __DATE__, __TIME__);
  SERIAL_DEBUG_LNF("PLatformI0 Unix compile time: %s", COMPILE_UNIX_TIME);
  SERIAL_DEBUG_LNF("Project directory: %s", PROJECT_PATH);
  SERIAL_DEBUG_LNF("Version: %s (%s)", VERSION, __DATE__);
  SERIAL_DEBUG_LNF("Heap: %d", Cell_Temp.getfreeRam());
  SERIAL_DEBUG_LNF("SDK: %s", system_get_sdk_version());
  SERIAL_DEBUG_LNF("MAC address: %s", WiFi.macAddress().c_str());
  SERIAL_DEBUG_LNF("CPU Speed: %d MHz", ESP.getCpuFreqMHz());
  SERIAL_DEBUG_LNF("Flash Size: %dKB", ESP.getFlashChipSize());
  SERIAL_DEBUG_LN("[INFO]: System Information Sent");
  SERIAL_DEBUG_EOL;

// FS debug information
// THIS NEEDS TO BE PAST THE WIFI SETUP!! OTHERWISE WIFI SETUP WILL BE DELAYED
#if HMS_DEBUG != 0
  SERIAL_DEBUG_LN(F("SPIFFS contents:"));
#ifdef ESP32
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file)
  {
    SERIAL_DEBUG_LNF("FS File: %s, size: %lu", file.name(), file.size());
    file = root.openNextFile();
  }
  SERIAL_DEBUG_EOL;
  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();
#endif
  if (usedBytes == 0)
  {
    SERIAL_DEBUG_LN(F("NO WEB SERVER FILES PRESENT! SEE: \n"));
  }
  SERIAL_DEBUG_LNF("FS Size: %luKB, used: %luKB, %0.2f%%",
                   totalBytes, usedBytes,
                   (float)100 / totalBytes * usedBytes);
  SERIAL_DEBUG_EOL;
#endif
}

void HMSnetwork::SetupWifiScan()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Disconnect from the access point if connected before
  delay(100);

  SERIAL_DEBUG_LN("[INFO]: Setup done");
}

bool HMSnetwork::LoopWifiScan()
{
  SERIAL_DEBUG_LN("[INFO]: Beginning WIFI Network");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  SERIAL_DEBUG_LN("[INFO]: scan done");
  if (n == 0)
  {
    SERIAL_DEBUG_LN("[INFO]: no networks found");
    return false;
  }
  else
  {
    SERIAL_DEBUG_ADD(n);
    SERIAL_DEBUG_LN("[INFO]:  networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      SERIAL_DEBUG_ADD(i + 1);
      SERIAL_DEBUG_ADD(": ");
      SERIAL_DEBUG_ADD(WiFi.SSID(i));
      SERIAL_DEBUG_ADD(" (");
      SERIAL_DEBUG_ADD(WiFi.RSSI(i));
      SERIAL_DEBUG_ADD(")");
      SERIAL_DEBUG_LN((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
      return true;
    }
  }
  SERIAL_DEBUG_LN("[INFO]: ");

  // Wait a bit before scanning again
  delay(5000);
  return true;
}

void HMSnetwork::setupOTA()
{
  Serial.print("Device version: v.");
  Serial.println(VERSION);
  Serial.print("Connecting to " + String(cfg.config.WIFISSID) + "...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");
  _lastOTACheck = millis();
}

void HMSnetwork::loopOTA()
{
  if ((millis() - OTA_CHECK_INTERVAL) > _lastOTACheck) {
    _lastOTACheck = millis();
    checkFirmwareUpdates();
  }
}

HMSnetwork network;
