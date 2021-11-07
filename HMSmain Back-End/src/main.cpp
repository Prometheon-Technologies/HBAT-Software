#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HMS.h>
#include <Humidity.h>
#include <Battery.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEBUG 0

#if DEBUG == 1
#define debug(x) debug(x)
#define debugln(x) debugln(x)
#define debugf(x) debugf(x)
#define debugCalibrateAmps() HMSmain.calibrateAmps()

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugCalibrateAmps()
#endif

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// variable to hold device addresses
DeviceAddress Thermometer;

// Addresses of 3 DS18B20s CHANGE THESE WITH DEVICE ADDRESSES PRINTED TO CONSOLE
uint8_t sensor1[8] = {0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC};
uint8_t sensor2[8] = {0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27};
uint8_t sensor3[8] = {0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6};

int deviceCount = 0;

#define LED1 12
#define LED2 9

/* #define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
/* #define TIME_TO_SLEEP 30    */ /* Time ESP32 will go to sleep (in seconds) */

/* RTC_DATA_ATTR int bootCount = 0; */

// Method to print the reason by which ESP32
// has been awaken from sleep

/* void print_wakeup_reason()
{
  runningState = true;
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    debugf("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    debugf("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    debugf("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    debugf("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    debugf("Wakeup caused by ULP program");
    break;
  default:
    debugln("Wakeup was not caused by deep sleep: %d\n" + wakeup_reason);
    break;
  }
} */

HMS HMSmain = HMS();
Humidity Hum = Humidity();

int received;
Battery battery(1000, 3300, A0);

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

/* void SerialandBT(String data)
{
  bool BTwritestate = false;
  if (BTwritestate = true)
  {
    ledFade(LED2);
  }
  while (true)
  {
    BTwritestate = true;

    if (SerialBT.available())
    {
      SerialBT.print(data); //write on BT app
      Serial.write(SerialBT.read());
    }
    debug(data);
  }
} */

void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
  Serial.print((char)176);
  Serial.print("C  |  ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  Serial.print((char)176);
  Serial.println("F");
}

void floattostring()
{
  float *climatedata = Hum.ReadSensor();
  char climateData[100];
  sprintf(climateData, "%3d, %3d", climatedata[0], climatedata[1]);
  /* SerialandBT(climateData); */

  String voltageaverage = "";
  float *readvoltage = HMSmain.readSensAndCondition();
  for (int i = 0; i < sizeof(readvoltage); i++)
  {
    char temp[100];
    sprintf(temp, "%s, %3f", voltageaverage, readvoltage[i]);
    voltageaverage = temp;
  }
}

void setup()
{
  debugln("\n===================================");
  Serial.begin(115200);
  while (!Serial)
      delay(10); // will pause Zero, Leonardo, etc until serial console opens
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  debugln("Locating devices...");
  debug("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  debugln(" devices.");
  debugln("");
  debugln("Printing addresses...");
  for (int i = 0; i < deviceCount; i++)
  {
    debug("Sensor ");
    debug(i + 1);
    debug(" : ");
    sensors.getAddress(Thermometer, i);
    printAddress(Thermometer);
  }
  Hum.setupSensor();
  HMSmain.setupSensor();
  battery.begin(3300, 1.0, &sigmoidal);
  /* +bootCount;
  char bootChar[100];
  sprintf(bootChar, "Boot number: %s", String(bootCount));
  debugf(bootChar);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  //First we configure the wake up source
  //We set our ESP32 to wake up every 5 seconds

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  char sleeptime[1];
  sprintf(sleeptime, "Setup ESP32 to sleep for every ", String(TIME_TO_SLEEP) + " Seconds");
  debugf(sleeptime); */
  debugf("HMS booting - please wait");
  /* SerialBT.begin("ESP32_HMS"); */
  debugf("Device now Discoverable");
  // debugf(__FILE__);
  debugf("Setup Complete");
  delay(100);
  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //debugln("Configured all RTC Peripherals to be powered down in sleep");
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs*/
  // Serial.flush();
  /* if (!runningState)
  {
    esp_deep_sleep_start();
  } */
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void read_temp_sensor_data()
{
  sensors.requestTemperatures();
  
  Serial.print("Sensor 1: ");
  printTemperature(sensor1);
  
  Serial.print("Sensor 2: ");
  printTemperature(sensor2);
  
  Serial.print("Sensor 3: ");
  printTemperature(sensor3);
  
  Serial.println();
  delay(1000);
}

void loop()
{
  read_temp_sensor_data();
  Hum.ReadSensor();
  HMSmain.readAmps();
  // ledtestOnOff(500); //comment out when not testing - Blink led from Unity Terminal over BTSerial
  delay(100);
  floattostring();
  delay(100);
  debugCalibrateAmps(); // only needed for manual calibration of HalEffect Sensorsensor
  // debugf("Going to sleep now");
  delay(100);
}
