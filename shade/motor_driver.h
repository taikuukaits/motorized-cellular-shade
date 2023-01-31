#ifndef MotorDriver_H
#define MotorDriver_H

#include <Encoder.h>

class MotorDriver 
{
    public:
        MotorDriver(int motor_pin_one, int motor_pin_two, Encoder encoder);

        void begin(); // The position always starts at 0 after setup. The position is read-only. 
        void loop();
        bool request_move(int new_target);
        bool request_stop();
        bool is_moving(); // If moving, no new moves can be requested. If not moving, requested target reached. 

        int get_current_position(); // This can probably be private
        int get_last_known_rest_position(); //Last known REST, meaning this ONLY UPDATES when at rest. This is safe to store.

    private: 
        bool _has_reached_target();
        void _move_positive();
        void _move_negative();
        void _halt();
        bool _has_stall_delay_been_exceeded();
        bool _is_within_reached_target_delay();
        void _diagnostic_position_change_without_movement();
        void _diagnostic_position_change_detected_within_delay();
        void _diagnostic_motor_stall_detected();
        void _diagnostic_position_change();
        void _diagnostic_reached_target();

        bool _moving = false;

        int _target_position = 0; // only valid if moving
        int _current_position = 0; // always valid, relative to encoder startup. starts at zero from power up.
        bool _direction_positive = false; // only valid if moving, target is in the positive direction or negative direction

        bool _reached_target = false;
        unsigned long _millis_when_reached_target = 0;
        int _reached_target_delay = 500; // how long to allow position changes after reaching the delay before assuming something went wrong - ie hit position at 300, position change detected at 350, delay of 100 says thats ok, a delay of 25 says panic
        unsigned long _millis_when_position_last_changed = 0;
        int _position_change_stall_delay = 200; // how long to wait before assuming motor has stalled

        bool _current_position_changed = false;
        int _last_known_rest_position = 0;

        int _motor_pin_one;
        int _motor_pin_two;
        Encoder _encoder;
};

#endif