#include "motor_driver.h"
#include "Arduino.h"

void MotorDriver::begin() {
    pinMode(_pinEncoder, INPUT_PULLUP);
    pinMode(_pinA, OUTPUT);
    pinMode(_pinB, OUTPUT);
}

void MotorDriver::loop() {
    if (at_target()) return;
    
    bool encoder_on = digitalRead(_pinEncoder);
    bool rising = _current < _target;
    if (rising) {
        motor_forward();
    } else {
        motor_backward();
    }
    bool moved = false;
    if (encoder_on && !_tripped) {
        _tripped = true;
        moved = true;
    } else if (!encoder_on) {
        _tripped = false;
    }

    if (moved) {
        if (rising) {
            _current++;
        } else {
            _current--;
        }
        if (at_target()) {
            motor_stop();
        }
    }
}

bool MotorDriver::at_target() {
    return _current == _target;
}

void MotorDriver::set_target(int target){
    _target = constrain(target, _min, _max);
}

void MotorDriver::set_max(int max) {
    _max = max;
    if (_target > _max) {
        _target = max;
    } 
}

void MotorDriver::set_current(int current) {
    _current = constrain(current, _min, _max);
}

void MotorDriver::motor_stop() {
    digitalWrite(_pinA, LOW);
    digitalWrite(_pinB, LOW);
}

void MotorDriver::motor_forward() {
    digitalWrite(_pinA, HIGH);
    digitalWrite(_pinB, LOW);
}

void MotorDriver::motor_backward() {
    digitalWrite(_pinA, LOW);
    digitalWrite(_pinB, HIGH);
}
