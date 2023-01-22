#include <EEPROM_Rotate.h>

class PermanentStorage 
{
    public:
        PermanentStorage();

        void begin();
        void error_during_ota();
        void prepare_for_ota();

        int read_last_known_shade_position();
        int read_closed_position();

        void write_last_known_shade_position(int to_write);
        void write_closed_position(int to_write);

    private:
        int _eeprom_read_int();
        void _eeprom_write_int(int index, int value);

        EEPROM_Rotate _eepromr;
}