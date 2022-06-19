#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include <defines.hpp>

class AccumulateData
{
public:
    AccumulateData();
    virtual ~AccumulateData();

    void InitAccumulateData();
    ProgramStates::BatteryChargeState::ChargeState ChargeStatus();
    bool SendData();

private:
    // Stack Data to send
    byte _maxVoltage;
    byte _maxTemp;
    byte _numSensors;
};

extern AccumulateData accumulatedata;

#endif