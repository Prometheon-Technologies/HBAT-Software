#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HMS.h>
#include <Humidity.h>
#include <celltemp.h>
//#include <MemoryFree.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <index.html.h>
#include <data.json.h>

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
CELLTEMP CellTemp = CELLTEMP();

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

  Serial.println("HTTP server started");

  // Serial.println("Configuring RTC...");
  // debug("freeMemory()="+freeMemory());

  debugln();
  delay(1000);

  debugln("\n===================================");
  CellTemp.setup_sensors();
  Hum.setupSensor();
  HMSmain.setupSensor();
  debugf("HMS booting - please wait");
  debugf("Device now Discoverable");
  // debugf(__FILE__);
  debugf("Setup Complete");
  delay(100);
}

data_arrays accumulate_data()
{
  float stack_humidity = Hum.Stackhumidity();
  float *cell_voltage[10] = {HMSmain.readSensAndCondition()};
  float stack_temp = Hum.Stacktemp();
  float cell_temp[10] = {CellTemp.read_temp_sensor_data()};
  float stack_voltage = {0};

  for (int i = 0; i < 10; i++)
  {
    stack_voltage += *cell_voltage[i];
  }
  stack_voltage = stack_voltage / 10;
  return;
  {
    stack_humidity;
    stack_temp;
    stack_voltage;
    cell_temp[10];
    cell_voltage[10];
  }
}

void loop()
{
  /* CellTemp.read_temp_sensor_data();
  Hum.ReadSensor();
  HMSmain.readAmps();
  // ledtestOnOff(500); //comment out when not testing - Blink led from Unity Terminal over BTSerial
  delay(100);
  stack_climate();
  delay(100);
  debugCalibrateAmps(); // only needed for manual calibration of HalEffect Sensorsensor
  // debugf("Going to sleep now");
  delay(100);
  // debugln(freeRam()); */
}
