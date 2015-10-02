#include <ESP8266WiFi.h>

//http://www.arduinesp.com/wifiwebserver
 
const char* ssid = "dlink942";
const char* password = "4166029277";
 
int ledPin = 5; // GPIO5
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
 
  digitalWrite(ledPin, LOW);
  delay(500);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
     
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  int timeout;
  timeout = 500;
  while(!client.available()){
    delay(1);
    timeout--;
    if (timeout == 0) return;
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = digitalRead(ledPin);
  
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.print("HTTP/1.1 200 OK\n\
  Content-Type: text/html\n\n\
  <!DOCTYPE HTML>\n\
  <html>\n\
  PIN5=,");
  if(value == HIGH) {
    client.print("1");
  } else {
    client.print("0");
  }
  client.print(",<br><br>\n\
  Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>\n\
  Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>\n");
   
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}

