/*
 i2cscan.h - HBAT library
 Copyright (c) 2021 ZanzyTHEbar
 */

#ifndef I2CSCAN_HPP
#define I2CSCAN_HPP
#include <defines.hpp>
#include <Arduino.h>
#include <Wire.h>

class Scanner
{
public:
    Scanner(void);
    virtual ~Scanner(void);
    void SetupScan();
    void BeginScan();

private:
};

extern Scanner Scan;
#endif