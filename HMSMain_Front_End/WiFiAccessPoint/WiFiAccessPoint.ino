#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include "SPIFFS.h"
#include "index.html.h"



#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const String ssid = "H-BAT-" + String(WiFi.macAddress());
const String password = "hbat";

WebServer server(80);



struct AccumulateData
{
  float stack_humidity;
  float stack_temp;
  float stack_voltage;
  float cell_temp[10];
  float cell_voltage[10];
};
AccumulateData dataTosend;







void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  dataTosend.stack_humidity = 10;
  dataTosend.stack_temp = 11;
  dataTosend.stack_voltage = 12;
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








  connectToApWithFailToStation("", "");




  Serial.println("Server started");



  server.on(F("/"), []() {
    server.send(200, "text/html", indexHtml);
  });



  server.on(F("/wifiUpdate"), []() {
    //Place code here to setup wifi connectivity
    String NewApName = server.arg("apName");
    String NewApPass = server.arg("apPass");
    server.send(200, "application/json", "yay");
  });


  server.on(F("/data.json"), []() {
    String json = "";
    json += R"====({)====";

    json += R"====("stack_humidity":)====";
    json += (String)dataTosend.stack_humidity + ",";

    json += R"====("stack_temp":)====";
    json += (String)dataTosend.stack_temp + ",";

    json += R"====("stack_voltage":)====";
    json += (String)dataTosend.stack_voltage + ",";

    json += R"====("fakeGraphData":[)====";
    for (int i = 0; i <= 10; i++) {
      json += R"====({"label": "🌡 )====" + (String)i + '",';
      json += R"====("type": "temp",)====";
      json += R"====({"value": )====" + (String)dataTosend.cell_temp[i] + '",';
            
            
      json += R"====("maxValue": 80},)====";


      
      //doc["cell_temp"][0] = (String)dataTosend.cell_temp[i];
      //doc["cell_voltage"][0] = (String)dataTosend.cell_voltage[i];
      delay(0);
      json += R"====(})====";
      if(i>10){json += R"====(,)====";};
    }
    json += R"====(])====";
    json += R"====(})====";
    server.send(200, "application/json", json);
  });



  server.begin();
  Serial.println("HTTP server started");
}


String json_return_data() {

}
void loop() {
  server.handleClient();
  delay(0);
}

void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_SSID.c_str(), WIFI_STA_PASS.c_str());

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
    if (numberOfAttempts > 50)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      WiFi.softAP(ssid.c_str(), password.c_str());
      Serial.print("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      Serial.println(WiFi.softAPIP());
      break;
    }
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}
