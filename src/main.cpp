#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTTYPE DHT22
#define BUFFER_SIZE 7

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t DHTPin = D2; 
DHT dht(DHTPin, DHTTYPE); /* Serial communication */

float Temperature;
float Humidity;
char BUF[7];

const char* ssid = "******";  // Enter SSID here
const char* password = "*******";  //Enter Password here
const char* mqttServer = "*******";    // IP adress Raspberry Pi
const int mqttPort = 1883; // Enter mqtt port
const char* temperature_topic = "dht22/temp"; // topic 1
const char* humidity_topic_topic = "dht22/hum"; // topic 2
const long interval = 10000; // measure every 10 seconds

unsigned long previousMillis = 0;

void setup(){
  Serial.begin(115200);
  delay(100);
  
  pinMode(DHTPin, INPUT);

  dht.begin(); 

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client")) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
  client.subscribe("dht22/temp");
  client.subscribe("dht22/hum");
}

void loop(){
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval){
    
    previousMillis = currentMillis;
    Temperature = dht.readTemperature(); // reading temperature from sensor
    Humidity = dht.readHumidity(); // reading humidity from sensor
   
    char buffer_temp[BUFFER_SIZE];
    //1 char for the sign, 1 char for the decimal dot, 4 chars for the value & 1 char for null termination
    dtostrf(Temperature, BUFFER_SIZE - 1 /*width, including the decimal dot and minus sign*/, 2 /*precision*/, buffer_temp);
    client.publish("dht22/temp", buffer_temp, BUFFER_SIZE);
  
    char buffer_hum[BUFFER_SIZE];
    dtostrf(Humidity, BUFFER_SIZE - 1 /*width, including the decimal dot and minus sign*/, 2 /*precision*/, buffer_hum);
    client.publish("dht22/hum", buffer_hum, BUFFER_SIZE);
    
    Serial.println("------------------------");
    Serial.printf("Temperature: %f\n", Temperature);
    Serial.printf("Humidity: %f\n", Humidity);
    Serial.println("------------------------");
    
  }

}

