#include <HMSmqtt.h>

FrontEnd Front_End = FrontEnd();

HMSmqtt::HMSmqtt()
{
}

HMSmqtt::~HMSmqtt(void)
{
}

int HMSmqtt::CheckWifiState()
{
    int wifiStateCounter = 0;
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(". ");
        delay(1000);
        wifiStateCounter++;
        if (wifiStateCounter > 100)
            return 0;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nconnected!");
        return 1;
    }
    return 0;
}

void HMSmqtt::MQTTConnect()
{
    // float *cell_voltage = (float *)malloc(sizeof(float) * 10);
    const char *clientPass = (char *)malloc(sizeof(char));
    const char *clientUser = (char *)malloc(sizeof(char));
    clientPass = Front_End.NewMQTTPass.c_str();
    clientUser = Front_End.NewMQTTUser.c_str();
    CheckWifiState();
    if (CheckWifiState() == 1)
    {
        Serial.print("\nconnecting to MQTT Broker...");
        while (!mqtt.connect(&client, clientUser, clientPass))
        {
            Serial.print(". ");
            delay(1000);
        }
        Serial.println("\nconnected!");
    }
}

int HMSmqtt::ReConnect()
{
    // reconnect failover
    if (!mqtt.connected())
    {
        MQTTConnect();
        return 0;
    }
    return 1;
}

void MessageReceived(String &topic, String &payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the mqtt in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `mqtt.loop()`.
}

void HMSmqtt::MQTTSetup()
{
    // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
    // by Arduino. You need to set the IP address directly.
    //
    // MQTT brokers usually use port 8883 for secure connections.
    mqtt.begin(clientIP, 8883, net);
    mqtt.onMessage(MessageReceived);

    MQTTConnect();
}

void HMSmqtt::MQTTPublish(String topic, String payload)
{
    if (MQTTLoop() == 1)
    {
        mqtt.publish(topic, payload);
    }
    else
    {
        Serial.println("MQTT not connected, not publishing. Attempting to reconnect...");
        Serial.print(". ");
    }
}

int HMSmqtt::MQTTLoop()
{
    mqtt.loop();
    if (!ReConnect() == 0)
    {
        // reconnect if connection dropped
        ReConnect();
        return 0;
    }
    else
    {
        return 1;
        delay(10); // <- fixes some issues with WiFi stability
    }
}
