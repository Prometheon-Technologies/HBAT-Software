#include "mdns.hpp"

IPAddress mqttServer;
String mdnsdotlocalurl = "hmshbat";

int HMSmdns::DiscovermDNSBroker()
{
    // check if there is a WiFi connection
    if (WiFi.status() == WL_CONNECTED)
    {
        SERIAL_DEBUG_LN("[mDNS Broker Discovery]: \nconnected!");

        // ######################## Multicast DNS #########################

        SERIAL_DEBUG_ADD("[mDNS Broker Discovery]: Setting up mDNS: ");
        if (!MDNS.begin(mqtt_mDNS_clientId))
        {
            SERIAL_DEBUG_LN("[mDNS Broker Discovery]: [Fail]");
        }
        else
        {
            SERIAL_DEBUG_LN("[mDNS Broker Discovery]: [OK]");
            SERIAL_DEBUG_ADD("Querying MQTT broker: ");

            int n = MDNS.queryService("mqtt", "tcp");

            if (n == 0)
            {
                // No service found
                SERIAL_DEBUG_LN("[mDNS Broker Discovery]: [Fail]");
                return 0;
            }
            else
            {
                int mqttPort;
                // Found one or more MQTT service - use the first one.
                SERIAL_DEBUG_LN("[mDNS Broker Discovery]: [OK]");
                mqttServer = MDNS.IP(0);
                mqttPort = MDNS.port(0);
                heapStr(&(cfg.config.MQTTBroker), mqttServer.toString().c_str());
                SERIAL_DEBUG_ADDF("[mDNS Broker Discovery]: The port is:%d", mqttPort);
                cfg.config.MQTTPort = mqttPort;
                SERIAL_DEBUG_ADD("[mDNS Broker Discovery]: MQTT broker found at: ");
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
void HMSmdns::SetupmDNSServer()
{
    // ######################## Multicast DNS #########################
    SERIAL_DEBUG_ADD("Setting up mDNS: ");
    // Set up mDNS responder:
    // - first argument is the domain name, in this example
    //   the fully-qualified domain name is "esp32.local"
    // - second argument is the IP address to advertise
    //   we send our IP address on the WiFi HMSmdns
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

bool HMSmdns::mDNSLoop()
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