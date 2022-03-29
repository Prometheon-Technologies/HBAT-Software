#include "config.hpp"

// save last "timestamp" the config has been saved


Config::Config()
{
    last_config_change = false;
    last_config = 0;
    maxVoltage = 24;
    maxTemp = 120;
    doc_string = "";
}

Config::~Config()
{
    /* // Free the allocated memory
    if (config.hostname)
    {
        freeStr(&config.hostname);
    }
    if (config.MQTTBroker)
    {
        freeStr(&config.MQTTBroker);
    }
    if (config.MQTTPort)
    {
        freeStr(&config.MQTTPort);
    }
    if (config.MQTTUser)
    {
        freeStr(&config.MQTTUser);
    }
    if (config.MQTTPass)
    {
        freeStr(&config.MQTTPass);
    }
    if (config.MQTTTopic)
    {
        freeStr(&config.MQTTTopic);
    }
    if (config.MQTTSetTopic)
    {
        freeStr(&config.MQTTSetTopic);
    }
    if (config.MQTTDeviceName)
    {
        freeStr(&config.MQTTDeviceName);
    }
    if (config.NTPTIME)
    {
        freeStr(&config.NTPTIME);
    }
    if (config.NTPTIMEOFFSET)
    {
        freeStr(&config.NTPTIMEOFFSET);
    }
    if (config.WIFISSID)
    {
        freeStr(&config.WIFISSID);
    }
    if (config.WIFIPASS)
    {
        freeStr(&config.WIFIPASS);
    }
    if (config.MDNS)
    {
        freeStr(&config.MDNS);
    }
    if (config.DHCPCHECK)
    {
        freeStr(&config.DHCPCHECK);
    }
    if (config.configData)
    {
        freeStr(&config.configData);
    } */
}

void Config::CreateDefaultConfig()
{
    config.hostname = NULL;
    config.MQTTEnabled = 0;
    config.MQTTPort = NULL;     // Port to use for unsecured MQTT
    config.MQTTPort_Secure = 0; // port to use if Secured MQTT is enabled
    config.MQTTUser = NULL;
    config.MQTTPass = NULL;
    config.MQTTTopic = NULL;
    config.MQTTSetTopic = NULL;
    config.MQTTDeviceName = NULL;
    config.last_mqtt_connect_attempt = 0;
    config.last_mqtt_publish_attempt = 0;
    config.lastMillis = 0;
    config.clientIP = NULL;
    config.MQTTSecureState = false;
    config.MQTTBroker = NULL;
    config.lastMsg = 0;
    config.msg = NULL;
    config.value = 0;
    config.WIFISSID = NULL;
    config.WIFIPASS = NULL;
    config.MQTTConnectedState = false;
    config.configData = NULL;
    config.NTPTIME = NULL;
    config.NTPTIMEOFFSET = NULL;
    config.MDNS = NULL;
    config.DHCPCHECK = NULL;

    for (int i = 0; i < 5; i++)
    {
        config.relays[i] = false;
    }

    config.stack_humidity = 0;
    config.stack_temp = 0;
    config.stack_voltage = 0;
    config.stack_current = 0;

    for (int i = 0; i < 10; i++)
    {
        config.cell_temp[i] = 0;
        config.cell_voltage[i] = 0;
    }
}

// Initialize SPIFFS
bool Config::initSPIFFS()
{
    if (!SPIFFS.begin(false))
    {
        SERIAL_DEBUG_LN("An error has occurred while mounting SPIFFS");
        return false;
    }
    SERIAL_DEBUG_LN("SPIFFS mounted successfully");
    return true;
}

// Read File from SPIFFS
String Config::readFile(fs::FS &fs, const char *path)
{
    SERIAL_DEBUG_ADDF("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        SERIAL_DEBUG_LN("[INFO]: Failed to open file for reading");
        return String();
    }

    String fileContent;
    while (file.available())
    {
        fileContent = file.readStringUntil('\n');
        break;
    }
    return fileContent;
}

// Write file to SPIFFS
void Config::writeFile(fs::FS &fs, const char *path, const char *message)
{
    SERIAL_DEBUG_ADDF("[Writing File]: Writing file: %s\r\n", path);
    delay(10);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        SERIAL_DEBUG_LN("[Writing File]: failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        SERIAL_DEBUG_LN("[Writing File]: file written");
    }
    else
    {
        SERIAL_DEBUG_LN("[Writing File]: file write failed");
    }
}

