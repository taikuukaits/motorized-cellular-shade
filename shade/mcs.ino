int target_position = 0;
int current_position = 0;
bool direction_positive = false;

bool moving = false;

bool reached_target = false;
unsigned long millis_when_reached_target = 0;
int reached_target_delay = 50; // how long to wait after reaching the target to be done moving
unsigned long millis_from_last_position_change = 0;
int minimum_position_delay = 50;

int previous_position = 0;
bool current_position_changed = false;

#include <Encoder.h>

Encoder encoder(ENCODER_PIN_ONE, ENCODER_PIN_TWO);

void _move_positive() {
  digitalWrite(MOTOR_PIN_ONE, LOW);
  digitalWrite(MOTOR_PIN_TWO, HIGH);
}

void _move_negative() {
  digitalWrite(MOTOR_PIN_ONE, HIGH);
  digitalWrite(MOTOR_PIN_TWO, LOW);
}

void _halt() {
  digitalWrite(MOTOR_PIN_ONE, LOW);
  digitalWrite(MOTOR_PIN_TWO, LOW);
}

void motor_setup() {
    pinMode(MOTOR_PIN_ONE, OUTPUT);
    pinMode(MOTOR_PIN_TWO, OUTPUT);
}

void motor_loop() {
    next_position = encoder.read();
    if (next_position != current_position) {
        previous_position = current_position;
        current_position = next_position;
        current_position_changed = true;
    } else {
        current_position_changed = false;
    }

    if (current_position_changed) {
        _handle_position_change()
    }
}

bool is_within_reached_target_delay() {
    return (unsigned long)(millis() - millis_when_reached_target) > reached_target_delay;
}

void _handle_position_change() {
    if (moving) {
        _handle_movement();
    } else {
        _handle_position_change_but_not_moving(); // We aren't moving but the encoder moved!
    }
}


void _start_moving() {
    moving = true;
    reached_target = false;
}

void _handle_arrived() {
    millis_when_reached_target = millis()
    reached_target = true  // we are still moving!
}

void _handle_position_change_but_not_moving() {
    // If we are within a delay, normal movement expected, otherwise - PANIC!
    if ((unsigned long)(millis() - millis_when_reached_target) > reached_target_delay) { 
        _diagnostic_position_change_without_movement();
        // We need to save this position now or it won't be saved.
        storageSaveCurrentPosition(current_position); 
    } else {
        // TODO: Remove this log once we know it works
        Serial.println("trace: Normal position change between delay and finish moving.");
    }
}

_diagnostic_position_change_without_movement() {
    Serial.println("ERROR: Detected position change after delay! Something could be wrong.");
    // publish to MQTT?
}