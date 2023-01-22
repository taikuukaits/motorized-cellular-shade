#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "motor_driver.h"
#include "permanent_storage.h"
#include "shade_positioner.h"
#include "secrets.h"
#include "device.h"
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);

int current_position = 0;
int max_steps = 0;
bool started = false;

enum SHADE_MODE {
  IS_IDLING,
  IS_OPENING,
  IS_CLOSING,
  IS_JOGGING,
  IS_UPDATING
};

SHADE_MODE current_mode = IS_IDLING;

Encoder encoder = Encoder(ENCODER_PIN_ONE, ENCODER_PIN_TWO);
PermanentStorage permanent_storage = PermanentStorage();
MotorDriver motor_driver = MotorDriver(MOTOR_PIN_ONE, MOTOR_PIN_TWO, encoder);
ShadePositioner shade_positioner = ShadePositioner(motor_driver, permanent_storage);

void setup() {
  Serial.begin(9600);

  delay(1000);
  Serial.println("----------------------------------------------");
  Serial.println();
  Serial.println("Motorized Cellular Shade v" SOFTWARE_VERSION);
  Serial.println();
  Serial.println("----------------------------------------------");
  delay(1000);

  Serial.println("Setting up device.");

  permanent_storage.begin();
  motor_driver.begin();
  shade_positioner.begin();

  Serial.println("State of the device.");

  Serial.print("Last Shade Position: ");
  Serial.println(permanent_storage.read_last_known_shade_position());
  Serial.print("Closed Position: ");
  Serial.println(permanent_storage.read_closed_position());
  Serial.println("----------------------------------------------");

  client.setBufferSize(MQTT_PACKET_SIZE);

  connect();

  current_mode = IS_IDLING;

  sendCoverDiscovery(max_steps);

  client.publish(AVAILABILITY_TOPIC, "online", true);

  client.subscribe(COVER_COMMAND_TOPIC);

  //Calibration
  client.subscribe(GREETINGS_TOPIC);
  client.subscribe(CALIBRATE_CLOSED_POSITION_TOPIC);
  client.subscribe(CALIBRATE_POSITION_TOPIC);
  client.subscribe(CALIBRATE_JOG_TOPIC);

  client.setCallback(callback);

  started = true;

  Serial.println("Started succesfully!");
}

void loop() {
  motor_driver.loop();
  shade_positioner.loop();
  if (!started) return; // hmm should OTA go before this?

  ArduinoOTA.handle();
  if (current_mode == IS_UPDATING) {
    return;
  }

  if (current_mode == IS_OPENING || current_mode == IS_CLOSING || current_mode == IS_JOGGING) {
    if (shade_positioner.is_moving()) {
      Serial.println("Done moving.");

      if (current_mode == IS_OPENING) {
        Serial.println("OPEN");
        client.publish(COVER_STATE_TOPIC, "open");
        current_position = 0;
      } else if (current_mode == IS_CLOSING) {
        Serial.println("CLOSED");
        client.publish(COVER_STATE_TOPIC, "closed");
        current_position = max_steps;
      }

      current_mode = IS_IDLING;
    } else {
      return;
    }
  }

  //only when not UPDATING or MOVING will we respond to MQTT messages.
  client.loop();
}

void callback(char* topic_raw, byte* payload, unsigned int length) {
  Serial.print("Receieved a message on ");
  Serial.println(topic_raw);

  if (current_mode != IS_IDLING) {
    Serial.println("Ignoring the message. Currently busy.");
    return;
  }
  String topic = String(topic_raw);

  if (topic == COVER_COMMAND_TOPIC) {
    if (!strncmp((char *)payload, "OPEN", length)) {
      Serial.print("OPENING");
      current_mode = IS_OPENING;
      client.publish(COVER_STATE_TOPIC, "opening");
      shade_positioner.move_to_shade_position(0);
    } else if (!strncmp((char *)payload, "CLOSE", length)) {
      Serial.print("CLOSING");
      current_mode = IS_CLOSING;
      client.publish(COVER_STATE_TOPIC, "closing");
      shade_positioner.move_to_close_position();
    } else if (!strncmp((char *)payload, "STOP", length)) {
      Serial.print("STOPPING");
      //Theoretically, here we need to guarantee motors are in known position.
      shade_positioner.request_stop();
    } else {
      Serial.print("UNKNOWN command.");
    }
  } else if (topic == GREETINGS_TOPIC) {
    if (!strncmp((char *)payload, HELLO_PAYLOAD, length)) {
      if (max_steps == 0) {
        Serial.println("Sent hello!");
        client.publish(GREETINGS_TOPIC, DEVICE_ID);
      }
    } else if (!strncmp((char *)payload, ANNOUNCE_PAYLOAD, length)) {
      Serial.println("Sent hello!");
      client.publish(GREETINGS_TOPIC, DEVICE_ID);
    }

  }  else if (topic == CALIBRATE_JOG_TOPIC) {
    int jog_amount = payload_to_int(payload, length);
    shade_positioner.move_to_shade_position(shade_positioner.get_shade_position() + jog_amount);
    current_mode = IS_JOGGING;
    Serial.print("Jogging ");
    Serial.print(jog_amount);
    Serial.print(" shade position.");

  } else if (topic == CALIBRATE_CLOSED_POSITION_TOPIC) {
    int position = payload_to_int(payload, length);
    Serial.print("Updating close position to ");
    Serial.println(position);
    shade_positioner.calibration_set_shade_closed_position(position);
    restart();

  } else if (topic == CALIBRATE_POSITION_TOPIC) {
    int position = payload_to_int(payload, length);
    Serial.print("Updating current shade position to ");
    Serial.println(position);
    shade_positioner.calibration_shade_is_currently_at(position);
    restart();
  }
}

void restart() {
    Serial.println("Restarting...");
    ESP.restart();
}

int payload_to_int(byte* payload, unsigned int length) {
  payload[length] = '\0';
  String s = String((char*)payload);
  return s.toInt();
}
