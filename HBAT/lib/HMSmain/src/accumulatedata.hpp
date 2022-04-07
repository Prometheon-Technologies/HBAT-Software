#ifndef ACCUMULATEDATA_HPP
#define ACCUMULATEDATA_HPP
#include "defines.hpp"

class AccumulateData
{
public:
    AccumulateData();
    virtual ~AccumulateData();

    void InitAccumulateData();
    int ChargeStatus();

private:
    // Stack Data to send
};

extern AccumulateData accumulatedata;

#endif