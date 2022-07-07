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
            Started,
            Stopping,
            Stopped,
            Error
        };

        enum WiFiState
        {
            WiFiState_None,
            WiFiState_Connecting,
            WiFiState_Connected,
            WiFiState_Disconnected,
            WiFiState_Disconnecting,
            WiFiState_Error
        };

        enum WebServerState
        {
            WebServerState_None,
            WebServerState_Starting,
            WebServerState_Started,
            WebServerState_Stopping,
            WebServerState_Stopped,
            WebServerState_Error
        };

        enum MDNSState
        {
            MDNS_Success,
            MDNS_Connected,
            MDNS_Disconnected,
            MDNS_Error
        };

        enum MQTTState
        {
            MQTT_Success,
            MQTT_Connected,
            MQTT_Disconnected,
            MQTT_Error
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
    StateManager();
    virtual ~StateManager();
    
    void setState(ProgramStates::DeviceState::State state);
    void setState(ProgramStates::DeviceState::WiFiState state);
    void setState(ProgramStates::DeviceState::WebServerState state);
    void setState(ProgramStates::DeviceState::MDNSState state);
    void setState(ProgramStates::DeviceState::MQTTState state);
    void setState(ProgramStates::EnabledHumiditySensors::EnabledSensors state);
    void setState(ProgramStates::BatteryChargeState::ChargeState state);

    ProgramStates::DeviceState::State getCurrentState();
    ProgramStates::DeviceState::WiFiState getCurrentWiFiState();
    ProgramStates::DeviceState::WebServerState getCurrentWebServerState();
    ProgramStates::DeviceState::MDNSState getCurrentMDNSState();
    ProgramStates::DeviceState::MQTTState getCurrentMQTTState();
    ProgramStates::EnabledHumiditySensors::EnabledSensors getCurrentSensorState();
    ProgramStates::BatteryChargeState::ChargeState getCurrentBatteryState();

private:
    ProgramStates::DeviceState::State _current_state;
    ProgramStates::DeviceState::WiFiState _current_wifi_state;
    ProgramStates::DeviceState::WebServerState _current_webserver_state;
    ProgramStates::DeviceState::MDNSState _current_mdns_state;
    ProgramStates::DeviceState::MQTTState _current_mqtt_state;
    ProgramStates::EnabledHumiditySensors::EnabledSensors _enabled_humidity_sensors;
    ProgramStates::BatteryChargeState::ChargeState _battery_charge_state;
};
extern StateManager stateManager;
#endif // STATEMANAGER_HPP