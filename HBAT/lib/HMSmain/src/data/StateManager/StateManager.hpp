#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

/*
 * StateManager
 * All Project States are managed here
 */
class ProgramStates
{
public:
    struct EnabledHumiditySensors
    {
        enum EnabledSensors
        {
            HUM_NONE,
            HUM_SHT31_1,
            HUM_SHT31_2,
            HUM_SFM3003,
            HUM_SHT31_SFM3003,
            HUM_SHT31_SFM3003_SHT31,
            HUM_SHT31_BOTH
        };
    };

    struct BatteryChargeState
    {
        enum ChargeState
        {
            CHARGE_OFF,
            CHARGE_ON,
            CHARGE_AUTO,
            CHARGE_MANUAL,
            CHARGE_LOW,
            CHARGE_MEDIUM,
            CHARGE_FULL,
            CHARGE_OVERCHAREGD,
            CHARGE_DISCHARGED,
            CHARGE_DISCHARGING,
            CHARGE_CHARGED,
            CHARGE_CHARGING,
            CHARGE_ERROR
        };
    };

    struct DeviceState
    {
        enum State
        {
            Starting,
            ConnectingToWifi,
            ConnectingToWifiError,
            ConnectingToWifiSuccess,
            ServerError,
            MDNSSuccess,
            MDNSError,
            MQTTSuccess,
            MQTTError
        };
    };
};

/*
 * EventManager
 * All Project Events are managed here
 */

class StateManager
{
public:
    StateManager() {}
    void setState(ProgramStates::DeviceState::State state);
    void setState(ProgramStates::EnabledHumiditySensors::EnabledSensors state);
    void setState(ProgramStates::BatteryChargeState::ChargeState state);
    ProgramStates::DeviceState::State getCurrentState();
    ProgramStates::EnabledHumiditySensors::EnabledSensors getCurrentSensorState();
    ProgramStates::BatteryChargeState::ChargeState getCurrentBatteryState();

private:
    ProgramStates::DeviceState::State _current_state;
    ProgramStates::EnabledHumiditySensors::EnabledSensors _enabled_humidity_sensors;
    ProgramStates::BatteryChargeState::ChargeState _battery_charge_state;
};
extern StateManager stateManager;
#endif // STATEMANAGER_HPP