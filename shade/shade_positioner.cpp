#include "shade_positioner.h"

ShadePositioner::ShadePositioner(MotorDriver motor_driver, PermanentStorage storage) : _motor_driver(motor_driver), _storage(storage) {
}

void ShadePositioner::begin() {
    _last_known_shade_position = _storage.read_last_known_shade_position();
    _closed_position = _storage.read_closed_position();
}

void ShadePositioner::loop() {
    int current_motor_position = _motor_driver.get_last_known_rest_position();
    if (current_motor_position != _last_known_motor_position) {
        int delta = _last_known_motor_position - current_motor_position;
        _last_known_shade_position = _last_known_shade_position + delta;
        _last_known_motor_position = current_motor_position;
        _storage.write_last_known_shade_position(_last_known_shade_position);
    }
}
void ShadePositioner::request_stop() {
    _motor_driver.request_stop();
}

bool ShadePositioner::is_moving() {
    return _motor_driver.is_moving();
}

bool ShadePositioner::is_open() {
    return !_motor_driver.is_moving() && _last_known_shade_position < 0;
}

bool ShadePositioner::is_closed() {
    return !_motor_driver.is_moving() && _last_known_shade_position > _closed_position;
}

bool ShadePositioner::is_stopped() {
    return !_motor_driver.is_moving() && !is_open() && !is_closed();
}

int ShadePositioner::get_shade_position() {
    return _last_known_shade_position;
}

void ShadePositioner::move_to_close_position() {
    move_to_shade_position(_closed_position);
}

void ShadePositioner::move_to_shade_position(int target) {
    if (_motor_driver.is_moving()) {
        Serial.println("Can't move! Already moving.");
        return;
    }

    //say last_shade = 20, target = 30
    //and last_motor = 15, calculated target = 25
    //delta = target - last_shade = 30 - 20 = 10
    //target_motor = last_motor + delta = 15 + 10 = 25
    int delta = target - _last_known_shade_position;
    int target_motor_position = _last_known_motor_position + delta;
    _motor_driver.request_move(target_motor_position);
}

void ShadePositioner::calibration_shade_is_currently_at(int shade_position) {
    _last_known_shade_position = shade_position;
    _storage.write_last_known_shade_position(_last_known_shade_position);
}

void ShadePositioner::calibration_set_shade_closed_position(int closed_position) {
    _closed_position = closed_position;
    _storage.write_closed_position(_closed_position);
}
