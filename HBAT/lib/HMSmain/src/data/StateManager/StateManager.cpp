#include "StateManager.hpp"

StateManager::StateManager() : _current_state(ProgramStates::DeviceState::Starting),
                               _enabled_humidity_sensors(ProgramStates::EnabledHumiditySensors::HUM_NONE),
                               _battery_charge_state(ProgramStates::BatteryChargeState::Charge_OFF)
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