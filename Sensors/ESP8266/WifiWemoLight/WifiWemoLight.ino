#include <ESP8266WiFi.h> 

const int inputPin = 15;

char* host ="*.*.*.*";
int port = 49153;

const char* ssid = "*";
const char* password = "*";

int wemo_status=0;
String wemo_on="<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>";
String wemo_off="<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>0</BinaryState></u:SetBinaryState></s:Body></s:Envelope>";

void setup() {
  pinMode(buttonPin, INPUT);


}

void loop() {
  int val = digitalRead(inputPin);
  Serial.print("Button is on ");
  Serial.println(val);
  
  if (val == HIGH) {
    wemo_status = 1;
    Serial.println("Turning WeMo On...");
  }
  if (val == LOW) {
    wemo_status = 0;
    Serial.println("Turning WeMo Off...");
  }
  
  Serial.println();
  wemo_control(wemo_status);
  delay(5000);
}

void wemo_control(int cmd) {
  Serial.print("Connecting to ");
  Serial.println(host);
 
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    return;
  }

  // This will send the request to the server
  client.println("POST /upnp/control/basicevent1 HTTP/1.1");
  client.println("Host: " + String(host) + ":" + String(port));
  client.println("User-Agent: ESP8266/1.0");
  client.println("Connection: close");
  client.println("Content-type: text/xml; charset=\"utf-8\"");
  client.print("Content-Length: ");
  if (cmd == 1) {
    client.println(wemo_on.length()); // both wemo_on and wemo_off are the same length, just in case it changes in the future
  }
  else {
     client.println(wemo_off.length());
  }
  client.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
  client.println();
  if (cmd == 1) {
      client.println(wemo_on);
  }
  else {
      client.println(wemo_off);
  }
  delay(10);
 
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
 
  Serial.println();
  Serial.println("Closing connection");
}
