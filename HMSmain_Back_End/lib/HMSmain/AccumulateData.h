/*
 accumulatedata.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef ACCUMULATEDATA_h
#define ACCUMULATEDATA_h
#include <Arduino.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
#include <PID_v1.h>
//#include <MemoryFree.h>

class AccumulateData
{
public:
    // Functions
    AccumulateData();
    struct data_arrays;
    void SetupMainLoop();
    void AccumulateDataMainLoop(data_arrays &data);
    void HumRelayOnOff(int time, float *stack_humidity);
    void debugdata(String str);
    void setup_relays();
    void SetupPID();
    HMS HMSmain = HMS();
    Humidity Hum = Humidity();
    CellTemp Cell_Temp = CellTemp();

    // Variables
    // Setup an array of relays to control peripherals. Numbers represent pin numbers.
    const int relays[5] = {45, 38, 36, 35, 48};
    int received;

private:
};
#endif