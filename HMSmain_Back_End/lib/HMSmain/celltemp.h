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
    void setup_sensors();
    void readAddresses(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);
    void *read_temp_sensor_data(float *cell_temp_sensor_results);
    int sensors_count;
    /* private: */
};
#endif