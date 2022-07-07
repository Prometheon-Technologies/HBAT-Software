#include "accumulatedata.hpp"

AccumulateData::AccumulateData() : _maxVoltage(24), _maxTemp(100), _numSensors(0)
{
}

AccumulateData::~AccumulateData()
{
}

/******************************************************************************
 * Function: Accumulate Data to send from sensors and store in json
 * Description: This function accumulates all sensor data and stores it in the main json data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::InitAccumulateData()
{
    if (!Cell_Temp.setSensorCount())
    {
        _numSensors = 0;
    }
    else
    {
        _numSensors = Cell_Temp.getSensorCount();
    }

    cfg.config.numSensors = _numSensors;
    // Initialize the library
    humidity.ReadSensor();

    /******************************************************************************
     * Function: Setup the Stack Climate Data
     * Description: This function setups the stack climate data.
     * Parameters: None
     * Return: The mean Stack Climate Data
     ******************************************************************************/
    // Stack Data to send
    cfg.config.stack_humidity = humidity.StackHumidity();
    cfg.config.stack_temp = humidity.AverageStackTemp();
    cfg.config.stack_current = HMSmain.readAmps();

    // Add arrays for Cell level Data.
    float *cell_voltage = HMSmain.readSensAndCondition().cell_voltage;
    // loop through and store per cell voltage
    for (int i = 0; i < _numSensors; i++)
    {
        cfg.config.cell_voltage[i] = cell_voltage[i];
    }

    /******************************************************************************
     * Function: Setup the Stack Voltage
     * Description: This function setups the mean Stack Voltage by calculating the sum of the cell_voltage array
     * Parameters: None
     * Return: The mean Stack Voltage
     ******************************************************************************/
#if !PRODUCTION
    float array[5];
#else
    float array[10];
#endif // !PRODUCTION
    
    float sum = 0;
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++)
    {
        array[i] = cell_voltage[i];
    }

    for (int i = 0; i < sizeof(cell_voltage) / sizeof(cell_voltage[0]); i++)
    {
        sum += array[i];
    }
    log_i("Stack Voltage: %.3f", sum);

    cfg.config.stack_voltage = sum;

    cfg.config.numSensors = _numSensors;

    // Flow Rate data to send
    cfg.config.flow_rate = humidity.result.flow;
    // returns a float array of cell temperatures

    // loop through and store per cell temp data

    for (int i = 0; i < _numSensors; i++)
    {
        cfg.config.cell_temp[i] = Cell_Temp.cell_temp_sensor_results.temp[i];
    }

    // Relays
    for (int i = 0; i < sizeof(cfg.config.relays_pin) / sizeof(cfg.config.relays_pin[0]); i++)
    {
        Relay.RelayOnOff(cfg.config.relays_pin[i], cfg.config.relays[i]);
        log_d("Relay on pin: %d is %s", cfg.config.relays_pin[i], cfg.config.relays[i] ? "on" : "off");
    }
}

/******************************************************************************
 * Function: Return Charge Status of Stack
 * Description: This function reads the current stack voltage and returns a number representing the charge status
 * Parameters: None
 * Return: Enum representing the charge status
 ******************************************************************************/
ProgramStates::BatteryChargeState::ChargeState AccumulateData::ChargeStatus()
{
    if (cfg.config.stack_voltage < 8.00 && cfg.config.stack_voltage >= 0.00)
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_DISCHARGED);
        return ProgramStates::BatteryChargeState::CHARGE_DISCHARGED;
        log_i("Stack is fully discharged");
    }
    else if (cfg.config.stack_voltage < 10.00 && cfg.config.stack_voltage >= 8.00)
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_LOW);
        return ProgramStates::BatteryChargeState::CHARGE_LOW;
        log_i("Stack needs to be charged");
    }
    else if (cfg.config.stack_voltage < 13.00 && cfg.config.stack_voltage >= 11.0)
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_MEDIUM);
        return ProgramStates::BatteryChargeState::CHARGE_MEDIUM;
        log_i("Stack has a full charge");
    }
    else if (cfg.config.stack_voltage < 14.0 && cfg.config.stack_voltage >= 12.0)
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_CHARGING);
        return ProgramStates::BatteryChargeState::CHARGE_CHARGING;
        log_i("Stack is charging");
    }
    else if (cfg.config.stack_voltage < 16.0 && cfg.config.stack_voltage >= 14.0)
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_OVERCHAREGD);
        return ProgramStates::BatteryChargeState::CHARGE_OVERCHAREGD;
        log_w("[DANGER]: Stack has encountered an overcharge condition");
    }

    else
    {
        stateManager.setState(ProgramStates::BatteryChargeState::CHARGE_ERROR);
        return ProgramStates::BatteryChargeState::CHARGE_ERROR;
        log_w("[DANGER]: Stack is in an unknown state!!");
    }
    return ProgramStates::BatteryChargeState::CHARGE_ERROR;
}

bool AccumulateData::SendData()
{
    ProgramStates::BatteryChargeState::ChargeState chargeState = ChargeStatus();
    String json = "";
    json += R"====({)====";

    json += R"====("stack_humidity":)====";
    json += (String)cfg.config.stack_humidity + ",\n";

    json += R"====("stack_temp":)====";
    json += (String)cfg.config.stack_temp + ",\n";

    json += R"====("relays":[)====";
    json += (String)cfg.config.relays[0] + "," + (String)cfg.config.relays[1] + "," + (String)cfg.config.relays[2] + "," + (String)cfg.config.relays[3] + "," + (String)cfg.config.relays[4] + "],\n";

    json += R"====("stack_voltage":)====";
    json += (String)cfg.config.stack_voltage + ",\n";

    json += R"====("mqtt_enable":)====";
    json += (String)cfg.config.MQTTEnabled + ",\n";

    json += R"====("charge_status":)====";
    json += (String)chargeState + ",\n";

    json += R"====("GraphData":[)====";
    json += "\n";
    for (int i = 0; i < 10; i++)
    {
        delay(0);
        json += R"====({"label": "🌡 )====" + (String)i + "\",\n";
        json += R"====("type": "temp",)====" + (String) "\n";
        json += R"====("value": )====" + (String)cfg.config.cell_temp[i] + (String) ",\n";
        json += R"====("maxValue": )====" + (String)_maxTemp;
        json += R"====(})====" + (String) "\n";
        json += R"====(,)====";

        json += R"====({"label": "⚡ )====" + (String)i + "\",\n";
        json += R"====("type": "volt",)====" + (String) "\n";
        json += R"====("value": )====" + (String)cfg.config.cell_voltage[i] + (String) ",\n";
        json += R"====("maxValue": )====" + (String)_maxVoltage;
        json += R"====(})====" + (String) "\n";

        if (i < 9)
        {
            json += R"====(,)====";
        };
    }
    json += R"====(])====";
    json += R"====(})====";

    if (json.length() > 0)
    {
        cfg.config.data_json_string = json;
        return true;
    }
    else
    {
        return false;
    }
}

AccumulateData accumulatedata;
