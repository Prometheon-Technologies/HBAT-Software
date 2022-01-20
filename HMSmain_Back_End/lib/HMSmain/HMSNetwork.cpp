#include <HMSNetwork.h>

HMSNetwork::HMSNetwork()
{
}

HMSNetwork::~HMSNetwork()
{
}

// ######################## server functions #########################
void HMSNetwork::HMSNetworkSetup()
{
  // ######################## server setup #########################
  SERIAL_DEBUG_ADD("\nStarting " + MQTT_DEVICE_NAME + " on " + String(ARDUINO_BOARD));
  SERIAL_DEBUG_LN(ESP_ASYNC_WIFIMANAGER_VERSION);
#if (USING_ESP32_S2 || USING_ESP32_C3)
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, NULL, "Async_AutoConnect");
#else
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, "Async_AutoConnect");
#endif

  // ESPAsync_wifiManager.resetSettings();   //reset saved settings
  // ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 132, 1), IPAddress(192, 168, 132, 1), IPAddress(255, 255, 255, 0));
  ESPAsync_wifiManager.autoConnect(cfg.WIFISSID);
  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_ADD(F("Connected. Local IP: "));
    SERIAL_DEBUG_LN(WiFi.localIP());
  }
  else
  {
    SERIAL_DEBUG_LN(ESPAsync_wifiManager.getStatus(WiFi.status()));
    CheckWifiState();
  }

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/plain", "Hi! I am MyESP32S."); });

  AsyncElegantOTA.begin(&webServer); // Start ElegantOTA
  webServer.begin();
  SERIAL_DEBUG_LN("HTTP server started");
}

int HMSNetwork::CheckWifiState()
{
  // check if there is a WiFi connection
  int wifiStateCounter = 0;
  SERIAL_DEBUG_ADD("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    SERIAL_DEBUG_ADD(". ");
    delay(1000);
    wifiStateCounter++;
    if (wifiStateCounter > 100)
      SERIAL_DEBUG_LN("WiFi not connected");
    return 0;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    SERIAL_DEBUG_LN("\nconnected!");
    return 1;
  }
  return 0;
}

/******************************************************************************
 * Function: Check Network Connection Loop
 * Description: This function checks the network connection and reconnects if necessary - is called in the loop() function every 5 seconds
 * Parameters: None
 * Return: None
 ******************************************************************************/
void HMSNetwork::CheckNetworkLoop()
{
  static unsigned int loop_counter = 0;

  // run current function every 5 seconds
  if (ReadTimer_2.ding())
  {
    int currentWifiStatus = CheckWifiState();

    if (currentWifiStatus != 1)
    {
      wifiConnected = false;
      SERIAL_DEBUG_LN(F("Wifi is not connected"));
    }
    else
    {
      SERIAL_DEBUG_LN(F("Wifi is connected and not connected to a network"));
    }
    if (currentWifiStatus == WL_CONNECTED && !wifiConnected)
    {
      wifiConnected = true;
      Serial.print("INFO: WiFi Connected! Open http://");
      Serial.print(WiFi.localIP());
      SERIAL_DEBUG_LN(" in your browser");
    }
  }
  loop_counter++;
  ReadTimer_2.start();
}

int HMSNetwork::DiscovermDNSBroker()
{
  // ######################## Multicast DNS #########################
  Serial.print("Setting up mDNS: ");
  if (!MDNS.begin(mqtt_mDNS_clientId))
  {
    Serial.println("[Fail]");
  }
  else
  {
    Serial.println("[OK]");
    Serial.print("Querying MQTT broker: ");

    int n = MDNS.queryService("mqtt", "tcp");

    if (n == 0)
    {
      // No service found
      Serial.println("[Fail]");
      return 0;
    }
    else
    {
      // Found one or more MQTT service - use the first one.
      Serial.println("[OK]");
      mqttServer = MDNS.IP(0);
      mqttPort = MDNS.port(0);
      cfg.MQTTHost = mqttServer;
      cfg.MQTTPort = mqttPort;
      Serial.print("MQTT broker found at: ");
      Serial.print(mqttServer);
      Serial.print(":");
      Serial.println(mqttPort);
      return 1;
    }
  }
}

void HMSNetwork::SetupmDNSServer()
{
  // ######################## Multicast DNS #########################
  Serial.print("Setting up mDNS: ");
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(mDNS_hostname))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  webServer.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void HMSNetwork::SetupmDNSLoop()
{
  // ######################## Multicast DNS #########################
  // Check if a new client has connected
  WiFiClient client;
  int wifi_status = CheckWifiState();
  if (wifi_status == 1)
  {
    if (!client)
    {
      return;
    }
    // Wait for data from client to become available
    while (!client.available())
    {
      delay(1);
    }

    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');

    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1)
    {
      Serial.print("Invalid request: ");
      Serial.println(req);
      return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);

    String s;
    if (req == "/")
    {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
      s += ipStr;
      s += "</html>\r\n\r\n";
      Serial.println("Sending 200");
    }
    else
    {
      s = "HTTP/1.1 404 Not Found\r\n\r\n";
      Serial.println("Sending 404");
    }
    client.print(s);

    client.stop();
    Serial.println("Done with client");
  }
  else
  {
    return;
  }
}

/******************************************************************************
 * Function: Connect to Wifi with fail to AP mode if no connection
 * Description: This connection will attempt to create a WiFi connection with the given SSID and password. If the connection fails, it will attempt to connect to a default Wifi AP.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void HMSNetwork::connectToApWithFailToStation()
{
  WiFi.persistent(true);
  SERIAL_DEBUG_LN("Configuring access point...");
  SERIAL_DEBUG_ADD("cfg.WIFISSID:");
  SERIAL_DEBUG_LN(cfg.WIFISSID);
  SERIAL_DEBUG_ADD("cfg.WIFIPASS:");
  SERIAL_DEBUG_LN(cfg.WIFIPASS);

  WiFi.mode(WIFI_STA);
  if (cfg.WIFISSID == "")
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(cfg.WIFISSID, cfg.WIFIPASS);
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
      WiFi.softAP(cfg.WIFISSID, cfg.WIFIPASS);
      SERIAL_DEBUG_ADD("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      SERIAL_DEBUG_LN(WiFi.softAPIP());
      return;
      break;
    }
  }
  SERIAL_DEBUG_LN("");
  SERIAL_DEBUG_LN("Connected! IP address: ");
  SERIAL_DEBUG_LN(WiFi.localIP());
}