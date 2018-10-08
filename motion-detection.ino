#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define USE_SERIAL Serial

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "------";
const char* password = "------";

const char* mqttServer = "______";
const int mqttPort = 17152;
const char* clientId = "------";
const char* outTopic = "-------";
const char* inTopic = "-------";
const char* brokerUser = "-------";
const char* brokerPassword = "--------";
const int brokerRetryDelay = 5000;

const int sensorInput = D7;
const int motionLedOut = D0;

const int mainLoopDelay = 1000;
const int coolDownDelay = 500;
const int wifiDelay = 500;

void waitUntilConnection(const char* id, const char* pass, int waitTime);
void setupMqtt(const char* server, int port);
void connectToBroker(const char* id, const char* user, const char* pass, int brokerDelay);
void callback(char* topic, byte* payload, unsigned int length);
void mqttPublish();
void onSensorActivity(void (*callback)());

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(motionLedOut, OUTPUT);
  pinMode(sensorInput, INPUT);

  waitUntilConnection(ssid, password, wifiDelay);
  setupMqtt(mqttServer, mqttPort);
  connectToBroker(clientId, brokerUser, brokerPassword, brokerRetryDelay);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    if (!client.connected()) {
      connectToBroker(clientId, brokerUser, brokerPassword, brokerRetryDelay);
    }
    client.loop();

    onSensorActivity(mqttPublish);

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
  WiFi.mode(WIFI_STA);
  WiFi.begin(id, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(waitTime);
    USE_SERIAL.println("Waiting for connection");
  }

  USE_SERIAL.println("WiFi connected");
}

void setupMqtt(const char* server, int port) {
  client.setServer(server, port);
  client.setCallback(callback);
}

void connectToBroker(const char* id, const char* user, const char* pass, int brokerDelay) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(id, user, pass)) {
      Serial.println("connected");
      client.subscribe(inTopic);

    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(brokerDelay);
    }
  }
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

void mqttPublish() {
  USE_SERIAL.println("publish");
  client.publish(outTopic, "motion detected");
}
