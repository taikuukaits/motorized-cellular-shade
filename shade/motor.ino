bool moving = false;


#include <Encoder.h>

Encoder encoder(ENCODER_PIN_ONE, ENCODER_PIN_TWO);

void motor_setup() {
    pinMode(MOTOR_PIN_ONE, OUTPUT);
    pinMode(MOTOR_PIN_TWO, OUTPUT);
}

void motor_loop() {
    int next_position = encoder.read();
    if (next_position != _current_position) {
        _current_position = next_position;
        _current_position_changed = true;
        _millis_when_position_last_changed = millis();
        _diagnostic_position_change();
    } else {
        _current_position_changed = false;
    }

    if (_current_position_changed) {
        if (_moving && !_reached_target) {
            if (_has_reached_target()) {
                _reached_target = true;
                _millis_when_reached_target = millis();
                _diagnostic_reached_target();
                _halt();
            }
        } else if (_moving && reached_target && is_within_reached_target_delay()) {
            // normal situation, this is totally fine, we just swallow this movement
            _diagnostic_position_change_detected_within_delay();
        } else {
            //the panic situation - we were not moving or are outside of the delay (which shouldn't be possible)
            _diagnostic_position_change_without_movement();
            storageSaveCurrentPosition(_current_position); 
        }
    } else if (moving) {
        // if we are trying to move but the last change was too long ago, assume something has gone wrong
        if (_has_stall_delay_been_exceeded()){
            _diagnostic_motor_stall_detected();
            _moving = false; //stop moving, we haven't reached the target but nothing else to be done.
            _halt();
        }
    }

    // Even if the position did not change - but we are moving, and at the target, when the delay is exceeded, we are dont moving
    if (_moving && _reached_target && !_is_within_reached_target_delay()) {
        _moving = false;
        _reached_target = true;
    }
}

bool request_move(int new_target) {
    if (moving) {
        return false; 
    }
    moving = true;
    reached_target = false;
    target = new_target;

    if (new_target > position) {
        direction_positive = true; 
    } else {
        direction_positive = false;
    }

    if (direction_positive) {
        _move_positive();
    } else {
        _move_negative();
    }
}

bool _has_reached_target() {
    if (direction_positive) {
        if (current_position > target) {
            return true;
        }
    } else {
        if (current_position < target) {
            return true;
        }
    }
    return false;
}

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

bool _has_stall_delay_been_exceeded() {
    return (unsigned long)(millis() - millis_when_position_last_changed) > position_change_stall_delay;
}

bool _is_within_reached_target_delay() {
    return (unsigned long)(millis() - millis_when_reached_target) > reached_target_delay;
}

void _diagnostic_position_change_without_movement() {
    Serial.println("ERROR: Detected position change after delay! Something could be wrong.");
    // publish to MQTT?
}

void _diagnostic_position_change_detected_within_delay() {
    Serial.println("trace: Normal position change between delay and finish moving.");
}

void _diagnostic_motor_stall_detected() {
    Serial.println("ERROR: Motor stall detected! Halting movement.");
}

void _diagnostic_position_change() {
    Serial.print("Position changed: ");
    Serial.println(current_position);
}

void _diagnostic_reached_target() {
    Serial.print("Reached target!");
}