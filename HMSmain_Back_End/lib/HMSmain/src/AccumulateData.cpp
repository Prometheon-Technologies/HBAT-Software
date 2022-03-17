#include "AccumulateData.hpp"

AccumulateData::AccumulateData()
{
  json = "";
  maxVoltage = 24;
  maxTemp = 100; // in celsius
}

AccumulateData::~AccumulateData(void)
{
}

/******************************************************************************
 * Function: Setup Main Loop
 * Description: This is the setup function for the main loop of the whole program. Use this to setup the main loop.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::SetupMainLoop()
{
  Hum.setupSfm3003();
  HMSmqtt.MQTTSetup();
  // debug("freeMemory()="+freeMemory());
}

/******************************************************************************
 * Function: Accumulate Data to send from sensors and store in json
 * Description: This function accumulates all sensor data and stores it in the main json data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
void AccumulateData::InitAccumulateDataJson()
{
  // Temporary function to ensure that the correct number of cells are being read - this will be removed when the cell count is dynamically allocated
  int numSensors = Cell_Temp.GetSensorCount();
  if (numSensors > maxCellCount)
  {
    numSensors = maxCellCount;
  }

  // Stack Data to send
  Doc["HMS_Stack_Humidity"] = Hum.StackHumidity();
  Doc["HMS_Stack_Temp"] = Hum.AverageStackTemp();
  Doc["HMS_Stack_Current"] = HMSmain.readAmps();
  Doc["HMS_Stack_Voltage"] = HMSmain.StackVoltage();

  // Relays
  JsonArray Relays = Doc.createNestedArray("HMS_Relays");
  bool relays[5] = {1, 2, 3, 4, 5};
  for (int i = 0; i < 5; i++)
  {
    Relays.add(relays[i]);
  }

  // Flow Rate dataTosend
  Doc["Flow_Rate_Sensor_Status"] = Hum.loopSFM3003();
  int flow_rate_sensor_status = Hum.loopSFM3003();
  if (flow_rate_sensor_status == 0)
  {
    // SFM3003 flow rate dataTosend in slm
    Doc["HMS_Flow_Rate"] = Hum.flow;
    // SFM3003 mass temp dataTosend
    Doc["Flow_Rate_Sensor_Temp"] = Hum.temperature;
  }
  else
  {
    SERIAL_DEBUG_LN(("Flow Rate Sensor Could Not Be Read\n"));
    // SFM3003 flow rate dataTosend in slm
    Doc["HMS_Flow_Rate"] = 0;
    // SFM3003 mass temp dataTosend
    Doc["Flow_Rate_Sensor_Temp"] = 0;
  }

  // Add arrays for Cell level Data.
  JsonArray Cell_Voltage = Doc.createNestedArray("HMS_Cell_Voltage"); // from 0 - 10 in increasing order
  float *cell_voltage = HMSmain.readSensAndCondition();
  // loop through and store per cell voltage
  for (int i = 0; i < numSensors; i++)
  {
    Cell_Voltage.add(cell_voltage[i]);
  }

  free(cell_voltage); // free the memory

  JsonArray CellTemp = Doc.createNestedArray("HMS_Cell_Temp");
  float *cell_temp = Cell_Temp.ReadTempSensorData(); // returns a float array of cell temperatures
  // loop through and store per cell temp data
  for (int i = 0; i < numSensors; i++)
  {
    CellTemp.add(cell_temp[i]);
  }

  free(cell_temp); // free the memory

  // Individual Humidity sensor data
  JsonArray Humidity_Sensor_Data = Doc.createNestedArray("Humidity_Sensor_Data");
  float stack_humidity[4];
  for (int i = 0; i < 4; i++)
  {
    stack_humidity[i] = *Hum.ReadSensor();
    Humidity_Sensor_Data.add(stack_humidity[1]);
    Humidity_Sensor_Data.add(stack_humidity[3]);
  }

  SERIAL_DEBUG_EOL(serializeJson(Doc, Serial));
  json = Doc.as<String>();
  if (json.length() > 0)
  {
    SERIAL_DEBUG_LN(json);
  }
}

AccumulateData Accumulate_Data;