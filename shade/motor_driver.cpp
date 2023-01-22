#include "motor_driver.h"
#include "Arduino.h"
#include <Encoder.h>

// Responsible for moving the motor to positions is relative to setup (is always 0 after setup).
MotorDriver::MotorDriver(int motor_pin_one, int motor_pin_two, Encoder encoder){
  _motor_pin_one = motor_pin_one;
  _motor_pin_two = motor_pin_two;
  _encoder = encoder;
}

void MotorDriver::begin() {
    pinMode(_motor_pin_one, OUTPUT);
    pinMode(_motor_pin_two, OUTPUT);
    _halt();
}

bool MotorDriver::request_move() {
    if (_moving) {
        return false; 
    }
    _moving = true;
    _reached_target = false;
    _target = _new_target;

    if (_new_target > _position) {
        _direction_positive = true; 
    } else {
        _direction_positive = false;
    }

    if (_direction_positive) {
        _move_positive();
    } else {
        _move_negative();
    }
    return true;
}

bool MotorDriver::request_stop() {
  if (_moving) {
    _target = _current_position; // We consider wherever we are the current target - let actual stop happen naturally.
    return true;
  }
  return false;
}

void MotorDriver::loop() {
  int _next_position = encoder.read();
  if (_next_position != _current_position) {
      _current_position = _next_position;
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
      } else if (_moving && _reached_target && is_within_reached_target_delay()) {
          // normal situation, this is totally fine, we just swallow this movement
          _diagnostic_position_change_detected_within_delay();
      } else {
          //the panic situation - we were not moving or are outside of the delay (which shouldn't be possible)
          _last_known_rest_position = _current_position;
          _diagnostic_position_change_without_movement();
      }
  } else if (_moving) {
      // if we are trying to move but the last change was too long ago, assume something has gone wrong
      if (_has_stall_delay_been_exceeded()){
          _last_known_rest_position = _current_position;
          _diagnostic_motor_stall_detected();
          _moving = false; //stop moving, we haven't reached the target but nothing else to be done.
          _halt();
      }
  }

  // Even if the position did not change - but we are moving, and at the target, when the delay is exceeded, we are done moving
  if (_moving && _reached_target && !_is_within_reached_target_delay()) {
      _moving = false;
      _reached_target = true;
      _last_known_rest_position = _current_position;
  }
}

bool MotorDriver::is_moving() {
    return _moving;
}

int MotorDriver::get_last_known_rest_position() {
    return _last_known_rest_position;
}

int MotorDriver::get_current_position() {
    return _current_position;
}

bool MotorDriver::_has_reached_target() {
    if (_direction_positive) {
        if (_current_position > _target) {
            return true;
        }
    } else {
        if (_current_position < _target) {
            return true;
        }
    }
    return false;
}

void MotorDriver::_move_positive() {
  digitalWrite(_motor_pin_one, LOW);
  digitalWrite(_motor_pin_two, HIGH);
}

void MotorDriver::_move_negative() {
  digitalWrite(_motor_pin_one, HIGH);
  digitalWrite(_motor_pin_two, LOW);
}

void MotorDriver::_halt() {
  digitalWrite(_motor_pin_one, LOW);
  digitalWrite(_motor_pin_two, LOW);
}

void MotorDriver::_has_stall_delay_been_exceeded() {
    return (unsigned long)(millis() - millis_when_position_last_changed) > position_change_stall_delay;
}

void MotorDriver::_is_within_reached_target_delay() {
    return (unsigned long)(millis() - millis_when_reached_target) > reached_target_delay;
}

void MotorDriver::_diagnostic_position_change_without_movement() {
    Serial.println("ERROR: Detected position change after delay! Something could be wrong.");
}

void MotorDriver::_diagnostic_position_change_detected_within_delay() {
    Serial.println("trace: Normal position change between delay and finish moving.");
}

void MotorDriver::_diagnostic_motor_stall_detected() {
    Serial.println("ERROR: Motor stall detected! Halting movement.");
}

void MotorDriver::_diagnostic_position_change() {
    Serial.print("Position changed: ");
    Serial.println(_current_position);
}

void MotorDriver::_diagnostic_reached_target() {
    Serial.print("Reached target!");
}
