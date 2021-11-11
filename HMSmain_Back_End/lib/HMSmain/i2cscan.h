/*
 i2cscan.h - HBAT library
 Copyright (c) 2021 Zacariah Austin Heim.
 */

#ifndef i2cscan_h
#define i2cscan_h
#include "Arduino.h"
#include "Wire.h"

class Scanner
{
public:
    Scanner();
    void setup_scan();
    void begin_scan();
    /* private: */
};
#endif