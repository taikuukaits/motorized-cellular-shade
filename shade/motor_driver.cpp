#include "motor_driver.h"
#include "Arduino.h"

MotorDriver::MotorDriver(int pinA, int pinB, int pinEncoder){
  _pinA = pinA;
  _pinB = pinB;
  _pinEncoder = pinEncoder;
}

void MotorDriver::begin() {
    pinMode(_pinEncoder, INPUT_PULLUP);
    pinMode(_pinA, OUTPUT);
    pinMode(_pinB, OUTPUT);

    motor_stop();
}

void MotorDriver::loop() {
    if (_steps <= 0) {
      if (_motor_on) {
        motor_stop();
      }
      return;
    }
    
    bool encoder_on = digitalRead(_pinEncoder);
    if (encoder_on && !_tripped) {
        _tripped = true;
        _steps--;
    } else if (!encoder_on) {
        _tripped = false;
    }
}

bool MotorDriver::is_moving() {
  return _motor_on;
}

void MotorDriver::move_forward_steps(int steps) {
    _steps = steps;
    _motor_on = true;
    digitalWrite(_pinA, HIGH);
    digitalWrite(_pinB, LOW);
}

void MotorDriver::move_backward_steps(int steps) {
    _steps = steps;
    _motor_on = true;
    digitalWrite(_pinA, LOW);
    digitalWrite(_pinB, HIGH);
}

void MotorDriver::motor_stop() {
    _motor_on = false;
    digitalWrite(_pinA, LOW);
    digitalWrite(_pinB, LOW);
}
