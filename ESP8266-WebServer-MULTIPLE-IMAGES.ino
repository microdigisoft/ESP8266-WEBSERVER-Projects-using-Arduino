//Include all required libraries for ESP32 board
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#else
#include <Arduino.h>  //Include all required libraries for ESP8266 board
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#endif

// Replace with your network credentials
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxx";

// Create AsyncWebServer object
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <center><h2>ESP32/ ESp8266 Image Webserver</h2>
  <img src="ball">
  <img src="Bat">
  <img src="football">
  <img src="tennisball">
  <img src="tennisracket">
  </center> 
</body>  
</html>)rawliteral";
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/ball", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/ball.jfif", "image/jfif");
  });
  server.on("/Bat", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/Bat.jfif", "image/jfif");
  });
  server.on("/football", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/football.jfif", "image/jfif");
  });
  server.on("/tennisball", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/tennisball.jfif", "image/jfif");
  });
  server.on("/tennisracket", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/tennisracket.jfif", "image/jfif");
  });
  server.begin();
}

void loop() {

}
