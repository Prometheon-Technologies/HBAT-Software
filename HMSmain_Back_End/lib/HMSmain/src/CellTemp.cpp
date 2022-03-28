#include "CellTemp.hpp"

CellTemp::CellTemp()
{
    sensors_count = sensors.getDeviceCount();
}

CellTemp::~CellTemp()
{
}

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// variable to hold device addresses

DeviceAddress temp_sensor_addresses;

/******************************************************************************
 * Function: Setup DS18B20 sensors
 * Description: Setup DS18B20 sensors by beginning the Dallas Temperature library and counting the connected sensors
 * Parameters: None
 * Return: None
 ******************************************************************************/
void CellTemp::SetupSensors()
{
    // Start up the ds18b20 library
    sensors.begin();

    // locate devices on the bus
    SERIAL_DEBUG_LN("Locating devices...");
    SERIAL_DEBUG_LN("Found ");
    Serial.print(sensors_count, DEC);
    SERIAL_DEBUG_LN(" devices.");

    // Loop through each device, print out address
    for (int i = 0; i < sensors_count; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(temp_sensor_addresses, i))
        {
            SERIAL_DEBUG_LN("Found device ");
            Serial.print(i, DEC);
            SERIAL_DEBUG_LN(" with address: ");
            printAddress(temp_sensor_addresses);
            SERIAL_DEBUG_EOL("");
        }
        else
        {
            SERIAL_DEBUG_LN("Found ghost device at ");
            Serial.print(i, DEC);
            SERIAL_DEBUG_LN(" but could not detect address. Check power and cabling");
        }
    }
}

/******************************************************************************
 * Function: Print Address
 * Description: Print the addresses of the sensors
 * Parameters: DeviceAddress - Address of the sensor
 * Return: None
 ******************************************************************************/
// function to print a device address
void CellTemp::printAddress(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < sensors_count; i++)
    {
        if (deviceAddress[i] < 16)
            SERIAL_DEBUG_LN("0");
        Serial.print(deviceAddress[i], HEX);
    }
}

/******************************************************************************
 * Function: Get Temperature
 * Description: Get the temperatures of the sensors and allocate the memory for the temperatures
 * Parameters: None
 * Return: float array - Temperature of the sensors
 ******************************************************************************/
float *CellTemp::ReadTempSensorData()
{
    float *cell_temp_sensor_results = (float *)malloc(sizeof(sensors_count));
    for (int i = 0; i < sensors_count; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(temp_sensor_addresses, i))
        {
            cell_temp_sensor_results[i] = sensors.getTempC(temp_sensor_addresses);

            printAddress(temp_sensor_addresses);
            SERIAL_DEBUG_EOL("");
        }
        else
        {
            SERIAL_DEBUG_LN("Found ghost device at ");
            Serial.print(i, DEC);
            SERIAL_DEBUG_LN(" but could not detect address. Check power and cabling");
        }
    }
    return cell_temp_sensor_results;
}

CellTemp Cell_Temp;