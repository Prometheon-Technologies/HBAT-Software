#include "CellTemp.hpp"

CellTemp::CellTemp()
{
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
 * Function: Get Sensor Count
 * Description: Return the number of sensors on the bus
 * Parameters: None
 * Return: Int - Number of sensors on the bus
 ******************************************************************************/
int CellTemp::GetSensorCount()
{
    // Grab a count of devices on the wire
    return sensors.getDeviceCount();
}

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
            Serial.print("0");
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
            Serial.println();
        }
        else
        {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.print(" but could not detect address. Check power and cabling");
        }
    }
    return cell_temp_sensor_results;
}

CellTemp Cell_Temp;