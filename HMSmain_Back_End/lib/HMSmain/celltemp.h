/*
 celltemp.h - HBAT library
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
    void readAddresses(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);
    int read_temp_sensor_data();
    void setup_sensors();
    /* private: */
};
#endif