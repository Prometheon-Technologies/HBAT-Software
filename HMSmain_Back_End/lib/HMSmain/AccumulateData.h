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
#include <HMSmqtt.h>
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
    float flow_rate;
    float flow_rate_sensor_temp;
    int flow_rate_sensor_status;
};

extern HMS HMSmain;
extern Humidity Hum;
extern CellTemp Cell_Temp;

class AccumulateData
{
public:
    // Functions
    AccumulateData();
    virtual ~AccumulateData(void);
    void SetupMainLoop();
    data_arrays AccumulateDataMainLoop();
    // Variables
    String json;
    int maxVoltage;
    int maxTemp;

private:
};
#endif