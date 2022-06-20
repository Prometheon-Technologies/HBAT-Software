#include "StateManager.hpp"

StateManager::StateManager() : _current_state(ProgramStates::DeviceState::Starting),
                               _enabled_humidity_sensors(ProgramStates::EnabledHumiditySensors::HUM_NONE),
                               _battery_charge_state(ProgramStates::BatteryChargeState::CHARGE_OFF)
{
}

StateManager::~StateManager() {}

void StateManager::setState(ProgramStates::DeviceState::State state)
{
    _current_state = state;
}

void StateManager::setState(ProgramStates::EnabledHumiditySensors::EnabledSensors state)
{
    _enabled_humidity_sensors = state;
}

void StateManager::setState(ProgramStates::BatteryChargeState::ChargeState state)
{
    _battery_charge_state = state;
}

void StateManager::setState(ProgramStates::DeviceState::WiFiState state)
{
    _current_wifi_state = state;
}

void StateManager::setState(ProgramStates::DeviceState::WebServerState state)
{
    _current_webserver_state = state;
}

void StateManager::setState(ProgramStates::DeviceState::MDNSState state)
{
    _current_mdns_state = state;
}

void StateManager::setState(ProgramStates::DeviceState::MQTTState state)
{
    _current_mqtt_state = state;
}

/*
 * Get States
 * Returns the current state of the device
 */

ProgramStates::DeviceState::WiFiState StateManager::getCurrentWiFiState()
{
    return _current_wifi_state;
}

ProgramStates::DeviceState::WebServerState StateManager::getCurrentWebServerState()
{
    return _current_webserver_state;
}

ProgramStates::DeviceState::MDNSState StateManager::getCurrentMDNSState()
{
    return _current_mdns_state;
}

ProgramStates::DeviceState::MQTTState StateManager::getCurrentMQTTState()
{
    return _current_mqtt_state;
}

ProgramStates::DeviceState::State StateManager::getCurrentState()
{
    return _current_state;
}

ProgramStates::EnabledHumiditySensors::EnabledSensors StateManager::getCurrentSensorState()
{
    return _enabled_humidity_sensors;
}

ProgramStates::BatteryChargeState::ChargeState StateManager::getCurrentBatteryState()
{
    return _battery_charge_state;
}

StateManager stateManager;