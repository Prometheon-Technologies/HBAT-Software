#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include "index.html.h"
#include "data.json.h"

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "H-BAT-" + (String)WiFi.macAddress();
const char *password = "hbat";

WebServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  connectToApWithFailToStation();




  Serial.println("Server started");



  server.on(F("/"), []() {
    server.send(200, "text/html", indexHtml);
  });


  
  server.on(F("/wifiUpdate"), []() {
    //Place code here to setup wifi connectivity
    String NewApName = server.arg("apName");
    String NewApPass = server.arg("apPass");
    server.send(200, "application/json", jsonString);
  });

  
  server.on(F("/data.json"), []() {
    server.send(200, "application/json", jsonString);
  });

   server.on(F("/data.json"), []() {
    data_arrays = accumulate_data();
    server.send(200, "application/json", jsonText);
  });

  server.begin();
  Serial.println("HTTP server started");
}


String json_return_data(){

}
void loop() {
  server.handleClient();
  delay(0);
}

void connectToApWithFailToStation(String WIFI_STA_SSID, String WIFI_STA_PASS)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);

  int numberOfAttempts = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
    if (numberOfAttempts > 50)
    {
      WiFi.mode(WIFI_AP);
      // You can remove the password parameter if you want the AP to be open.
      WiFi.softAP(ssid, password);
      Serial.print("Wifi Connect Failed. \r\nStarting AP. \r\nAP IP address: ");
      Serial.println(WiFi.softAPIP());
      break;
    }
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}