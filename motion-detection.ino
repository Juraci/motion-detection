#include <Arduino.h>

#include <MqttWrapper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define USE_SERIAL Serial

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "______";
const char* password = "______";

char* mqttServer = "______";
int mqttPort = 17152;
char* clientId = "______";
char* outTopic = "______";
char* inTopic = "______";
char* brokerUser = "______";
char* brokerPassword = "______";

const int sensorInput = D7;
const int motionLedOut = D0;

const int mainLoopDelay = 1000;
const int coolDownDelay = 500;
const int wifiDelay = 500;

void waitUntilConnection(const char* id, const char* pass, int waitTime);
void callback(char* topic, byte* payload, unsigned int length);
void mqttPublish();
void onSensorActivity();

MqttWrapper mqtt(mqttServer, clientId, brokerUser, brokerPassword, mqttPort, client);

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(motionLedOut, OUTPUT);
  pinMode(sensorInput, INPUT);

  waitUntilConnection(ssid, password, wifiDelay);
  mqtt.setup();
  mqtt.setTopics(inTopic, outTopic);
  mqtt.setCallback(callback);
  mqtt.connect();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    mqtt.loop();
    onSensorActivity();

  } else {
    USE_SERIAL.println("Connection lost");
    waitUntilConnection(ssid, password, wifiDelay);
  }

  delay(mainLoopDelay);
}

void onSensorActivity() {
  if(digitalRead(sensorInput) == LOW) return;

  digitalWrite(motionLedOut, HIGH);
  USE_SERIAL.print("Motion started!\n");

  mqtt.publish("1");

  while(digitalRead(sensorInput) == HIGH) {
    delay(coolDownDelay);
  }

  mqtt.publish("0");
  digitalWrite(motionLedOut, LOW);

  USE_SERIAL.print("\nMotion ended!\n");
}

void waitUntilConnection(const char* id, const char* pass, int waitTime) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(id, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(waitTime);
    USE_SERIAL.println("Waiting for connection");
  }

  USE_SERIAL.println("WiFi connected");
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
