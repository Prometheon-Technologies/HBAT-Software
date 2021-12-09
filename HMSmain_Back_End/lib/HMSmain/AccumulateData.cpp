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

AccumulateData::AccumulateData(void)
{
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
  // debug("freeMemory()="+freeMemory());
}

/******************************************************************************
 * Function: Accumulate Data from sensor arrays
 * Description: This function accumualtes all sensor data and stores it in the main data structure.
 * Parameters: None
 * Return: None
 ******************************************************************************/
data_arrays AccumulateData::AccumulateDataMainLoop()
{
  data_arrays data;
  // Flow Rate data
  data.flow_rate_sensor_status = Hum.SFM3003();
  if (data.flow_rate_sensor_status == 0)
  {
    // SFM3003 flow rate data in slm
    data.flow_rate = Hum.flow;
    // SFM3003 mass temp data
    data.flow_rate_sensor_temp = Hum.temperature;
  }
  else
  {
    printf("Flow Rate Sensor Could Not Be Read\n");
  }

  // Stack level data
  data.stack_humidity = Hum.StackHumidity();
  debugln(data.stack_humidity);
  data.stack_temp = Hum.AverageStackTemp();
  debugln(data.stack_temp);

  // Cell level data

  float *cell_temp = Cell_Temp.ReadTempSensorData();

  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    data.cell_temp[i] = cell_temp[i];
    debugln(data.cell_temp[i]);
  }

  free(cell_temp);

  float *cell_voltage = HMSmain.readSensAndCondition();

  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    data.cell_voltage[i] = cell_voltage[i];
    debugln(data.cell_voltage[i]);
  }

  free(cell_voltage);

  data.stack_voltage = 0;

  int numSensors = Cell_Temp.GetSensorCount();
  if (numSensors > maxCellCount)
  {
    numSensors = maxCellCount;
  }
  for (int i = 0; i < numSensors; i++)
  {
    data.stack_voltage += cell_voltage[i];
    debugln(data.cell_temp[i]);
  }
  return data;
}
