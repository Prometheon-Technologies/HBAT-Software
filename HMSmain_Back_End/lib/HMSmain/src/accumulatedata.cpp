#include "accumulatedata.hpp"

int numSensors = 10;

AccumulateData::AccumulateData()
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
    /* if (numSensors > maxCellCount)
    {
        numSensors = maxCellCount;
    } */

    // Stack Data to send
    cfg.config.stack_humidity = humidity.StackHumidity();
    cfg.config.stack_temp = humidity.AverageStackTemp();
    cfg.config.stack_current = HMSmain.readAmps();
    cfg.config.stack_voltage = HMSmain.StackVoltage();
    cfg.config.cell_count_max = maxCellCount;
    cfg.config.numSensors = numSensors;
    // Flow Rate dataTosend
    if (cfg.config.flow_rate > 0)
    {
        // SFM3003 flow rate dataTosend in slm
        cfg.config.flow_rate = humidity.loopSFM3003();
        // SFM3003 mass temp dataTosend
        //cfg.config.flow_rate_sensor_temp = humidity.temperature;
    }
    else
    {
        SERIAL_DEBUG_LN(("Flow Rate Sensor Could Not Be Read\n"));
        // SFM3003 flow rate dataTosend in slm
        cfg.config.flow_rate = 0;
        // SFM3003 mass temp dataTosend
        cfg.config.flow_rate_sensor_temp = 0;
    }

    // Add arrays for Cell level Data.
    float *cell_voltage = HMSmain.readSensAndCondition();
    // loop through and store per cell voltage
    for (int i = 0; i < numSensors; i++)
    {
        cfg.config.cell_voltage[i] = cell_voltage[i];
    }

    free(cell_voltage); // free the memory

    float *cell_temp = Cell_Temp.ReadTempSensorData(); // returns a float array of cell temperatures
    // loop through and store per cell temp data
    for (int i = 0; i < numSensors; i++)
    {
        cfg.config.cell_temp[i] = cell_temp[i];
    }

    free(cell_temp); // free the memory

    // Individual Humidity sensor data
    float stack_humidity[4];
    for (int i = 0; i < 4; i++)
    {
        stack_humidity[i] = *humidity.ReadSensor();
        cfg.config.stack_humidity = stack_humidity[1];
        cfg.config.stack_temp = stack_humidity[3];
    }
}

AccumulateData accumulatedata;
