/*
 relaylogic.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef RELAYLOGIC_h
#define RELAYLOGIC_h
#include <Arduino.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
#include <PID_v1.h>
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
    // Setup an array of relays to control peripherals. Numbers represent pin numbers.
    const int relays[5] = {45, 38, 36, 35, 48};
    int received;

private:
};
#endif