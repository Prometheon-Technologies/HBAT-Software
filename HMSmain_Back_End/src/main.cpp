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
#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

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

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

WebServer server(80);


Struct data_arrays
{
  float stack_humidity;
  float stack_temp;
  float stack_voltage;
  float cell_temp[10];
  float cell_voltage[10];
  //float humidity_temp[2]
}

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

data_arrays accumulate_data()
{
  float stack_humidity = Hum.Stackhumidity();
  float stack_temp = Hum.Stacktemp();
  float cell_voltage[10] = HMSmain.readSensAndCondition();
  float cell_temp[10] = CellTemp.cell_temp_sensor_results();
  float stack_voltage = 0;

  for (int i = 0; i < 10; i++)
  {
    stack_voltage += cell_voltage[i]; 
  }
   stack_voltage = stack_voltage / 10;
  return 
  {
    stack_humidity;
    stack_temp;
    stack_voltage;
    cell_temp[10];
    cell_voltage[10];
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause until serial console opens

  //  server.on(UriBraces("/users/{}"), []() {
  //    String user = server.pathArg(0);
  //    server.send(200, "text/plain", "User: '" + user + "'");
  //  });
  //
  //  server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []() {
  //    String user = server.pathArg(0);
  //    String device = server.pathArg(1);
  //    server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
  //  });

  server.begin();
  Serial.println("HTTP server started");

  // Serial.println("Configuring RTC...");
  // debug("freeMemory()="+freeMemory());

  debugln();
  delay(1000);

  debugln("\n===================================");
  CellTemp.setup_sensors();
  Hum.setupSensor();
  HMSmain.setupSensor();
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
  // pinMode(LED1, OUTPUT);
  // pinMode(LED2, OUTPUT);
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
