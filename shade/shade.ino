#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "Arduino.h"

#include "motor_driver.h"
#include "permanent_storage.h"
#include "shade_positioner.h"
#include "secrets.h"
#include "device.h"
#include "config.h"
#include <Encoder.h>

WiFiClient espClient;
PubSubClient client(espClient);

bool started = false;

enum SHADE_MODE {
  IS_IDLING,
  IS_MOVING,
  IS_UPDATING
};

enum MOVEMENT_TYPE {
  CLOSING, // Moving to the CLOSED position
  OPENING, // Moving to the OPEN position
  POSITION // Just moving to any target position, we are not open or closed. Jogging or set position.
};

SHADE_MODE current_mode = IS_IDLING;
MOVEMENT_TYPE movement_type = CLOSING;

Encoder encoder = Encoder(ENCODER_PIN_ONE, ENCODER_PIN_TWO);
PermanentStorage permanent_storage = PermanentStorage();
MotorDriver motor_driver = MotorDriver(MOTOR_PIN_ONE, MOTOR_PIN_TWO, encoder);
ShadePositioner shade_positioner = ShadePositioner(motor_driver, permanent_storage);

void setup() {
  Serial.begin(9600);

  permanent_storage.begin();
  motor_driver.begin();
  shade_positioner.begin();

  delay(1000);
  log_setup_welcome();
  delay(1000);

  log_setup_starting_state(permanent_storage.read_last_known_shade_position(), permanent_storage.read_closed_position());

  client.setBufferSize(MQTT_PACKET_SIZE);

  connect();

  current_mode = IS_IDLING;

  sendCoverDiscovery(shade_positioner.get_closed_position());

  client.publish(AVAILABILITY_TOPIC, "online", true);
  client.subscribe(COVER_COMMAND_TOPIC);

  //Calibration
  client.subscribe(GREETINGS_TOPIC);
  client.subscribe(CALIBRATE_CLOSED_POSITION_TOPIC);
  client.subscribe(CALIBRATE_POSITION_TOPIC);
  client.subscribe(CALIBRATE_JOG_TOPIC);

  client.setCallback(callback);

  started = true;

  log_setup_complete();
}

void loop() {
  motor_driver.loop();
  shade_positioner.loop();
  if (!started) return; // hmm should OTA go before this?

  ArduinoOTA.handle();
  if (current_mode == IS_UPDATING) {
    return;
  }
  
  client.loop();

  if (current_mode == IS_MOVING) {
    if (shade_positioner.is_moving()) {
      Serial.println("Done moving.");

      if (movement_type == OPENING) {
        Serial.println("OPEN");
        client.publish(COVER_STATE_TOPIC, "open");
      } else if (current_mode == CLOSING) {
        Serial.println("CLOSED");
        client.publish(COVER_STATE_TOPIC, "closed");
      }

      current_mode = IS_IDLING;
    } else {
      return;
    }
  }

}

void callback(char* topic_raw, byte* payload, unsigned int length) {
  Serial.print("Receieved a message on ");
  Serial.println(topic_raw);

  if (is_moving()) {
    Serial.println("Busy. Can't respond to message. Already moving."); //Except maybe emergency stop?
    return;
  }

  if (current_mode != IS_IDLING) {
    Serial.println("Ignoring the message. Currently busy.");
    return;
  }
  String topic = String(topic_raw);

  if (topic == COVER_COMMAND_TOPIC) { // We could also support move to a specific position. 
    if (!strncmp((char *)payload, "OPEN", length)) {
      open();
    } else if (!strncmp((char *)payload, "CLOSE", length)) {
      close();
    } else if (!strncmp((char *)payload, "STOP", length)) {
      Serial.print("Shade asked to STOP.");
      shade_positioner.request_stop(); // If we were moving, when the shade_positioner stops, we will stop. (return to IDLE mode).
    } else {
      Serial.print("UNKNOWN command.");
    }
  } else if (topic == GREETINGS_TOPIC) {
    if (!strncmp((char *)payload, HELLO_PAYLOAD, length)) {
      if (!has_been_setup()) {
        client.publish(GREETINGS_TOPIC, DEVICE_ID);
      }
    } else if (!strncmp((char *)payload, ANNOUNCE_PAYLOAD, length)) {
      client.publish(GREETINGS_TOPIC, DEVICE_ID);
    }

  }  else if (topic == CALIBRATE_JOG_TOPIC) {
    int jog_amount = payload_to_int(payload, length);
    log_jogged(jog_amount);
    shade_positioner.move_to_shade_position(shade_positioner.get_shade_position() + jog_amount);
    current_mode = IS_MOVING;
    movement_type = POSITION;

  } else if (topic == CALIBRATE_CLOSED_POSITION_TOPIC) {
    int position = payload_to_int(payload, length);
    log_calibrated_closed(position);
    shade_positioner.calibration_set_shade_closed_position(position);
    restart();

  } else if (topic == CALIBRATE_POSITION_TOPIC) {
    int position = payload_to_int(payload, length);
    log_calibrated_position(position);
    shade_positioner.calibration_shade_is_currently_at(position);
    restart();
  }
}

void open() {
  Serial.print("Shade asked to OPEN");
  current_mode = IS_MOVING;
  movement_type = OPENING
  client.publish(COVER_STATE_TOPIC, "opening");
  shade_positioner.move_to_shade_position(0);
}

void close() {
  Serial.print("Shade asked to CLOSE.");
  current_mode = IS_CLOSING;
  client.publish(COVER_STATE_TOPIC, "closing");
  shade_positioner.move_to_close_position();
}

bool has_been_setup() {
  // This is really just a best guess - we know if the max-steps is low we probably haven't been setup or if the max-steps is ludicriously high. Also if current position is outside of 0 and max steps, likely not been setup.
  if (shade_positioner.get_closed_position() == 0) { //We at LEAST know this to be true.
    return true;
  }
  return false;
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
