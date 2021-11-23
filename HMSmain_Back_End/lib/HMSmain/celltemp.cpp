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
    // Start up the ds18b20 library
    sensors.begin();

    // Grab a count of devices on the wire
    sensors_count = sensors.getDeviceCount();

    // locate devices on the bus
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(sensors_count, DEC);
    Serial.println(" devices.");

    // Loop through each device, print out address
    for (int i = 0; i < sensors_count; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(temp_sensor_addresses, i))
        {
            Serial.print("Found device ");
            Serial.print(i, DEC);
            Serial.print(" with address: ");
            printAddress(temp_sensor_addresses);
            Serial.println();
        }
        else
        {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.print(" but could not detect address. Check power and cabling");
        }
    }
}

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// variable to hold device addresses
int sensors_count;
DeviceAddress temp_sensor_addresses;

int CELLTEMP::freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// function to print a device address
void CELLTEMP::printAddress(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < sensors_count; i++)
    {
        if (deviceAddress[i] < 16)
            Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}

int CELLTEMP::read_temp_sensor_data()
{
    int cell_temp_sensor_results[10];
    for (int i = 0; i < sensors_count; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(temp_sensor_addresses, i))
        {
            cell_temp_sensor_results[i] = sensors.getTempC(temp_sensor_addresses);
            printAddress(temp_sensor_addresses);
            Serial.println();
        }
        else
        {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.print(" but could not detect address. Check power and cabling");
        }
    }
}