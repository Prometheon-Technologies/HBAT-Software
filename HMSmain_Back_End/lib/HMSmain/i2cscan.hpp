/*
 i2cscan.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef I2CSCAN_h
#define I2CSCAN_h
#include <Arduino.h>
#include <Wire.h>

class Scanner
{
public:
    Scanner(void);
    virtual ~Scanner(void);
    void SetupScan();
    void BeginScan();
    /* private: */
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HMSMQTT)
extern Scanner Scan;
#endif
#endif