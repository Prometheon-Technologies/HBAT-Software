#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include "SPIFFS.h"
#include "index.html.h"

#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const String ssid = "H-BAT-" + String(WiFi.macAddress());
const String password = "hbathbat";

WebServer server(80);

const int maxVolatage = 24;
const int maxTemp = 100;

struct AccumulateData
{
  boolean relays[5];
  float stack_humidity;
  float stack_temp;
  float stack_voltage;
  float cell_temp[10];
  float cell_voltage[10];
};
AccumulateData dataTosend;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  dataTosend.stack_humidity = 60;
  dataTosend.stack_temp = 120;
  dataTosend.stack_voltage = 24;
  dataTosend.cell_temp[0] = 13;
  dataTosend.cell_temp[1] = 14;
  dataTosend.cell_temp[2] = 15;
  dataTosend.cell_temp[3] = 16;
  dataTosend.cell_temp[4] = 17;
  dataTosend.cell_temp[5] = 18;
  dataTosend.cell_temp[6] = 19;
  dataTosend.cell_temp[7] = 20;
  dataTosend.cell_temp[8] = 21;
  dataTosend.cell_temp[9] = 22;
  dataTosend.cell_voltage[0] = 23;
  dataTosend.cell_voltage[1] = 24;
  dataTosend.cell_voltage[2] = 25;
  dataTosend.cell_voltage[3] = 26;
  dataTosend.cell_voltage[4] = 27;
  dataTosend.cell_voltage[5] = 28;
  dataTosend.cell_voltage[6] = 29;
  dataTosend.cell_voltage[7] = 30;
  dataTosend.cell_voltage[8] = 41;
  dataTosend.cell_voltage[9] = 42;
  dataTosend.relays[2] = 1;

  connectToApWithFailToStation("", "");

  Serial.println("Server started");

  server.on(F("/"), []()
            { server.send(200, "text/html", indexHtml); });

  server.on(F("/wifiUpdate"), []()
            {
    //Place code here to setup wifi connectivity
    String NewApName = server.arg("apName");
    String NewApPass = server.arg("apPass");
    server.send(200, "application/json", "yay");
    connectToApWithFailToStation(NewApName, NewApPass); });

  server.on(F("/toggle"), []()
            {
    //Place code here to setup wifi connectivity
    int pinToToggle = server.arg("pin").toInt();
    Serial.print("switching state of pin :");
    Serial.println(pinToToggle);
    dataTosend.relays[pinToToggle] = (dataTosend.relays[pinToToggle] == true) ? false : true;
    server.send(200, "application/json", "toggled"); });

  server.on(F("/data.json"), []()
            {
    String json = "";
    json += R"====({)====";

    json += R"====("stack_humidity":)====";
    json += (String)dataTosend.stack_humidity + ",\n";

    json += R"====("stack_temp":)====";
    json += (String)dataTosend.stack_temp + ",\n";

    json += R"====("relays":[)====";
    json += (String) dataTosend.relays[0] + "," + (String) dataTosend.relays[1] + "," +  (String) dataTosend.relays[2] + "," +  (String) dataTosend.relays[3] + "," +  (String) dataTosend.relays[4] + "],\n";

    json += R"====("stack_voltage":)====";
    json += (String)dataTosend.stack_voltage + ",\n";

    json += R"====("fakeGraphData":[)====";
    json +=  "\n";
    for (int i = 0; i < 10; i++) {

      delay(0);
      json += R"====({"label": "🌡 )====" + (String)i + "\",\n";
      json += R"====("type": "temp",)====" + (String)"\n";
      json += R"====("value": )====" + (String)dataTosend.cell_temp[i] + (String)",\n";
      json += R"====("maxValue": )====" + (String) maxTemp;
      json += R"====(})====" + (String)"\n";
      json += R"====(,)====";

      json += R"====({"label": "⚡ )====" + (String)i + "\",\n";
      json += R"====("type": "volt",)====" + (String)"\n";
      json += R"====("value": )====" + (String)dataTosend.cell_voltage[i] + (String)",\n";
      json += R"====("maxValue": )====" + (String) maxVolatage;
      json += R"====(})====" + (String)"\n";

      if (i < 9) {
        json += R"====(,)====";
      };
    }
    json += R"====(])====";
    json += R"====(})====";
    server.send(200, "application/json", json);
    json = ""; });

  server.begin();
  Serial.println("HTTP server started");
}

String json_return_data()
{
}
void loop()
{
  server.handleClient();
  delay(0);
}

void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS)
{
  WiFi.persistent(true);
  Serial.println("Configuring access point...");
  Serial.print("WIFI_STA_SSID:");
  Serial.println(WIFI_STA_SSID);
  Serial.print("WIFI_STA_PASS:");
  Serial.println(WIFI_STA_PASS);

  WiFi.mode(WIFI_STA);
  if (WIFI_STA_SSID == "")
  {
    WiFi.reconnect();
  }
  else
  {
    WiFi.begin(WIFI_STA_SSID.c_str(), WIFI_STA_PASS.c_str());
  }

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    numberOfAttempts++;
    delay(200);
    Serial.print(".");
    if (numberOfAttempts > 100)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      WiFi.softAP(ssid.c_str(), password.c_str());
      Serial.print("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      Serial.println(WiFi.softAPIP());
      return;
      break;
    }
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}
