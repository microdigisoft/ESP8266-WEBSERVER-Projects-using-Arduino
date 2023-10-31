#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Replace with your network credentials
const char* ssid = "xxxxxxx";
const char* password = "xxxxx";


// Define NTP Server details
const char* ntpServerName = "pool.ntp.org";
const int timeZone = 19800;  // Replace with your timezone offset in hours
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)

// Define UDP port for NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, timeZone,daylightOffset_sec);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  timeClient.begin();
}

void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());
  
  delay(1000); // Wait for 1 second
}
