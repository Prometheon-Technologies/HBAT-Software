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

/******************************************************************************
 * Function: Main Sensor Data Structure
 * Description: This is the main Data Structure where all sensor data is accumulated. To be passed into the main program.
 * Parameters: None
 * Return: None
 ******************************************************************************/
struct data_arrays
{
    boolean relays[5];
    float stack_humidity;
    float stack_temp;
    float stack_voltage;
    float cell_temp[10];
    float cell_voltage[10];
};

HMS HMSmain = HMS();
Humidity Hum = Humidity();
CellTemp Cell_Temp = CellTemp();

class AccumulateData
{
public:
    // Functions
    AccumulateData();
    struct data_arrays;
    void SetupMainLoop();
    data_arrays AccumulateDataMainLoop();
    void HumRelayOnOff(int time, float *stack_humidity);
    void debugdata(String str);
    void setup_relays();
    void SetupPID();

    // Variables
    // Setup an array of relays to control peripherals. Numbers represent pin numbers.
    const int relays[5] = {45, 38, 36, 35, 48};
    int received;

private:
};
#endif