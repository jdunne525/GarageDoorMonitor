#include <ESP8266WiFi.h>

//http://www.arduinesp.com/wifiwebserver
 
const char* ssid = "dlink942";
const char* password = "4166029277";
 
int LEDPin = 5;
int openClosePin = 4;
int statusPin = 13;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long DoorActionDuration = 20000;   //door takes this long to open / close
WiFiServer server(80);
 
void setup() {
  int i;
  Serial.begin(115200);
  pinMode(openClosePin, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(statusPin, INPUT);
  
  for (i = 0; i < 10; i++) {
    if (digitalRead(LEDPin)) digitalWrite(LEDPin, LOW);
    else digitalWrite(LEDPin, HIGH);
    delay(500);
  }
       
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
  digitalWrite(LEDPin, HIGH);
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
 
  if (request.indexOf("/DOOR=CLOSE") != -1)  {
    DoorAction(1);
  }
  if (request.indexOf("/DOOR=OPEN") != -1)  {
    DoorAction(0);
  }
 
  // Return the response
  client.print("HTTP/1.1 200 OK\n\
  Content-Type: text/html\n\n\
  <!DOCTYPE HTML>\n\
  <html>\n\
  DOOR=,");

  
  if(IsDoorClosed()) {
    client.print("CLOSED");
  }
  else {
    client.print("OPEN");
  }
  client.print(",<br><br>\n\
  Click <a href=\"/DOOR=OPEN\">here</a> Open the door.<br>\n\
  Click <a href=\"/DOOR=CLOSE\">here</a> Close the door.<br>\n");
   
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}


void  DoorAction(boolean CloseRequest) {
  currentMillis = millis();
  if (currentMillis - previousMillis >= DoorActionDuration) {
    previousMillis = currentMillis;

    if (CloseRequest && !IsDoorClosed()) {
      Serial.println("Closing door");
      SignalDoor();
    }
    else if (!CloseRequest && IsDoorClosed()) {
      Serial.println("Opening door");
      SignalDoor();
    }
    else {
      Serial.println("Door already in desired state");
    }
  }
  else {
    Serial.println("Unable to initiate door action.  Wait at least 20 seconds between action requests.");
  }
}

void  SignalDoor() {
  digitalWrite(openClosePin, HIGH);
  delay(500);
  digitalWrite(openClosePin, LOW);
}

boolean IsDoorClosed() {
  return digitalRead(statusPin);
}



