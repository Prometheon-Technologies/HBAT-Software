#include "HMSnetwork.hpp"

AsyncWebServer server(80);
IPAddress localIP;

WiFiClient espClient;

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "password";
const char *PARAM_INPUT_3 = "apName";
const char *PARAM_INPUT_4 = "apPass";
const char *mdnsPath = "/mdns.txt";
const char *dhcpcheckPath = "/dhcpcheck.txt";
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *ntptimePath = "/ntptime.txt";
const char *ntptimeoffsetPath = "/ntptimeoffset.txt";

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
const long interval = 30000; // interval to wait for Wi-Fi connection (milliseconds)

HMSnetwork::HMSnetwork()
{
    // constructor
    SERIAL_DEBUG_LN("[INFO]: HMSnetwork::HMSnetwork()");
    SERIAL_DEBUG_LN("[INFO]: Creating network object");
}

HMSnetwork::~HMSnetwork()
{
    // destructor
    SERIAL_DEBUG_LN("[INFO]: HMSnetwork::~HMSnetwork()");
    SERIAL_DEBUG_LN("[INFO]: Destroying network object");
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

// create file upload callback
void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        Serial.printf("UploadStart: %s\n", filename.c_str());
        filename = "/" + filename;
        if (!index)
        {
            // SPIFFS.remove(filename);
            Serial.printf("UploadStart: %s\n", filename.c_str());
        }
    }
    if (index + len > 1024)
    {
        Serial.printf("TOO MUCH DATA\n");
        return;
    }
    if (!index)
    {
        Serial.printf("UploadStart: %s\n", filename.c_str());
    }
    File file = SPIFFS.open(filename, "a+");
    if (!file)
    {
        Serial.printf("OPEN FAIL\n");
        return;
    }
    if (file.write(data, len) != len)
    {
        Serial.printf("WRITE FAIL\n");
    }
    file.close();
    Serial.printf("Upload End: %s, %u B\n", filename.c_str(), index + len);
}

