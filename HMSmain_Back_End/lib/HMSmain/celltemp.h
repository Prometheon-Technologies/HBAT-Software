/*
 celltemp.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef CELLTEMP_h
#define CELLTEMP_h
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class CellTemp
{
public:
    CellTemp();
    int freeRam();
    void setup_sensors();
    void readAddresses(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);
    float *read_temp_sensor_data();
    int GetSensorCount();
    void SetSensorCount();

private:
    int sensors_count;
};
#endif