class MotorDriver 
{
    public:
        MotorDriver(int pinA, int pinB, int pinEncoder, int max);

        void begin();
        void loop();

        bool at_target();
        void set_target(int target);

        void set_max(int max);
        void set_current(int current);

    private: 
        void motor_forward();
        void motor_backward();
        void motor_stop();
        int _pinA;
        int _pinB;
        int _pinEncoder;
        int _min = 0;
        int _max;
        int _target = 0;
        int _current = 0;
        bool _tripped = false;
};
