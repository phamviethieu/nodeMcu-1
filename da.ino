#include <ESP8266WiFi.h>
#include "DHT.h"
#include <PubSubClient.h>
#include <stdlib.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <MFRC522.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
char auth[] = "G1-ROTEzA0Yspaq53PNy7nYduZRIqOaL";
#define DHTPIN D5
#define DHTTYPE DHT11
WiFiClient nodeMCU;
PubSubClient mqttClient(nodeMCU);
DHT dht(DHTPIN, DHTTYPE);
const char *ssid = "Ladykillah";
const char *password = "mot23456";

const char *mqtt_server = "farmer.cloudmqtt.com";
const int   mqttPort    = 10573; // your_port
const char *mqttUser    =  "nvaelbpw";
const char *mqttPassword = "nnqApb3r9Irc";
const char *clientID = "1";
int ledPin[] = {D0, D1, D2, D3};
int pinCount ;
float h;//doam 
float t;//nhietdo 
char server[] = "192.168.1.10";
unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long t3 = 0;

void setup() {
  dht.begin();
  Blynk.begin(auth, ssid, password);
  pinCount = sizeof(ledPin);
  for (int i = 0 ; i < pinCount; i++) {
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], 0);
  }
  //  pinMode(led1,OUTPUT);
  //  pinMode(led2,OUTPUT);
  //  pinMode(led3,OUTPUT);
  //  pinMode(led4,OUTPUT);

  Serial.begin(115200);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  mqttClient.setServer(mqtt_server, mqttPort);
  mqttClient.setCallback(callback);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String receiver = "";
  for (byte i = 0; i < length; i++) {
    receiver += (char)payload[i];
  }
  Serial.println(receiver);
  String myTopic = String(topic);
  // if(myTopic == "LED" && receiver == "1") digitalWrite(led1, HIGH);
  // if(myTopic == "LED" && receiver == "0") digitalWrite(led1, LOW);
  //
  //  if(myTopic == "TangGiam") analogWrite(led1, atoi(receiver.c_str()));
  //  Serial.print(atoi(receiver.c_str()));


  //  if(topic == "LED"){
  //      if(receiver == "1") digitalWrite(led,1);
  //      else digitalWrite(led,0);
  //    }

}
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(clientID, mqttUser, mqttPassword)) {
      Serial.println("connected");
      // your subscribe topic
      mqttClient.subscribe("LED");
      mqttClient.subscribe("TangGiam");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void send_data(float nhietdo, float doam){
   
    Serial.print("Nhiet do : ");
    Serial.println(nhietdo);
    Serial.print("Do Am : ");
    Serial.println(doam);
    mqttClient.publish("nhietdo", String(t).c_str());
    mqttClient.publish("doam", String(h).c_str());

  }
void mode1(){
  {
     for (int i = pinCount - 1 ; i >= 0; i--) {
        digitalWrite(ledPin[i], 0);
        delay(10);
     }
      for (int i = 0; i < pinCount; i++) {
      digitalWrite(ledPin[i], 1);
      delay(10);
    }
  }
}

void loop()
{
  //  digitalWrite(led1,0);
  //    digitalWrite(led2,0);
  //    digitalWrite(led3,0);
  //    digitalWrite(led4,0);
  Blynk.run();
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop();
  t = dht.readTemperature();
  h = dht.readHumidity();
  unsigned long current_time = millis();
  if(current_time - t1 >= 5000)
  {
       send_data(t, h);
       t1 = millis();
  }
  if(current_time - t2 >= 10)
  {
    if (h >90) mode1();  
    else 
    {
     for (int i = 0; i < pinCount; i++) {
      digitalWrite(ledPin[i], 1);
      delay(50);
     }
    }
    t2 = millis();
  }
//  if(current_time - t3 >= 30000){
//    sendToDataBase();
//    t3 = millis();
//  }
// 
}
void sendToDataBase()   //CONNECTING WITH MYSQL
{
  if (nodeMCU.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /nodeMcu/dht11.php?doam=");
    nodeMCU.print("GET /nodeMcu/dht11.php?doam=");     //YOUR URL
    Serial.println(h);
    nodeMCU.print(h);
    nodeMCU.print("&nhietdo=");
    Serial.println("&nhietdo=");
    nodeMCU.print(t);
    Serial.println(t);
    nodeMCU.print(" ");      //SPACE BEFORE HTTP/1.1
    nodeMCU.print("HTTP/1.1");
    nodeMCU.println();
    nodeMCU.println("Host: 192.168.1.10");
    nodeMCU.println("Connection: close");
    nodeMCU.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}
