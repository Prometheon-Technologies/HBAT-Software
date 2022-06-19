#ifndef OTA_HPP
#define OTA_HPP
#include <Arduino.h>
#include <defines.hpp>
#include <SecureOTA.hpp>

class OTA
{
public:
    OTA();
    virtual ~OTA();
    void setupOTA();
    void loopOTA();
private:
    // variables
};

extern OTA ota;
#endif