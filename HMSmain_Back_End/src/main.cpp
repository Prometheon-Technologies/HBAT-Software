#include <Arduino.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
//#include <MemoryFree.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x) Serial.printf(x)
#define debugCalibrateAmps() HMSmain.calibrateAmps()

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugCalibrateAmps()
#endif

#define LED1 37
#define LED2 47

// Variables

int received;

HMS HMSmain = HMS();
Humidity Hum = Humidity();
CellTemp Cell_Temp = CellTemp();

struct data_arrays
{
  float stack_humidity;
  float stack_temp;
  float stack_voltage;
  float cell_temp[10];
  float cell_voltage[10];
};

/* void led2OnOff(int time)
{
  digitalWrite(LED2, HIGH);
  delay(time);
  digitalWrite(LED2, LOW);
  delay(time);
} */

/* void ledFade(int LED)
{
  int freq = 1000;
  int ledChannel = 0;
  int resolution = 16;
  int dutyCycle = 0;
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED, ledChannel);
  dutyCycle = map(3, 0, 4095, 0, 32767);
  ledcWrite(ledChannel, dutyCycle);
  // increase the LED brightness
  for (int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for (int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--)
  {
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }
} */

/* void stack_climate()
{
  float *climatedata = Hum.ReadSensor();
  char climateData[100];
  sprintf(climateData, "%3d, %3d", "%3d, %3d", climatedata[0], climatedata[1], climatedata[2], climatedata[3]);
  debugln(climateData);

  String voltageaverage = "";
  float *readvoltage = HMSmain.readSensAndCondition();
  for (int i = 0; i < sizeof(readvoltage); i++)
  {
    char temp[100];
    sprintf(temp, "%s, %3f", voltageaverage, readvoltage[i]);
    voltageaverage = temp;
  }
} */

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause until serial console opens

  // debug("freeMemory()="+freeMemory());

  debugln();
  delay(1000);

  debugln("\n===================================");
  Hum.SetupSensor();
  HMSmain.setupSensor();
  debugf("HMS booting - please wait");
  debugf("Setup Complete");
  delay(100);
}

void accumulate_data(data_arrays &data)
{

  //Stack level data
  data.stack_humidity = Hum.StackHumidity();

  data.stack_temp = Hum.AverageStackTemp();

  //Cell level data
  float * cell_temp = Cell_Temp.read_temp_sensor_data();

  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    data.cell_temp[i] = cell_temp[i];
    debugln(data.cell_temp[i]);
  }

  float *cell_voltage = HMSmain.readSensAndCondition();

  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    data.cell_voltage[i] = cell_voltage[i];
    debugln(data.cell_voltage[i]);
  }

  data.stack_voltage = 0;
  for (int i = 0; i < Cell_Temp.GetSensorCount(); i++)
  {
    data.stack_voltage += cell_voltage[i];
  }
}

/* {
  float stack_humidity = Hum.Stack_humidity();
  float cell_voltage[] = {HMSmain.readSensAndCondition()};
  float stack_temp = Hum.average_Stack_temp();
  float *cell_temp[10];
  CellTemp.read_temp_sensor_data(*cell_temp);

  float stack_voltage = {0};
  for (int i = 0; i < sizeof(cell_voltage); i++)
  {
    stack_voltage += cell_voltage[i];
  }
  stack_voltage = stack_voltage / 10;
  return {stack_humidity, stack_temp, stack_voltage, cell_voltage, *cell_temp};
} */

void loop()
{
  /* CellTemp.read_temp_sensor_data();
  Hum.ReadSensor();
  HMSmain.readAmps();
  // ledtestOnOff(500); //comment out when not testing - Blink led from Unity Terminal over BTSerial
  delay(100);
  delay(100);
  debugCalibrateAmps(); // only needed for manual calibration of HalEffect Sensorsensor
  // debugf("Going to sleep now");
  delay(100);
  // debugln(freeRam()); */
}
