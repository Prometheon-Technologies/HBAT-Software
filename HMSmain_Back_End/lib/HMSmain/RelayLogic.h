/*
 relaylogic.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef RELAYLOGIC_h
#define RELAYLOGIC_h
#include <Arduino.h>
#include <PID_v1.h>
#include <AccumulateData.h>
//#include <MemoryFree.h>

class RelayLogic
{
public:
    // Functions
    RelayLogic();
    void SetupRelays();
    void SetupPID();
    void HumRelayOnOff(int time, float *stack_humidity);
    // Variables
    /* HMS HMSmain = HMS();
    Humidity Hum = Humidity();
    CellTemp Cell_Temp = CellTemp(); */
    AccumulateData StackData = AccumulateData();
    // Setup an array of relays to control peripherals. Numbers represent pin numbers.
    const int relays[5] = {45, 38, 36, 35, 48};
    int received;

private:
};
#endif