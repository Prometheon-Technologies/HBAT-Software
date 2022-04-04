#ifndef RELAYS_HPP
#define RELAYS_HPP
#include "defines.hpp"

class Relays
{
public:
    Relays();
    virtual ~Relays();
    void SetupRelays();
    void RelayOnOff(int relay, bool on, long delay);
    void SetupPID();
    void HumRelayOnOff();
private:
};
extern Relays Relay;
#endif