bool HMSnetwork::SetupNetworkStack()
{
    cfg.CreateDefaultConfig();
    if (!cfg.loadConfig())
    {
        SERIAL_DEBUG_LN("[INFO]: Failed to load config");
    }
    else
    {
        SERIAL_DEBUG_LN("[INFO]: Loaded config");
        // Load values saved in SPIFFS
        SSID = cfg.config.WIFISSID;
        PASS = cfg.config.WIFIPASS;
        ntptime = cfg.config.NTPTIME;
        ntptimeoffset = cfg.config.NTPTIMEOFFSET;
        mdns = cfg.config.MDNS;
        dhcpcheck = cfg.config.DHCPCHECK;

        if (!PRODUCTION)
        {
            // print it on the serial monitor
            SERIAL_DEBUG_LN(PASS);
        }

        SERIAL_DEBUG_LN(mdns);
        SERIAL_DEBUG_LN(dhcpcheck);
    }

    if (SSID[0] == '\0' || PASS[0] == '\0')
    {
        SERIAL_DEBUG_LN("[INFO]: No SSID or password has been set.");
        SERIAL_DEBUG_LN("[INFO]: Please configure the Wifi Manager by scanning the QR code on your device.");
        SERIAL_DEBUG_LN("");
        return false;
    }
    else
    {
        SERIAL_DEBUG_ADD("[INFO]: Configured SSID: ");
        SERIAL_DEBUG_LN(SSID);
        SERIAL_DEBUG_LN("");

        WiFi.mode(WIFI_STA);
        localIP.fromString(WiFi.localIP().toString());

        if (dhcpcheck == "on")
        {
            SERIAL_DEBUG_LN("[INFO]: DHCP Check is on");
            if (!WiFi.config(localIP, gateway, subnet))
            {
                SERIAL_DEBUG_LN("[INFO]: STA Failed to configure");
                return false;
            }
        }
        else
        {
            SERIAL_DEBUG_LN("[INFO]: DHCP Check is off");
            if (!WiFi.config(localIP, gateway, subnet))
            {
                SERIAL_DEBUG_LN("[INFO]: STA Failed to configure");
                return false;
            }
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
    return 0;
}

void HMSnetwork::SetupWebServer()
{
    if (SetupNetworkStack())
    {
        networkRoutes();
        Serial.println("HBAT HMS server started");
    }
    else
    {
        // TODO: Route for root to  "Please Scan QR code" - Route for Wifi Manager /HBAT_HMS wifi page
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
            SERIAL_DEBUG_ADD("[INFO]: MD5 HASH of MAC ADDRESS: ");
            SERIAL_DEBUG_LN(md5str);
            SERIAL_DEBUG_LN("");
        }

        SERIAL_DEBUG_LN("[INFO]: Configuring access point...");
        WiFi.mode(WIFI_AP);
        WiFi.setTxPower(WIFI_POWER_11dBm);
        // You can remove the password parameter if you want the AP to be open.
        if (!PRODUCTION)
        {
            SERIAL_DEBUG_ADD("Wifi Connection Failed. \r\nStarting AP. \r\nAP IP address: ");
            IPAddress IP = WiFi.softAPIP();
            SERIAL_DEBUG_ADD("[INFO]: AP IP address: ");
            Serial.println(IP);
            SERIAL_DEBUG_LN("");
            WiFi.softAP(WIFI_SSID, WIFI_PASS, 10, 0, 3); // MAC address is used as password for the AP - Unique to each device - MD5 hash of MAC address
        }
        else
        {
            SERIAL_DEBUG_ADD("Wifi Connection Failed. \r\nStarting AP. \r\nAP IP address: ");
            IPAddress IP = WiFi.softAPIP();
            SERIAL_DEBUG_ADD("[INFO]: AP IP address: ");
            Serial.println(IP);
            WiFi.softAP("HMS-WIFI", md5str, 10, 1, 2); // MAC address is used as password for the AP - Unique to each device - MD5 hash of MAC address
        }

        // Give the Memory back to the System if you run the md5 Hash generation in a loop
        free(md5str);

        // free dynamically allocated 16 byte hash from make_hash()
        free(hash);

        networkRoutes();
    }
}

void HMSnetwork::networkRoutes()
{
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/frontend.html", "text/html"); });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/wifimanager", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wifimanager.html", "text/html"); });

    server.serveStatic("/wifimanager", SPIFFS, "/");

    server.on("/upload", HTTP_GET, [&](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "OK"); });

    server.on("/wifimanager", HTTP_POST, [&](AsyncWebServerRequest *request)
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

                                heapStr(&cfg.config.WIFISSID, ssID.c_str());
                                my_delay(100000L);
                            }
                            // HTTP POST pass value
                            if (p->name() == PARAM_INPUT_2) {
                                String passWord; 
                                passWord = p->value().c_str();

                                heapStr(&cfg.config.WIFIPASS, passWord.c_str());
                                my_delay(100000L);
                            }
                            cfg.setConfigChanged();
                            SERIAL_DEBUG_ADDF("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                        }
                    }
                    request->send(200, "application/json", "Done. ESP will restart, connect to your router and go to IP address");
                    my_delay(30000L);
                    ESP.restart(); });

    server.on("/wifiUpdate", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                    int params = request->params();
                    for(int i=0;i<params;i++){
                        AsyncWebParameter* p = request->getParam(i);
                        // HTTP POST ssid value
                        if (p->name() == PARAM_INPUT_1) {
                            String ssID; 
                            ssID = p->value().c_str();
                            SERIAL_DEBUG_ADD("SSID set to: ");
                            SERIAL_DEBUG_LN(ssID);

                            heapStr(&cfg.config.WIFISSID, ssID.c_str());
                            my_delay(100000L);
                        }
                        // HTTP POST pass value
                        if (p->name() == PARAM_INPUT_2) {
                            String passWord; 
                            passWord = p->value().c_str();

                            heapStr(&cfg.config.WIFIPASS, passWord.c_str());
                            my_delay(100000L);
                        }
                        cfg.setConfigChanged();
                        SERIAL_DEBUG_ADDF("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    }
                    request->send(200, "application/json", "Done. ESP will restart and connect to your router");
                    my_delay(30000L);
                    ESP.restart(); });

    server.on("/mqttUpdate", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                  int params = request->params();
                  for(int i=0;i<params;i++){
                      AsyncWebParameter* p = request->getParam(i);
                      if (p->name() == "mqttIP") {
                          String broker;
                          broker = p->value().c_str();
                          SERIAL_DEBUG_ADD("SSID set to: ");
                          SERIAL_DEBUG_LN(broker);
                          heapStr(&cfg.config.MQTTBroker, broker.c_str());
                          my_delay(100000L);
                      }
                      if (p->name() == "mqttName") {
                          String user; 
                          user = p->value().c_str();
                          SERIAL_DEBUG_ADD("SSID set to: ");
                          SERIAL_DEBUG_LN(user);
                          heapStr(&cfg.config.MQTTUser, user.c_str());
                          my_delay(100000L);
                      }
                      if (p->name() == "mqttPass") {
                          String pass; 
                          pass = p->value().c_str();
                          heapStr(&cfg.config.MQTTPass, pass.c_str());
                          my_delay(100000L);
                      }
                      cfg.setConfigChanged();
                      SERIAL_DEBUG_ADDF("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                  }
                  request->send(200, "application/json", "Done. ESP will now connect to your broker");
                  my_delay(30000L);
                  ESP.restart(); });

    // Route to set GPIO state to LOW
    server.on("/toggle", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                    int params = request->params();
                    for(int i=0;i<params;i++){
                        AsyncWebParameter* p = request->getParam(i);
                            // HTTP POST Relay Value
                        if (p->name() == "pin") {
                            String relay = p->value().c_str();
                            Serial.print("switching state of pin:");
                            Serial.println(relay);
                            cfg.config.relays[relay.toInt()] = (cfg.config.relays[relay.toInt()] == true) ? false : true;
                        }
                        /* cfg.setConfigChanged(); */
                        SERIAL_DEBUG_ADDF("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    }
                    request->send(200, "application/json", "toggled"); });

    server.on("/mqttEnable", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                    int params = request->params();
                    for(int i=0;i<params;i++){
                        AsyncWebParameter* p = request->getParam(i);
                            // HTTP POST Relay Value
                        if (p->name() == "mqttEnableState") {
                            bool state = p->value().c_str();
                            Serial.print("switching state of pin:");
                            Serial.println(state);
                            cfg.config.MQTTEnabled = (state == true) ? false : true;
                        }
                        /* cfg.setConfigChanged(); */
                        SERIAL_DEBUG_ADDF("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    }
                    request->send(200, "application/json", (cfg.config.MQTTEnabled == true) ?  "MQTT Disabled" : "MQTT Enabled"); });

    server.on("/data.json", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                    cfg.config.data_json = true;
                    my_delay(10000L);
                    String temp = cfg.config.data_json_string;
                    request->send(200, "application/json", temp);
                    temp = ""; });

    server.onNotFound(notFound);
    server.onFileUpload(onUpload);
    server.begin();
}

// ######################## server functions #########################

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
        Serial.println(WiFi.localIP());
        Serial.print("[INFO]:  in your browser");
    }
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
    cfg.CreateDefaultConfig();
    if (!cfg.loadConfig())
    {
        SERIAL_DEBUG_LN("[INFO]: Failed to load config");
    }
    else
    {
        SERIAL_DEBUG_LN("[INFO]: Loaded config");
        // Load values saved in SPIFFS
        SSID = cfg.config.WIFISSID;
        PASS = cfg.config.WIFIPASS;
        ntptime = cfg.config.NTPTIME;
        ntptimeoffset = cfg.config.NTPTIMEOFFSET;
        mdns = cfg.config.MDNS;
        dhcpcheck = cfg.config.DHCPCHECK;

        if (!PRODUCTION)
        {
            // print it on the serial monitor
            SERIAL_DEBUG_LN(PASS);
        }

        SERIAL_DEBUG_LN(mdns);
        SERIAL_DEBUG_LN(dhcpcheck);
    }

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
            WiFi.softAP(WIFI_SSID, WIFI_PASS, 10, 1, 2);
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

void HMSnetwork::SetupServer()
{
    SERIAL_DEBUG_EOL;
    SERIAL_DEBUG_LN(F("System Information:"));
    SERIAL_DEBUG_EOL;
    SERIAL_DEBUG_LNF("PLatformI0 compile time: %s (%s)", __DATE__, __TIME__);
    SERIAL_DEBUG_LNF("PLatformI0 Unix compile time: %d", COMPILE_UNIX_TIME);
    SERIAL_DEBUG_LNF("Project directory: %s", PROJECT_PATH);
    SERIAL_DEBUG_LNF("Version: %d", VERSION);
    SERIAL_DEBUG_LNF("Heap: %d", ESP.getFreeHeap());
    SERIAL_DEBUG_LNF("SDK: %s", ESP.getSdkVersion());
    SERIAL_DEBUG_LNF("MAC address: %s", WiFi.macAddress().c_str());
    SERIAL_DEBUG_LNF("CPU Speed: %dMHz", ESP.getCpuFreqMHz());
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
        SERIAL_DEBUG_LNF("FS File: %s, size: %i", file.name(), file.size());
        file = root.openNextFile();
    }
    SERIAL_DEBUG_EOL;
    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();
    if (usedBytes == 0)
    {
        SERIAL_DEBUG_LN(F("NO WEB SERVER FILES PRESENT: \n"));
    }
    SERIAL_DEBUG_LNF("FS Size: %iKB, used: %iKB, %0.2f%%",
                     totalBytes, usedBytes,
                     (float)100 / totalBytes * usedBytes);
    SERIAL_DEBUG_EOL;
#endif
#endif
}

void HMSnetwork::SetupWifiScan()
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Disconnect from the access point if connected before
    my_delay(100000L);

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
            delay(10000L);
            return true;
        }
    }
    SERIAL_DEBUG_LN("[INFO]: ");

    // Wait a bit before scanning again
    my_delay(5000000L);
    return true;
}

HMSnetwork network;
