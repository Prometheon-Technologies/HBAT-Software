#ifndef OTA_HPP
#define OTA_HPP
#include <defines.hpp>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

class OTA
{
public:
    OTA();
    virtual ~OTA();

    void SetupOTA(Config deviceConfig);

    void HandleOTAUpdate();

private:
    unsigned long _bootTimestamp = 0;
    bool _isOtaEnabled = true;
};

extern OTA ota;
#endif // OTA_HPP