#include <HMSNetwork.h>

HMSNetwork::HMSNetwork()
{
}

HMSNetwork::~HMSNetwork()
{
}

// ######################## server functions #########################

String HMSNetwork::getRebootString()
{
    return "<html><head><meta http-equiv=\"refresh\" content=\"4; url=/\"/></head><body><font face='arial'><b><h2>Rebooting... returning in 4 seconds</h2></b></font></body></html>";
}

void HMSNetwork::handleReboot()
{
    server.send(200, "text/html", getRebootString());
    delay(500);
    ESP.restart();
}

void HMSNetwork::HMSNetworkSetup()
{
    // setting up Wifi
    String macID = WiFi.macAddress().substring(12, 14) + WiFi.macAddress().substring(15, 17);

    // String macIDUpper = macID.toUpperCase();

    String nameString = String(cfg.hostname) + String(" - ") + macID;

    char nameChar[nameString.length() + 1];
    nameString.toCharArray(nameChar, sizeof(nameChar));

    // setup wifiManager
    wifiManager.setHostname(cfg.hostname);           // set hostname
    wifiManager.setConfigPortalBlocking(false);      // config portal is not blocking (LEDs light up in AP mode)
    wifiManager.setSaveConfigCallback(handleReboot()); // after the wireless settings have been saved a reboot will be performed

#if HMS_DEBUG != 0
    wifiManager.setDebugOutput(true);
#else
    wifiManager.setDebugOutput(false);
#endif

    // automatically connect using saved credentials if they exist
    // If connection fails it starts an access point with the specified name
    if (wifiManager.autoConnect(nameChar))
    {
        SERIAL_DEBUG_LN("INFO: Wi-Fi connected");
    }
    else
    {
        Serial.printf("INFO: Wi-Fi manager portal running. Connect to the Wi-Fi AP '%s' to configure your wireless connection\n", nameChar);
        wifiMangerPortalRunning = true;
    }
}

void HMSNetwork::HMSNetworkLoop()
{
    // check if there is a WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        SERIAL_DEBUG_LN("INFO: WiFi disconnected");
        return;
    }
}

void HMSNetwork::ClientLoop()
{
  server.handleClient();
  delay(0);
}

void HMSNetwork::connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS)
{
  WiFi.persistent(true);
  SERIAL_DEBUG_LN("Configuring access point...");
  Serial.print("WIFI_STA_SSID:");
  SERIAL_DEBUG_LN(WIFI_STA_SSID);
  Serial.print("WIFI_STA_PASS:");
  SERIAL_DEBUG_LN(WIFI_STA_PASS);

  WiFi.mode(WIFI_STA);
  if (WIFI_STA_SSID == "")
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(WIFI_STA_SSID.c_str(), WIFI_STA_PASS.c_str());
  }

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    numberOfAttempts++;
    delay(200);
    Serial.print(".");
    if (numberOfAttempts > 100)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      WiFi.softAP(ssid.c_str(), password.c_str());
      Serial.print("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      SERIAL_DEBUG_LN(WiFi.softAPIP());
      return;
      break;
    }
  }
  SERIAL_DEBUG_LN("");
  Serial.print("Connected! IP address: ");
  SERIAL_DEBUG_LN(WiFi.localIP());
}