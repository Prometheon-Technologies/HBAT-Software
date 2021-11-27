/*
 accumulatedata.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef ACCUMULATEDAT_h
#define ACCUMULATEDATA_h
#include <Arduino.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
//#include <MemoryFree.h>

class AccumulateData
{
public:
    //Functions
    AccumulateData();
    struct data_arrays;
    void SetupMainLoop();
    void AccumulateDataMainLoop(data_arrays &data);
    // Variables
    int received;

private:
};
#endif