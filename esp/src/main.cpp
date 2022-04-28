#include <Arduino.h>
#include <ESP8266WiFi.h>
#define PORT    1016

const char* ssid = "FiveGuys";
const char* pass = "Milo9420";

const char* server = "192.168.99.157";
WiFiServer w(PORT);

void setup(){
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }
  Serial.print("ASSIGNED IP: %s\n");
  Serial.print(WiFi.localIP().toString());
  w.begin();
}

void loop(){}