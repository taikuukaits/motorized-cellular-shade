class MotorDriver 
{
    public:
        MotorDriver(int motorPinPositive, int motorPinNegative, int encoderPinOne, int encoderPinTwo);

        void setup(); // The position always starts at 0 after setup. The position is read-only. 
        void loop();
        bool request_move(int new_target);
        bool is_moving(); // If moving, no new moves can be requested. If not moving, requested target reached. 

        // Called whenever the position has been officially changed - as in reached the target. May also be called if movement detected but not moving. 
        // The provided position is considered at 'rest' and likely should be saved for use in 'absolute' movements, it is relative to the zero from setup.
        void set_position_changed_callback(void (*callback)(int)); 

    private: 
        void motor_stop();
        Encoder encoder;
        
        int _target_position = 0; // only valid if moving
        int _current_position = 0; // always valid, relative to encoder startup. starts at zero from power up.
        bool _direction_positive = false; // only valid if moving, target is in the positive direction or negative direction

        bool _reached_target = false;
        unsigned long _millis_when_reached_target = 0;
        int _reached_target_delay = 50; // how long to wait after reaching the target to be done moving
        unsigned long _millis_when_position_last_changed = 0;
        int _position_change_stall_delay = 50; // how long to wait before assuming motor has stalled

        bool _current_position_changed = false;
};
