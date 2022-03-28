class MotorDriver 
{
    public:
        MotorDriver(int pinA, int pinB, int pinEncoder);

        void begin();
        void loop();
        
        void move_forward_steps(int steps);
        void move_backward_steps(int steps);
        bool is_moving();

    private: 
        void motor_stop();

        int _steps;
        int _pinA;
        int _pinB;
        int _pinEncoder;
        bool _tripped = false;
        bool _motor_on = false;
};
