#include <Arduino.h>
#include <WifiWrapper.h>
#include <MqttWrapper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define USE_SERIAL Serial

WiFiClient espClient;
PubSubClient client(espClient);

char* ssid = "____";
char* password = "____";

char* mqttServer = "____";
int mqttPort = 17152;
char* clientId = "----";
char* outTopic = "____";
char* inTopic = "____";
char* brokerUser = "____";
char* brokerPassword = "____";
long now = 0;
long lastMsg = 0;

const int sensorInput = D7;
const int motionLedOut = D0;

const int mainLoopDelay = 1000;
const int coolDownDelay = 1000;

void callback(char* topic, byte* payload, unsigned int length);
void onSensorActivity();

WifiWrapper wifi(ssid, password);
MqttWrapper mqtt(mqttServer, clientId, brokerUser, brokerPassword, mqttPort, client);

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(motionLedOut, OUTPUT);
  pinMode(sensorInput, INPUT);

  wifi.waitForConnection();
  mqtt.setup();
  mqtt.setTopics(inTopic, outTopic);
  mqtt.setCallback(callback);
  mqtt.connect();
}

void loop() {
  wifi.waitForConnection();

  mqtt.loop();
  onSensorActivity();

  now = millis();
  if (now - lastMsg >= 10000) {
    lastMsg = now;
    mqtt.publish("0");
  }

  delay(mainLoopDelay);
}

void onSensorActivity() {
  if(digitalRead(sensorInput) == LOW) return;

  digitalWrite(motionLedOut, HIGH);
  USE_SERIAL.print("Motion started!\n");

  while(digitalRead(sensorInput) == HIGH) {
    mqtt.publish("1");
    delay(coolDownDelay);
  }

  digitalWrite(motionLedOut, LOW);

  USE_SERIAL.print("\nMotion ended!\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
