#include "motor_driver.h"
#include "permanent_storage.h"

class ShadePositioner
{
    public:
        ShadePositioner(MotorDriver motor_driver, PermanentStorage storage);

        void begin();
        void loop();

        bool is_open();
        bool is_closed();
        bool is_stopped();
        bool is_moving();

        int get_shade_position();
        int get_closed_position();
        
        void move_to_shade_position(int target); 
        void move_to_close_position();
        void request_stop();

        void calibration_shade_is_currently_at(int shade_position);
        void calibration_set_shade_closed_position(int closed_position);

    private: 
        // if we are moving, these are not the current positions. 
        int _last_known_motor_position = 0;
        int _last_known_shade_position = 0;
        int _closed_position = 100;

        MotorDriver _motor_driver;
        PermanentStorage _storage;        
};

// shade starts up - motor is at position 0
// positioner reads shade position 200 from storage
// positioner knows (shade=200, motor=0)
// request comes in, set shade position to 0
// positioner goes delta = shade - target shade = -200
// issue motor.move(-200)
// after motor finishes move, positioner knows (shade=0, motor=-200)

// we need to know:
//  what shade position does the motor consider zero

// all we have is a mapping of motor 0 to some shade position
//  shade position = motor 0
// then we can 

// _current_shade_position is essentially motor_position
// 