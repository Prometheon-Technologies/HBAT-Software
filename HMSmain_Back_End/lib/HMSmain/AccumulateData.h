/*
 accumulatedata.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef ACCUMULATEDATA_h
#define ACCUMULATEDATA_h
#include <defines.h>

//#include <MemoryFree.h>

// extern DynamicJsonDocument  doc(200);

class AccumulateData
{
public:
    // Functions
    AccumulateData();
    virtual ~AccumulateData(void);
    void SetupMainLoop();
    void InitAccumulateDataJson();
    /* data_arrays AccumulateDataMainLoop(); */
    // Variables
    String json;
    int maxVoltage;
    int maxTemp;

private:
};
#endif