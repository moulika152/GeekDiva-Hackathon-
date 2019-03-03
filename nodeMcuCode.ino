#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "DHT.h"
#define DHTPIN D2     // what pin we're connected to
#define DHTTYPE DHT11
DHT dht (D2, DHTTYPE);
int UpperThreshold = 590;
int LowerThreshold = 490;
int reading = 0;
float BPM = 0.0;
String data;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 1000;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
float humidity, temperature;
WiFiClient client;
const int httpPort = 80;
const char* host = "api.thingspeak.com";
const char* privateKey="KLAOZ1VAXFVWGGEP";
const char* ssid = "cva"; //Enter SSID
const char* password = "shivanicva";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP addres
  pinMode(A0, INPUT);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  Serial.println("connected to host");
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();  
  Serial.println("Humidity");
  Serial.println("temperature");
  Serial.println(humidity);
  Serial.println(temperature);
  reading = analogRead(0); 

      // Heart beat leading edge detected.
      if(reading > UpperThreshold && IgnoreReading == false){
        if(FirstPulseDetected == false){
          FirstPulseTime = millis();
          FirstPulseDetected = true;
        }
        else{
          SecondPulseTime = millis();
          PulseInterval = SecondPulseTime - FirstPulseTime;
          FirstPulseTime = SecondPulseTime;
        }
        IgnoreReading = true;
      }

      // Heart beat trailing edge detected.
      if(reading < LowerThreshold){
        IgnoreReading = false;
      }  
      BPM = (1.0/PulseInterval) * 60.0 * 1000;
      Serial.print(reading);
      Serial.print("\t");
      Serial.print(PulseInterval);
      Serial.print("\t");
      Serial.print(BPM);
      Serial.println(" BPM");
      BPM=BPM-8;
      String url = "/update";
      url += "?api_key=";
      url += privateKey;
      url+="&field1=";
      url+=temperature;
      url+="&field2=";
      url+=humidity;
      url+="&field3=";
      url+=BPM;
      url+="&field4=";
      url+="17.4015734, 78.3330046";
     
      Serial.print("Requesting URL: ");
      Serial.println(url);
      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
      //client.flush();
      client.stop();
      Serial.flush();
      FirstPulseTime = millis();
      // Please don't use delay() - this is just for testing purposes.
      delay(50);  
}

