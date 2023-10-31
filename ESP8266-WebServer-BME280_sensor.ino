#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

// Replace with your network credentials
const char* ssid = "xxxxxxx";
const char* password = "xxxxx";

/*#include <SPI.h>  // uncomment his if you are using SPI interface
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/
const int ledPin1 = 5;
const int ledPin2 = 6;
const int ledPin3 = 7;
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

float temperature;
float humidity;
float pressure;

AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;  // send readings timer

void getBME280Readings(){
 
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  humidity = bme.readHumidity();
}

String processor(const String& var){
  getBME280Readings();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    digitalWrite(ledPin3, HIGH); 
    return String(temperature);
  }
  else if(var == "HUMIDITY"){
    digitalWrite(ledPin2, HIGH); 
    return String(humidity);
  }
  else if(var == "PRESSURE"){
    digitalWrite(ledPin1, HIGH); 
    return String(pressure);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>BME280 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 10px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 500px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.temperature { color: red; }
    .card.humidity {color: yellow; }
    .card.pressure { color: blue; }
    .card.gas { color: #d62246; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>BME280 WEB SERVER</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h5><i class="fas fa-thermometer-half"></i> TEMPERATURE</h5><p><a href=""><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></a></p><br>
      </div>
      <div class="card humidity">
        <h5><i class="fas fa-tint"></i> HUMIDITY</h5><p><a href=""><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></a></p><br>
      </div>
      <div class="card pressure">
        <h5><i class="fas fa-angle-double-down"></i> PRESSURE</h5><p><a href=""><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></a></p><br>
      </div>
    </div>
  </div>
<script>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);
pinMode(ledPin1, OUTPUT);
pinMode(ledPin2, OUTPUT);
pinMode(ledPin3, OUTPUT);
  // Set the device as a Station and Soft Access Point simultaneously
  //WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not detect a BME280 sensor, Fix wiring Connections!");
    while (1);
  }
  
  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);

server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
   digitalWrite(ledPin3, HIGH); 
    request->send_P(200, "text/plain", index_html, processor);
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
     digitalWrite(ledPin2, HIGH); 
    request->send_P(200, "text/plain", index_html, processor);
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
     digitalWrite(ledPin1, HIGH); 
    request->send_P(200, "text/plain", index_html, processor);
  });


    
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getBME280Readings();
    Serial.printf("Temperature = %.2f ºC \n", temperature);
    Serial.printf("Humidity = %.2f % \n", humidity);
    Serial.printf("Pressure = %.2f hPa \n", pressure);
    Serial.println();

    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature).c_str(),"temperature",millis());
  
    events.send(String(humidity).c_str(),"humidity",millis());
  
    events.send(String(pressure).c_str(),"pressure",millis());
    
    lastTime = millis();
  }
}
