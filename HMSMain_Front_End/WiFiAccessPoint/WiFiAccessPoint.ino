#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include "index.html.h"
#include "data.json.h"

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

WebServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  Serial.println("Server started");



  server.on(F("/"), []() {
    server.send(200, "text/html", indexHtml);
  });

  
  server.on(F("/data.json"), []() {
    server.send(200, "application/json", jsonString);
  });

   server.on(F("/data.json"), []() {
    data_arrays = accumulate_data();
    server.send(200, "application/json", jsonText);
  });

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
}


String json_return_data(){

}
void loop() {
  server.handleClient();
  delay(0);
}
