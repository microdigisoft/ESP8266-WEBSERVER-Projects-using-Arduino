#include < ESP8266WiFi.h>
#include < WiFiClient.h>
#include < ESP8266WebServer.h>
const char* ssid = "microdigisoft";    //  Your Wi-Fi Name
const char* password = "microdigisoft";   // Wi-Fi Password
ESP8266WebServer server(80);
// Variable to store the HTTP request
String header;


// Auxiliar variables to store the current output state
String output12State = "off";
// Assign output variables to Wemos D1 Wifi pin number 13
const int output12 = 12;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output12, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output12, LOW);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client Connected.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == 'n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("RED LED ON at Pin 12");
              output12State = "on";
              digitalWrite(output12, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("RED LED OFF at Pin 12");
              output12State = "off";
              digitalWrite(output12, LOW);
            }
            // Display the HTML web page
            client.println("< html>");
            client.println("< head >< meta name="viewport"" content=""width=device-width
