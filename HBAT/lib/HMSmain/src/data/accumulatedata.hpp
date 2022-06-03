#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include <defines.hpp>

class AccumulateData
{
public:
    AccumulateData();
    virtual ~AccumulateData();

    void InitAccumulateData();
    int ChargeStatus();
    bool SendData();

private:
    // Stack Data to send
    int _maxVoltage;
    int _maxTemp;
    int _numSensors;
};

extern AccumulateData accumulatedata;

#endif