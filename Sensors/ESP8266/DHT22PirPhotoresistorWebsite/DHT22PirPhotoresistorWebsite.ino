#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>

#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321

const char* ssid = "*****";
const char* password = "*****";

WiFiServer server(80);

const int DHTPin = 5;
DHT dht(DHTPin, DHTTYPE);
const int PIRPin = 16;


const String PhotoresistorPin = "A0"; 

static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
static String lightCondition;
static bool pirData;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting sensors");
  dht.begin();
  pinMode(PIRPin, INPUT);
  
  // Connecting to WiFi network
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
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());

}

void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = dht.readTemperature(true);
            // Read the input on analog pin 0;
            int photocellReading = analogRead(A0);
            int val = digitalRead(PIRPin);
            
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) || isnan(f) || isnan(photocellReading) || isnan(val)) {
              Serial.println("Failed to read from sensors!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);
              dtostrf(h, 6, 2, humidityTemp);  

              // Computes string value of light conditions
              if (photocellReading < 10) {
                lightCondition = "Dark";
            } else if (photocellReading < 200) {
                lightCondition = "Dim";
            } else if (photocellReading < 500) {
                lightCondition = "Light";
            } else if (photocellReading < 800) {
                lightCondition = "Bright";
            } else {
                lightCondition = "Very bright";
            } 

              if(val == HIGH)
              {
                pirData = true;
              }
              else if(val == LOW)
              {
                pirData = false;
              }
              
              // You can delete the following Serial.print's, it's just for debugging purposes
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F \t");
              Serial.print("Light Condition: ");
              Serial.print(lightCondition);
              Serial.print(" \t Motion Detected: ");
              Serial.print(pirData?"true ":"false ");
              
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature and humidity
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>Huda's Room Data</h1><h3>Temperature in Celsius: ");
            client.println(celsiusTemp);
            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
            client.println(fahrenheitTemp);
            client.println("*F</h3><h3>Humidity: ");
            client.println(humidityTemp);
            client.println("%</h3><h3> Light Condition: ");
            client.println(lightCondition);
            client.println("</h3><h3> Motion Detected: ");
            client.println(pirData?"true":"false");
            client.println("</h3>");
            client.println("</body></html>");     
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}   

float Light (){
float Vout= analogRead(A0) *0.0048828125;
//int lux=500/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
float lux=(2500/Vout-500)/10;
return lux;
}
