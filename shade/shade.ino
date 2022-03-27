#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "device.h"

PubSubClient client;

void setup() {
  Serial.begin(9600);
  connect();

  client.subscribe(COVER_COMMAND_TOPIC);

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
  }
}
