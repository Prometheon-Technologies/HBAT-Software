/*
 celltemp.h - HBAT library
 Copyright (c) 2021 ZanzyTHEbar
 */

#ifndef CELLTEMP_HPP
#define CELLTEMP_HPP
// Data wire is plugged into port 42 on the ESP32
#define ONE_WIRE_BUS 42
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class CellTemp
{
public:
    CellTemp();
    virtual ~CellTemp();
    void SetupSensors();
    void readAddresses(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);

    struct Temp
    {
        float temp[10];
    };

    Temp cell_temp_sensor_results;

    Temp ReadTempSensorData();

private:
};

extern int sensors_count;
extern CellTemp Cell_Temp;
#endif