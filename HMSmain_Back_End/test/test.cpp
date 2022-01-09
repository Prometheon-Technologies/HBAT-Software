#include <Arduino.h>
#include <defines.h>
#define MQTT_TOPIC "HMS/data" 
void print()
{
    String currentTopic = MQTT_TOPIC + '//' + HMSmain.getDeviceID();
    Serial.print(currentTopic);
}
void setup() {
    Serial.begin(115200);

}

void loop() {
    print();
    delay(1000);
}

void HMSNetworkSetup()
{
    // setting up Wifi
    String macID = WiFi.macAddress().substring(12, 14) + WiFi.macAddress().substring(15, 17);

    //String macIDUpper = macID.toUpperCase();

    String nameString = String(cfg.hostname) + String(" - ") + macID;

    char nameChar[nameString.length() + 1];
    nameString.toCharArray(nameChar, sizeof(nameChar));
}
