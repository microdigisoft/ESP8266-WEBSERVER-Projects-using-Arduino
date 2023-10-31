#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <LiquidCrystal.h>  
//LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);
 #include <LiquidCrystal_I2C.h>
AsyncWebServer server(80);
 LiquidCrystal_I2C lcd(0x27,16,2);
const char* ssid = "xxxxxxxx";  //wifi ssid
const char* password = "xxxxxxx";   //wifi password
 
const char* PARAM_INPUT_1 = "input1";
 
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Smart Notice Board</title>
  <meta name="viewport" content="width=device-width, initial-scale=5">
<p> <font size="7" face="sans-serif"> <marquee> Smart Notice Board </marquee> </font> </p>
  </head><body><center>
<h1>......WELL-COME TO MICRODIGISOFT..... </h1><br>
  <form action="/get">
    Enter Text to Display: <input type="text" name="input1">
    <input type="submit" value="Send">
  </form><br>
 
</center></body></html>)rawliteral";
 
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
 
void setup() {
  Serial.begin(115200);
   //lcd.begin(16, 2);
   lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.print("");
   //lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Smart Notice Board");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
 
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_1)) {
      message = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
       //lcd.clear();
       lcd.setCursor(0,0);
      
      lcd.print(message);
    }
    else {
      message = "No message sent";
      inputParam = "none";
    }
    Serial.println(message);
   
  request->send(200, "text/html", index_html);
  });
  server.onNotFound(notFound);
  server.begin();
}
 
void loop() {
    for (int positionCounter = 0; positionCounter < 29; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(1000);
  }
}
