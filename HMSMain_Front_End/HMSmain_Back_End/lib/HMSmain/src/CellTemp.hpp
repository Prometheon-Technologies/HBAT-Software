/*
 celltemp.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef CELLTEMP_h
#define CELLTEMP_h
#include "defines.hpp"

class CellTemp
{
public:
    CellTemp(void);
    virtual ~CellTemp(void);
    uint32_t getfreeRam();
    void SetupSensors();
    void readAddresses(DeviceAddress deviceAddress);
    void printAddress(DeviceAddress deviceAddress);
    float *ReadTempSensorData();
    int GetSensorCount();
    void SetSensorCount();

private:
    int sensors_count;
};
extern CellTemp Cell_Temp;
#endif