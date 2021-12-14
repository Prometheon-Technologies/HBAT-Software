#include <AccumulateData.h>
#define DEBUG 1
#define maxCellCount 10
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#endif

HMS HMSmain = HMS();
Humidity Hum = Humidity();
CellTemp Cell_Temp = CellTemp();
HMSmqtt MqttData = HMSmqtt();

AccumulateData::AccumulateData()
{
  json = "";
  maxVoltage = 24;
  maxTemp = 100;
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
data_arrays AccumulateData::AccumulateDataMainLoop()
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
    printf("Flow Rate Sensor Could Not Be Read\n");
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
}

void InitJsonData()
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
