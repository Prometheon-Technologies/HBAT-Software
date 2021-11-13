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
DeviceAddress sensorslist[10];

int deviceCount = 0;

int CELLTEMP::freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void CELLTEMP::readAddresses(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        sensorslist[i] = deviceAddress[i];
    }
}

float CELLTEMP::read_temp_sensor_data()
{
    float cell_temp_sensor_results[10];
    for (uint8_t i = 0; i < 10; i++)
    {
        cell_temp_sensor_results[i] = sensors.getTempC(sensorslist[i]);
    }
    return cell_temp_sensor_results;
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