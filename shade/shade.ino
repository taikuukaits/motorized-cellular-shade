#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "device.h"

WiFiClient espClient;
PubSubClient client(espClient);

int current_position = 0;
int max_steps = DEFAULT_MAX_STEPS;
int left_offset = 0;
int right_offset = 0;

void setup() {
  Serial.begin(9600);

  delay(1000);
  Serial.println("----------------------------------------------");
  Serial.println();
  Serial.println("Motorized Cellular Shade v" SOFTWARE_VERSION);
  Serial.println();
  Serial.println("----------------------------------------------");

  storageSetup();

  current_position = storageReadPosition();
  max_steps = storageReadMaxSteps();    

  Serial.print("Position: ");
  Serial.println(current_position);
  Serial.print("Max Steps: ");
  Serial.println(max_steps);
  Serial.println("----------------------------------------------");
  
  client.setBufferSize(MQTT_PACKET_SIZE);
  
  connect();

  delay(1000);
  sendCoverDiscovery(max_steps);
  delay(1000);
  sendMaxStepsDiscovery();
  delay(1000);

  client.publish(AVAILABILITY_TOPIC, "online", true);

  publishInt(MAX_STEPS_STATE_TOPIC, max_steps, false);

  client.subscribe(COVER_COMMAND_TOPIC);
  client.subscribe(MAX_STEPS_COMMAND_TOPIC);

  client.setCallback(callback);
}

void loop() {
    //TODO: Eventually I think while the motors are moving, the only thing I want to be doing is observing them.
    
    ArduinoOTA.handle();

    client.loop();


    //TODO: Handle reconnect?
    //if (!client.connected()) {
    //    reconnect();
    //}
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (String(topic) == COVER_COMMAND_TOPIC) {
    if (!strncmp((char *)payload, "OPEN", length)) {
        Serial.print("OPENING");
        client.publish(COVER_STATE_TOPIC, "opening");
        delay(10000);
        client.publish(COVER_STATE_TOPIC, "open");
        Serial.print("Done OPENING");
    } else if (!strncmp((char *)payload, "CLOSE", length)) {
        Serial.print("CLOSING");
        client.publish(COVER_STATE_TOPIC, "closing");
        delay(10000);
        client.publish(COVER_STATE_TOPIC, "closed");
        Serial.print("Done CLOSING");
    }
  } else if (String(topic) == MAX_STEPS_COMMAND_TOPIC) {
    payload[length] = '\0';
    String s = String((char*)payload);
    int i= s.toInt();
    Serial.print("Updating max steps to ");
    Serial.println(i);
    storageSaveMaxSteps(i);
    ESP.restart();
  }
}
