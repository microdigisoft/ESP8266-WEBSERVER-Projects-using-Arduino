#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/events", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/event-stream", "data: Welcome\n\n");
  });

  server.begin();
}

void loop() {
  server.send(200, "text/event-stream", "data: Hello\n\n");
  delay(1000);
}
