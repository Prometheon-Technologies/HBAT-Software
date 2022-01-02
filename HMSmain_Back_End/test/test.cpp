#include <Arduino.h>
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