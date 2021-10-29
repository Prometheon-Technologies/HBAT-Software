/*
asyncserver.cpp - HBAT asyncserver library
 Copyright (c) 2021 Zacariah Austin Heim.

 File is inherently in Debug Mode. Change #define DEBUG to 0 to disable 
 */
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) debugln(x)
#define debugf(x) Serial.printf(x)

#else
#define debug(x)
#define debugln(x)
#define debugf(x)
#endif

const char* ssid = " "; //SSID of your network
const char* password =  " "; //password of your network

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

  if(type == WS_EVT_CONNECT){

    debugln("Websocket client connection received");
    client->text("This is sample text");

  } else if(type == WS_EVT_DISCONNECT){
    debugln("Client disconnected");

  }
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debugln("Connecting to WiFi..");
  }

  debugln(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
}

void loop(){}