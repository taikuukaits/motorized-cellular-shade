class PermanentStorage 
{
    public:
        PermanentStorage();

        void setup();
        void error_during_ota();
        void prepare_for_ota();

        void read_last_known_shade_position();
        void read_closed_position();

        void write_last_known_shade_position(int to_write);
        void write_closed_position(int to_write);

    private:
        void _eeprom_read_int();
        void _eeprom_write_int();