bool Config::loadConfig()
{
    SERIAL_DEBUG_LN("[Load config]: Loading Config File");

    // load the config file
    initSPIFFS();
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        SERIAL_DEBUG_LN("[Load config]: Failed to open config file");
        CreateDefaultConfig();
        return false;
    }

    size_t size = configFile.size();
    Serial.print("[Load config]: Config file size: ");
    SERIAL_DEBUG_LN(size);

    if (size > 1024)
    {
        SERIAL_DEBUG_LN("[Load config]: Config file size is too large");
        CreateDefaultConfig();
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);
    // We don't use String here because ArduinoJson library requires the input
    // to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);
    SERIAL_DEBUG_LN(F("[Load config]: Config file content:"));
    SERIAL_DEBUG_LN(buf.get());

    // Parse the buffer into an object

    StaticJsonDocument<1024> jsonBuffer;
    // Deserialize the JSON document
    auto error = deserializeJson(jsonBuffer, buf.get());
    if (error)
    {
        SERIAL_DEBUG_LN(F("[Load config]: Failed to parse config file"));
        Serial.print(F("[Load config]: deserializeJson() failed with code "));
        SERIAL_DEBUG_LN(error.c_str());
        CreateDefaultConfig();
        return false;
    }

    heapStr(&config.hostname, jsonBuffer["hostname"]);
    config.MQTTEnabled = jsonBuffer["MQTTEnabled"];
    heapStr(&config.MQTTPort, jsonBuffer["MQTTPort"]);
    config.MQTTPort_Secure = jsonBuffer["MQTTPort_Secure"];
    heapStr(&config.MQTTUser, jsonBuffer["MQTTUser"]);
    heapStr(&config.MQTTPass, jsonBuffer["MQTTPass"]);
    heapStr(&config.MQTTTopic, jsonBuffer["MQTTTopic"]);
    heapStr(&config.MQTTSetTopic, jsonBuffer["MQTTSetTopic"]);
    heapStr(&config.MQTTDeviceName, jsonBuffer["MQTTDeviceName"]);
    config.last_mqtt_connect_attempt = jsonBuffer["last_mqtt_connect_attempt"];
    config.last_mqtt_publish_attempt = jsonBuffer["last_mqtt_publish_attempt"];
    config.lastMillis = jsonBuffer["lastMillis"];
    heapStr(&config.clientIP, jsonBuffer["clientIP"]);
    config.MQTTSecureState = jsonBuffer["MQTTSecureState"];
    heapStr(&config.MQTTBroker, jsonBuffer["MQTTBroker"]);
    config.lastMsg = jsonBuffer["lastMsg"];
    heapStr(&config.msg, jsonBuffer["msg"]);
    config.value = jsonBuffer["value"];
    heapStr(&config.WIFISSID, jsonBuffer["WIFISSID"]);
    heapStr(&config.WIFIPASS, jsonBuffer["WIFIPASS"]);
    config.MQTTConnectedState = jsonBuffer["MQTTConnectedState"];
    heapStr(&config.configData, jsonBuffer["configData"]);
    heapStr(&config.NTPTIME, jsonBuffer["NTPTIME"]);
    heapStr(&config.NTPTIMEOFFSET, jsonBuffer["NTPTIMEOFFSET"]);
    heapStr(&config.MDNS, jsonBuffer["MDNS"]);
    heapStr(&config.DHCPCHECK, jsonBuffer["DHCPCHECK"]);

    for (int i = 0; i < sizeof(config.relays); i++)
    {
        config.relays[i] = jsonBuffer["relays"][i];
    }

    return true;
}

// trigger a config write/commit
bool Config::setConfigChanged()
{
    last_config_change = true;
    SERIAL_DEBUG_LN(F("[Set Config Changed]: Config save set to true"));
    return true;
}

