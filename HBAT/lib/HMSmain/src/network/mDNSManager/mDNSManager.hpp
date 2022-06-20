#ifndef MDNSMANAGER_HPP
#define MDNSMANAGER_HPP
#include <defines.hpp>
#include <ESPmDNS.h>
#include "observer.hpp"

namespace mDNSManager
{
    class MDNSHandler : public IObserver
    {
    public:
        MDNSHandler(StateManager *stateManager, Config *deviceConfig);
        virtual ~MDNSHandler();

        bool startMDNS();
        void update(ObserverEvent::Event event);

        //******************************************************************************
        // * Function: Discover mDNS Broker
        // * Description: Find the mDNS broker and port and write them to the config struct
        // * Parameters: None
        // * Return: int - 1 if success, 0 if fail
        //******************************************************************************
        bool DiscovermDNSBroker();

    private:
        StateManager *stateManager;
        Config *deviceConfig;
    };
}

extern mDNSManager::MDNSHandler *mDNSHandler;

#endif // MDNSMANAGER_HPP