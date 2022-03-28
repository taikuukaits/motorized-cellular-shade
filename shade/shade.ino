#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "motor_driver.h"
#include "secrets.h"
#include "device.h"

WiFiClient espClient;
PubSubClient client(espClient);

int current_position = 0;
int max_steps = 0;

enum SHADE_MODE {
  IS_IDLING,
  IS_OPENING,
  IS_CLOSING,
  IS_JOGGING,
  IS_UPDATING
};

SHADE_MODE current_mode = IS_IDLING;

MotorDriver* left_motor = new MotorDriver(LEFT_MOTOR_PIN_A, LEFT_MOTOR_PIN_B, LEFT_MOTOR_PIN_ENCODER);
MotorDriver* right_motor = new MotorDriver(RIGHT_MOTOR_PIN_A, RIGHT_MOTOR_PIN_B, RIGHT_MOTOR_PIN_ENCODER);

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
  if (max_steps == 0) {
    Serial.println("URGENT: This shade has not been calibrated.");
    Serial.println("----------------------------------------------");
  }
  
  client.setBufferSize(MQTT_PACKET_SIZE);
  
  connect();

  current_mode = IS_IDLING;
  
  sendCoverDiscovery(max_steps);

  client.publish(AVAILABILITY_TOPIC, "online", true);

  client.subscribe(COVER_COMMAND_TOPIC);

  //Calibration
  client.subscribe(GREETINGS_TOPIC);
  client.subscribe(CALIBRATE_MAX_STEPS_TOPIC);
  client.subscribe(CALIBRATE_POSITION_TOPIC);

  client.setCallback(callback);
}

void loop() {
    ArduinoOTA.handle();
    if (current_mode == IS_UPDATING) {
      return;
    }

    if (current_mode == IS_OPENING || current_mode == IS_CLOSING || current_mode == IS_JOGGING) {
      if (!left_motor->is_moving() && !right_motor->is_moving()) {
          Serial.println("Done moving.");

          if (current_mode = IS_OPENING) {
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
        move_both_motors_by(0 - current_position);
    } else if (!strncmp((char *)payload, "CLOSE", length)) {
        Serial.print("CLOSING");
        current_mode = IS_CLOSING;
        client.publish(COVER_STATE_TOPIC, "closing");
        move_both_motors_by(max_steps - current_position);
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
  } else if (topic == CALIBRATE_LEFT_JOG_TOPIC) {
    int steps = payload_to_int(payload, length);
    move_motor_by(left_motor, "left", steps);
    current_mode = IS_JOGGING;
    
  } else if (topic == CALIBRATE_RIGHT_JOG_TOPIC) {
    int steps = payload_to_int(payload, length);
    move_motor_by(right_motor, "right", steps);
    current_mode = IS_JOGGING;
    
  }  else if (topic == CALIBRATE_JOG_TOPIC) {
    int steps = payload_to_int(payload, length);
    move_both_motors_by(steps);
    current_mode = IS_JOGGING;

  } else if (topic == CALIBRATE_MAX_STEPS_TOPIC) {
    int steps = payload_to_int(payload, length); 
    Serial.print("Updating max steps to ");
    Serial.println(steps);
    storageSaveMaxSteps(steps);
    Serial.println("Restarting...");
    ESP.restart();
  } else if (topic == CALIBRATE_POSITION_TOPIC) {
    int steps = payload_to_int(payload, length); 
    Serial.print("Updating position to ");
    Serial.println(steps);
    storageSavePosition(steps);
    Serial.println("Restarting...");
    ESP.restart();
  }
}

void move_both_motors_by(int steps){
    move_motor_by(left_motor, "left", steps);
    move_motor_by(right_motor, "right", steps);
}

int payload_to_int(byte* payload, unsigned int length) {
  payload[length] = '\0';
  String s = String((char*)payload);
  return s.toInt();
}

void move_motor_by(MotorDriver* motor, char* motor_name, int steps) {
    Serial.print("Jogging ");
    Serial.print(motor_name);
    Serial.print(" motor by ");
    Serial.print(steps);
    if (steps < 0) {
      Serial.println(" forward.");
      motor->move_forward_steps(steps);  
    } else {
      Serial.println(" backward.");
      motor->move_backward_steps(abs(steps));  
    }
}