bool Config::saveConfig()
{
    // check if the data in config is different from the data in the file
    if (!last_config_change)
    {
        SERIAL_DEBUG_LN(F("[Save Config Changes]: Config has not changed because it is the same as the file"));
        return false;
    }

    SERIAL_DEBUG_LN(F("[Save Config Changes]: Saving Config"));

    // create a json file from the config struct and save it using SPIFFs
    SERIAL_DEBUG_LN(F("[Save Config Changes]: Writing config"));
    // create a json file from the config struct
    StaticJsonDocument<1024> jsonConfig;
    JsonObject json = jsonConfig.to<JsonObject>();

    // create a json file from the config struct
    json["hostname"] = config.hostname;
    json["MQTTEnabled"] = config.MQTTEnabled;
    json["MQTTPort"] = config.MQTTPort;
    json["MQTTPort_Secure"] = config.MQTTPort_Secure;
    json["MQTTUser"] = config.MQTTUser;
    json["MQTTPass"] = config.MQTTPass;
    json["MQTTTopic"] = config.MQTTTopic;
    json["MQTTSetTopic"] = config.MQTTSetTopic;
    json["MQTTDeviceName"] = config.MQTTDeviceName;
    json["last_mqtt_connect_attempt"] = config.last_mqtt_connect_attempt;
    json["last_mqtt_publish_attempt"] = config.last_mqtt_publish_attempt;
    json["lastMillis"] = config.lastMillis;
    json["clientIP"] = config.clientIP;
    json["MQTTSecureState"] = config.MQTTSecureState;
    json["MQTTBroker"] = config.MQTTBroker;
    json["lastMsg"] = config.lastMsg;
    json["msg"] = config.msg;
    json["value"] = config.value;
    json["WIFISSID"] = config.WIFISSID;
    json["WIFIPASS"] = config.WIFIPASS;
    json["MQTTConnectedState"] = config.MQTTConnectedState;
    json["configData"] = config.configData;
    json["NTPTIME"] = config.NTPTIME;
    json["NTPTIMEOFFSET"] = config.NTPTIMEOFFSET;
    json["MDNS"] = config.MDNS;
    json["DHCPCHECK"] = config.DHCPCHECK;
    json["Number of Sensors"] = config.numSensors;
    json["Max Cell Count"] = config.cell_count_max;

    // Relays
    JsonArray Relays = json.createNestedArray("HMS_Relays_State");
    for (int i = 0; i < sizeof(config.relays); i++)
    {
        Relays.add(config.relays[i]);
    }

    // Set the values in the document
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        SERIAL_DEBUG_LN(F("[Save Config Changes]: Failed to open config file for writing"));
        return false;
    }
    if (serializeJson(json, configFile) == 0)
    {
        SERIAL_DEBUG_LN(F("[Save Config Changes]: Failed to write to file"));
        return false;
    }

    configFile.print("\r\n");
    configFile.close();
    // end save
    SERIAL_DEBUG_LN(F("[Save Config Changes]: Config written"));
    last_config_change = false;

    /* if (!PRODUCTION)
    {
        SERIAL_DEBUG_EOL(serializeJson(json, Serial));
        doc_string = String(json);
        if (doc_string.length() > 0)
        {
            SERIAL_DEBUG_LN(doc_string);
        }
    } */

    return true;
}

bool Config::updateCurrentData()
{
    // check if the data in config is different from the data in the file
    if (!last_config_change)
    {
        SERIAL_DEBUG_LN(F("[Update Current Data]: Config has not changed because it is the same as the file"));
        return false;
    }
    SERIAL_DEBUG_LN(F("[Update Current Data]: Updating Config"));
    // call to save config if config has changed
    saveConfig();
    SERIAL_DEBUG_LNF("[INFO - FREE HEAP SIZE]: Heap: %d", ESP.getFreeHeap());
    return true;
}

// overwrite all config settings with "0"
void Config::resetConfig()
{
    CreateDefaultConfig();
    setConfigChanged();
}

bool Config::isValidHostname(char *hostname_to_check, long size)
{
    for (int i = 0; i < size; i++)
    {
        if (hostname_to_check[i] == '-' || hostname_to_check[i] == '.')
            continue;
        else if (hostname_to_check[i] >= '0' && hostname_to_check[i] <= '9')
            continue;
        else if (hostname_to_check[i] >= 'A' && hostname_to_check[i] <= 'Z')
            continue;
        else if (hostname_to_check[i] >= 'a' && hostname_to_check[i] <= 'z')
            continue;
        else if (hostname_to_check[i] == 0 && i > 0)
            break;

        return false;
    }

    return true;
}

// parse and set a new hostname to config
void Config::setHostname(String new_hostname)
{
    int j = 0;
    for (unsigned int i = 0; i < new_hostname.length() && i < sizeof(config.hostname); i++)
    {
        if (new_hostname.charAt(i) == '-' or
            (new_hostname.charAt(i) >= '0' && new_hostname.charAt(i) <= '9') or
            (new_hostname.charAt(i) >= 'A' && new_hostname.charAt(i) <= 'Z') or
            (new_hostname.charAt(i) >= 'a' && new_hostname.charAt(i) <= 'z'))
        {

            config.hostname[j] = new_hostname.charAt(i);
            j++;
        }
    }
    config.hostname[j] = '\0';
    setConfigChanged();
}

// we can't assing wifiManager.resetSettings(); to reset, somehow it gets called straight away.
void Config::setWiFiConf(String ssid, String password)
{
    /* #if defined(ESP32)
        if (WiFiGenericClass::getMode() != WIFI_MODE_NULL)
        {

            wifi_config_t conf;
            esp_wifi_get_config(WIFI_IF_STA, &conf);

            memset(conf.sta.ssid, 0, sizeof(conf.sta.ssid));
            for (int i = 0; i < ssid.length() && i < sizeof(conf.sta.ssid); i++)
                conf.sta.ssid[i] = ssid.charAt(i);

            memset(conf.sta.password, 0, sizeof(conf.sta.password));
            for (int i = 0; i < password.length() && i < sizeof(conf.sta.password); i++)
                conf.sta.password[i] = password.charAt(i);

            esp_wifi_set_config(WIFI_IF_STA, &conf);
        }
    #endif */
}

Config cfg;
