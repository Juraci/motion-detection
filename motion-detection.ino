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

void setup() {
  USE_SERIAL.begin(115200);

  WiFi.begin(ssid, password);

  waitUntilConnection();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    USE_SERIAL.print("connected!\n");

    HTTPClient http;

    http.begin("https://nodemcu-listener.herokuapp.com/motion", "F5 69 8C CA 29 68 5E 47 26 38 C5 1A 18 F1 8A 6A EA 60 56 4D");
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST("");
    USE_SERIAL.println(httpCode);

    http.writeToStream(&Serial);
    http.end();

  } else {
    USE_SERIAL.println("Connection lost");
    waitUntilConnection();
  }

  delay(10000);
}

void waitUntilConnection() {
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(1000);
    USE_SERIAL.println("Waiting for connection");
  }
}

