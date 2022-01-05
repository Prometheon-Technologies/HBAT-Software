#include <AccumulateData.h>

AccumulateData::AccumulateData()
{
  json = "";
  maxVoltage = 24;
  maxTemp = 100; // in celcius
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
  Hum.SetupSFM3003();
  MqttData.MQTTSetup();
  // debug("freeMemory()="+freeMemory());
}

/******************************************************************************
 * Function: Accumulate DataTosend from sensor arrays
 * Description: This function accumualtes all sensor data and stores it in the main data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
/* data_arrays AccumulateData::AccumulateDataMainLoop()
{
  data_arrays dataTosend;
  // Flow Rate dataTosend
  dataTosend.flow_rate_sensor_status = Hum.SFM3003();
  if (dataTosend.flow_rate_sensor_status == 0)
  {
    // SFM3003 flow rate dataTosend in slm
    dataTosend.flow_rate = Hum.flow;
    // SFM3003 mass temp dataTosend
    dataTosend.flow_rate_sensor_temp = Hum.temperature;
  }
  else
  {
    SERIAL_DEBUG_LN(("Flow Rate Sensor Could Not Be Read\n"));
  }

  // Stack level dataTosend
  dataTosend.stack_humidity = Hum.StackHumidity();
  debugln(dataTosend.stack_humidity);
  dataTosend.stack_temp = Hum.AverageStackTemp();
  debugln(dataTosend.stack_temp);

  // Cell level dataTosend

  float *cell_temp = Cell_Temp.ReadTempSensorData();
  // loop through and store per cell temp data
  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    dataTosend.cell_temp[i] = cell_temp[i];
    debugln(dataTosend.cell_temp[i]);
  }

  free(cell_temp);

  float *cell_voltage = HMSmain.readSensAndCondition();
  // loop through and store per cell voltage dataTosend
  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    dataTosend.cell_voltage[i] = cell_voltage[i];
    debugln(dataTosend.cell_voltage[i]);
  }

  free(cell_voltage);

  dataTosend.stack_voltage = 0;

  int numSensors = Cell_Temp.GetSensorCount();
  if (numSensors > maxCellCount)
  {
    numSensors = maxCellCount;
  }
  for (int i = 0; i < numSensors; i++)
  {
    dataTosend.stack_voltage += cell_voltage[i];
    debugln(dataTosend.cell_temp[i]);
  }
  return dataTosend;
} */

/******************************************************************************
 * Function: Accumulate Data to send from sensors and store in json
 * Description: This function accumualtes all sensor data and stores it in the main json data structure.
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
  doc["HMS_Stack_Humidity"] = Hum.StackHumidity();
  doc["HMS_Stack_Temp"] = Hum.AverageStackTemp();
  doc["HMS_Stack_Current"] = HMSmain.readAmps();
  doc["HMS_Stack_Voltage"] = HMSmain.StackVoltage();

  // Relays
  JsonArray Relays = doc.createNestedArray("HMS_Relays");
  bool relays[5] = {1, 2, 3, 4, 5};
  for (int i = 0; i < 5; i++)
  {
    Relays.add(relays[i]);
  }

  doc["Flow_Rate_Sensor_Status"] = Hum.SFM3003();
  // doc["HMS_Relays"] = "";

  // Flow Rate dataTosend
  int flow_rate_sensor_status = Hum.SFM3003();
  if (flow_rate_sensor_status == 0)
  {
    // SFM3003 flow rate dataTosend in slm
    doc["HMS_Flow_Rate"] = Hum.flow;
    // SFM3003 mass temp dataTosend
    doc["Flow_Rate_Sensor_Temp"] = Hum.temperature;
  }
  else
  {
    SERIAL_DEBUG_LN(("Flow Rate Sensor Could Not Be Read\n"));
    // SFM3003 flow rate dataTosend in slm
    doc["HMS_Flow_Rate"] = 0;
    // SFM3003 mass temp dataTosend
    doc["Flow_Rate_Sensor_Temp"] = 0;
  }

  // Add arrays for Cell level Data.
  JsonArray Cell_Voltage = doc.createNestedArray("HMS_Cell_Voltage"); // from 0 - 10 in increasing order
  float *cell_voltage = HMSmain.readSensAndCondition();
  // loop through and store per cell voltage
  for (int i = 0; i < numSensors; i++)
  {
    Cell_Voltage.add(cell_voltage[i]);
  }

  free(cell_voltage); // free the memory

  JsonArray CellTemp = doc.createNestedArray("HMS_Cell_Temp");
  float *cell_temp = Cell_Temp.ReadTempSensorData(); // returns a float array of cell temperatures
  // loop through and store per cell temp data
  for (int i = 0; i < numSensors; i++)
  {
    CellTemp.add(cell_temp[i]);
  }

  free(cell_temp); // free the memory

  // Individual Huimidity sensor data
  JsonArray Humidity_Sensor_Data = doc.createNestedArray("Humidity_Sensor_Data");
  float stack_humidity[4];

  for (int i = 0; i < 4; i++)
  {
    stack_humidity[i] = *Hum.ReadSensor();
    Humidity_Sensor_Data.add(stack_humidity[1]);
    Humidity_Sensor_Data.add(stack_humidity[3]);
  }

  SERIAL_DEBUG_EOL(serializeJson(doc, Serial));
  json = doc.as<String>();
  if (json.length() > 0)
  {
    SERIAL_DEBUG_LN(json);
  }
  if (MQTT_ENABLED && MQTT_CONNECTED)
  {
    String currentTopic = MQTT_TOPIC + HMSmain.getDeviceID() + "/all_json_data";
    MqttData.MQTTPublish(currentTopic, json);
  }
}

/* void InitJsonData()
{
  data_arrays dataTosend;
  String json = AccumulateData().json;
  int maxTemp = AccumulateData().maxTemp;
  int maxVoltage = AccumulateData().maxVoltage;
  json = "";
  json += R"====({)====";

  json += R"====("stack_humidity":)====";
  json += (String)dataTosend.stack_humidity + ",\n";

  json += R"====("stack_temp":)====";
  json += (String)dataTosend.stack_temp + ",\n";

  json += R"====("relays":[)====";
  json += (String)dataTosend.relays[0] + "," + (String)dataTosend.relays[1] + "," + (String)dataTosend.relays[2] + "," + (String)dataTosend.relays[3] + "," + (String)dataTosend.relays[4] + "],\n";

  json += R"====("stack_voltage":)====";
  json += (String)dataTosend.stack_voltage + ",\n";

  json += R"====("fakeGraphData":[)====";
  json += "\n";
  for (int i = 0; i < 10; i++)
  {

    delay(0);
    json += R"====({"label": "🌡 )====" + (String)i + "\",\n";
    json += R"====("type": "temp",)====" + (String) "\n";
    json += R"====("value": )====" + (String)dataTosend.cell_temp[i] + (String) ",\n";
    json += R"====("maxValue": )====" + (String)maxTemp;
    json += R"====(})====" + (String) "\n";
    json += R"====(,)====";

    json += R"====({"label": "⚡ )====" + (String)i + "\",\n";
    json += R"====("type": "volt",)====" + (String) "\n";
    json += R"====("value": )====" + (String)dataTosend.cell_voltage[i] + (String) ",\n";
    json += R"====("maxValue": )====" + (String)maxVoltage;
    json += R"====(})====" + (String) "\n";

    if (i < 9)
    {
      json += R"====(,)====";
    };
  }
  json += R"====(])====";
  json += R"====(})====";
  json = "";
  Serial.println(json);
  MqttData.MQTTPublish("/HMS" ,json);
}
 */