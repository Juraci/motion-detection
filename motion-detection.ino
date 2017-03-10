/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial


const char* ssid = "----";
const char* password = "----";
const char* boardId = "boardId=proto01";
const char* endpoint = "https://nodemcu-listener.herokuapp.com/motion";
const char* httpsFingerPrint = "F5 69 8C CA 29 68 5E 47 26 38 C5 1A 18 F1 8A 6A EA 60 56 4D";
const int pinOut = D0;
const int pinIn = D7; 

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(pinOut, OUTPUT);
  pinMode(pinIn, INPUT);
  waitUntilConnection();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    if(digitalRead(pinIn) == HIGH) {
      digitalWrite(pinOut, HIGH);
      USE_SERIAL.print("Motion started!\n");
      post();
      while(digitalRead(pinIn) == HIGH) {
        delay(500);
      }
      digitalWrite(pinOut, LOW);
      USE_SERIAL.print("Motion ended!\n");
    }

  } else {
    USE_SERIAL.println("Connection lost");
    waitUntilConnection();
  }

  delay(500); 
}

void waitUntilConnection() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(1000);
    USE_SERIAL.println("Waiting for connection");
  }
}

void post() {
  HTTPClient http;
  
  http.begin(endpoint, httpsFingerPrint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(boardId);
  USE_SERIAL.println(httpCode);

  http.writeToStream(&Serial);
  http.end();
}

