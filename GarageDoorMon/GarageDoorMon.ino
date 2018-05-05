#include <ESP8266WiFi.h>

//http://www.arduinesp.com/wifiwebserver
 
const char* ssid = "dlink942_2GEXT";
const char* password = "4166029277";
 
int LEDPin = 5;
int openClosePin = 4;
int statusPin = 13;

boolean DoorStateIsClosed = true;
unsigned long currentMillis = 0;
unsigned long DoorActionMillis = 0;        // will store last time the door was opened or closed
unsigned long DoorActionDuration = 20000;   //door takes this long to open / close
unsigned long DoorOpenMillis = 0;
unsigned long DoorOpenedTime = 0;
unsigned long DoorClosedMillis = 0;
unsigned long DoorClosedTime = 0;

void  DoorAction(boolean CloseRequest);
void  SignalDoor(void);
boolean IsDoorClosed(void);

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

  DoorStateIsClosed = !digitalRead(statusPin);   //preload the state of this flag (opposite of the correct state)
  IsDoorClosed(); 
}
 
void loop() {

  if(IsDoorClosed()) {
  
    currentMillis = millis();
    DoorClosedTime = currentMillis - DoorClosedMillis;
    DoorClosedTime = DoorClosedTime / 60000;          //this will be time in minutes
  }
  else {
    currentMillis = millis();
    DoorOpenedTime = currentMillis - DoorOpenMillis;
    DoorOpenedTime = DoorOpenedTime / 60000;          //this will be time in minutes
  }
  
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
    client.print("CLOSED, Duration = ,");
    client.print(DoorClosedTime);
  }
  else {
    client.print("OPEN, Duration = ,");
    client.print(DoorOpenedTime);
  }
  
  client.print(",<br><br>\n\
  Click <a href=\"/DOOR=OPEN\">here</a> Open the door.<br><br>\n\n\
  Click <a href=\"/DOOR=CLOSE\">here</a> Close the door.<br>\n");
   
  delay(1);
  Serial.println("Client disonnected");
  Serial.println(""); 
}


void  DoorAction(boolean CloseRequest) {
  currentMillis = millis();
  if (currentMillis - DoorActionMillis >= DoorActionDuration) {
    DoorActionMillis = currentMillis;

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

void  SignalDoor(void) {
  digitalWrite(openClosePin, HIGH);
  delay(500);
  digitalWrite(openClosePin, LOW);
}

boolean IsDoorClosed(void) {
  boolean NewDoorState;

  NewDoorState = digitalRead(statusPin);
  
  if (DoorStateIsClosed) {
    if (!NewDoorState) {
      //Door was closed and just became open
      DoorStateIsClosed = false;
      DoorOpenMillis = millis();
    }
    else {
      //Door was closed and remains closed.
    }
  }
  else {
    if (NewDoorState) {
      //Door was open and just became closed
      DoorStateIsClosed = true;
      DoorClosedMillis = millis();
    }
    else {
      //Door was open and remains open.
    }
  }
  return NewDoorState;
}



