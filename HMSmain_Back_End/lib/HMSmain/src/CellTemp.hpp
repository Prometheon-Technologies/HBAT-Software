/*
 celltemp.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef CELLTEMP_HPP
#define CELLTEMP_HPP
// Data wire is plugged into port 42 on the ESP32
#define ONE_WIRE_BUS 42
#include "defines.hpp"
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
    float *ReadTempSensorData();
    int sensors_count;

private:
};
extern CellTemp Cell_Temp;
#endif