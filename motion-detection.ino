#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial

const char* ssid = "-----";
const char* password = "------";
const char* endpoint = "https://nodemcu-listener.herokuapp.com/cards/cardId/motion"; // the number is the board id
const char* httpsFingerPrint = "F5 69 8C CA 29 68 5E 47 26 38 C5 1A 18 F1 8A 6A EA 60 56 4D";
const char* authToken = "----";

const int sensorInput = D7;
const int motionLedOut = D0;

const int mainLoopDelay = 1000;
const int coolDownDelay = 500;
const int wifiDelay = 1000;

void waitUntilConnection(const char* id, const char* pass, int waitTime);
void post();
void onSensorActivity(void (*callback)());

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(motionLedOut, OUTPUT);
  pinMode(sensorInput, INPUT);
  waitUntilConnection(ssid, password, wifiDelay);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    onSensorActivity(post);

  } else {
    USE_SERIAL.println("Connection lost");
    waitUntilConnection(ssid, password, wifiDelay);
  }

  delay(mainLoopDelay);
}

void onSensorActivity(void (*callback)()) {
  if(digitalRead(sensorInput) == LOW) return;

  digitalWrite(motionLedOut, HIGH);
  USE_SERIAL.print("Motion started!\n");

  (*callback)();

  while(digitalRead(sensorInput) == HIGH) {
    delay(coolDownDelay);
  }

  digitalWrite(motionLedOut, LOW);

  USE_SERIAL.print("\nMotion ended!\n");
}

void waitUntilConnection(const char* id, const char* pass, int waitTime) {
  WiFi.begin(id, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(waitTime);
    USE_SERIAL.println("Waiting for connection");
  }
}

void post() {
  HTTPClient http;
  
  http.begin(endpoint, httpsFingerPrint);
  http.setAuthorization(authToken);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("");
  USE_SERIAL.println(httpCode);

  http.writeToStream(&Serial);
  http.end();
}
