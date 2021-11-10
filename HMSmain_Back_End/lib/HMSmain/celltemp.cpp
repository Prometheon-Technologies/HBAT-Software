#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <celltemp.h>

// Data wire is plugged into port 42 on the ESP32
#define ONE_WIRE_BUS 42
#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#endif

CELLTEMP::CELLTEMP()
{
}

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// variable to hold device addresses
DeviceAddress Thermometer;

// Placeholder Addresses of 3 DS18B20s - CHANGE THESE WITH DEVICE ADDRESSES PRINTED TO CONSOLE from printAddress()
uint8_t sensor1[8] = {0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC};
uint8_t sensor2[8] = {0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27};
uint8_t sensor3[8] = {0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6};
uint8_t sensor4[8] = {0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC};
uint8_t sensor5[8] = {0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27};
uint8_t sensor6[8] = {0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6};
uint8_t sensor7[8] = {0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6};
uint8_t sensor8[8] = {0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC};
uint8_t sensor9[8] = {0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27};
uint8_t sensor10[8] = {0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6};

int deviceCount = 0;

int CELLTEMP::freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void CELLTEMP::printAddress(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        debug("0x");
        if (deviceAddress[i] < 0x10)
            debug("0");
        Serial.print(deviceAddress[i], HEX);
        if (i < 7)
            debug(", ");
    }
    debugln("");
}

void CELLTEMP::printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    debug(tempC);
    debug((char)176);
    debug("C  |  ");
    debug(DallasTemperature::toFahrenheit(tempC));
    debug((char)176);
    debugln("F");
}

void CELLTEMP::read_temp_sensor_data()
{
    sensors.requestTemperatures();

    debug("Sensor 1: ");
    printTemperature(sensor1);

    debug("Sensor 2: ");
    printTemperature(sensor2);

    debug("Sensor 3: ");
    printTemperature(sensor3);

    debug("Sensor 4: ");
    printTemperature(sensor4);

    debug("Sensor 5: ");
    printTemperature(sensor5);

    debug("Sensor 6: ");
    printTemperature(sensor6);

    debug("Sensor 7: ");
    printTemperature(sensor7);

    debug("Sensor 8: ");
    printTemperature(sensor8);

    debug("Sensor 9: ");
    printTemperature(sensor9);

    debug("Sensor 10: ");
    printTemperature(sensor10);

    debugln();
    debug("Free Memory: " + freeRam());
    delay(1000);
}

void CELLTEMP::setup_sensors()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10); // will pause until serial console opens

    // Start up the ds18b20 library
    sensors.begin();
    // locate devices on the bus
    debugln("Locating devices...");
    debug("Found ");
    deviceCount = sensors.getDeviceCount();
    Serial.print(deviceCount, DEC);
    debugln(" devices.");
    debugln("");
    debugln("Printing addresses...");
    for (int i = 0; i < deviceCount; i++)
    {
        debug("Sensor ");
        debug(i + 1);
        debug(" : ");
        sensors.getAddress(Thermometer, i);
        printAddress(Thermometer);
    }
}