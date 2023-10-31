#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>


const char* ssid = "Your_SSID"; //Write your SSID
const char* password = "Your_Password";   //Write your password

const char* input_parameter = "state";

const int output = 2;

const int Push_button_GPIO = 13 ;

// Variables will change:
int LED_state = LOW;         
int button_state;             
int lastbutton_state = LOW;   

unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;    

AsyncWebServer server(80);

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP8266-NodeMCU Momentary Webserver</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Times New Roman; text-align: center; margin:0px auto; padding-top: 30px;}
      /**************************
  DEFAULT BOOTSTRAP STYLES
**************************/
.btn {
  display: inline-block;
  padding: 6px 12px;
  margin-bottom: 0;
  font-size: 14px;
  font-weight: normal;
  line-height: 1.42857143;
  text-align: center;
  white-space: nowrap;
  vertical-align: middle;
  cursor: pointer;
  -webkit-user-select: none;
     -moz-user-select: none;
      -ms-user-select: none;
          user-select: none;
  background-image: none;
  border: 1px solid transparent;
  border-radius: 4px;
  padding: 10px 16px;
}
  
.btn-lg {
  font-size: 18px;
  line-height: 1.33;
  border-radius: 6px;
}

.btn-primary {
  color: #fff;
  background-color: #428bca;
  border-color: #357ebd;
}

.btn-primary:hover,
.btn-primary:focus,
.btn-primary:active,
.btn-primary.active,
.open .dropdown-toggle.btn-primary {
  color: #fff;
  background-color: #3276b1;
  border-color: #285e8e;
}

/***********************
  ROUND BUTTONS
************************/
.round {
  border-radius: 24px;
}

/***********************
  CUSTON BTN VALUES
************************/

.btn {
  padding: 14px 24px;
  border: 0 none;
  font-weight: 700;
  letter-spacing: 1px;
  text-transform: uppercase;
}
.btn:focus, .btn:active:focus, .btn.active:focus {
  outline: 0 none;
}

.btn-primary {
  background: #0099cc;
  color: #ffffff;
}
.btn-primary:hover, .btn-primary:focus, .btn-primary:active, .btn-primary.active, .open > .dropdown-toggle.btn-primary {
  background: #FF0000;
}
.btn-primary:active, .btn-primary.active {
  background: #008000;
  box-shadow: none;
}
     // .button:hover {background-color: #FF0000 }
     // .button:active {
      //  background-color: #008000;
      //  transform: translateY(2px);
      //}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #FF0000; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #27c437}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}


      
    </style>
  </head>
  <body>
    <center><h1>ESP8266-NodeMCU Momentary Webserver</h1></center>
    <button class="btn btn-primary btn-lg round" onmousedown="toggleCheckbox('ON');" ontouchstart="toggleCheckbox('ON');" onmouseup="toggleCheckbox('OFF');" ontouchend="toggleCheckbox('OFF');">Momentary Switch: Press to Turn ON/OFF</button>
   %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?state=1", true); }
  else { xhr.open("GET", "/update?state=0", true); }
  xhr.send();
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var inputChecked;
      var outputStateM;
      if( this.responseText == 0){ 
        inputChecked = true;
        outputStateM = "ON";
      }
      else { 
        inputChecked = false;
        outputStateM = "OFF";
      }
      document.getElementById("output").checked = inputChecked;
      document.getElementById("outputState").innerHTML = outputStateM;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 1000 ) ;
</script>

  </body>
</html>)rawliteral";


String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    String outputStateValue = outputState();
    buttons+= "<h4>Switch State: <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
  return "";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(output, OUTPUT);
  digitalWrite(output, HIGH);
  pinMode(Push_button_GPIO, INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }


  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String input_message;
    String inputParameter;
    // GET input1 value on <ESP_IP>/update?state=<input_message>
    if (request->hasParam(input_parameter)) {
      input_message = request->getParam(input_parameter)->value();
      inputParameter = input_parameter;
      digitalWrite(output, input_message.toInt());
      LED_state = !LED_state;
    }
    else {
      input_message = "No message sent";
      inputParameter = "none";
    }
    Serial.println(input_message);
    request->send(200, "text/plain", "OK");
  });

  
  server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(output)).c_str());
  });
  // Start server
  server.begin();
}
  
void loop() {
  int data = digitalRead(!Push_button_GPIO);

  if (data != lastbutton_state) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (data = !button_state) {
      button_state = data;


      if (button_state == LOW) {
      

        LED_state = button_state;
        
      }
    }
  }

  
  digitalWrite(output, !LED_state);


  lastbutton_state = data;
}
