/*
 HMS.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef CELLTEMP_h
#define CELLTEMP_h
#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"

class CELLTEMP
{
public:
    CELLTEMP();
    int freeRam();
    int deviceCount = 0;
    void printTemperature(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);
    void read_temp_sensor_data();
    void setup_sensors();
    /* private: */
};
#endif