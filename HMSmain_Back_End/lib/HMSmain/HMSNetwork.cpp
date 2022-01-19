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