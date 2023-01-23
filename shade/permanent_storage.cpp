#include "permanent_storage.h"

#define CLOSED_POSITION_INDEX 10
#define LAST_KNOWN_SHADE_POSITION_INDEX 14

//Not sure about these values, can't find a good explanation of what I want. This is what one of the examples uses and some of the uses I saw on github.
#define EEPROM_SECTORS 4
#define EEPROM_SPACE 4096

PermanentStorage::PermanentStorage() {

}

// this should probably be passed in as constructor args, idk, only instantiate one :) 
void PermanentStorage::begin() {
    _eepromr.size(EEPROM_SECTORS);
    _eepromr.begin(EEPROM_SPACE);
}

void PermanentStorage::write_last_known_shade_position(int to_write) {
  _eeprom_write_int(LAST_KNOWN_SHADE_POSITION_INDEX, to_write); 
}

int PermanentStorage::read_last_known_shade_position() {
  return _eeprom_read_int(LAST_KNOWN_SHADE_POSITION_INDEX);
}

void PermanentStorage::write_closed_position(int to_write) {
  _eeprom_write_int(CLOSED_POSITION_INDEX, to_write); 
}

int PermanentStorage::read_closed_position() {
  return _eeprom_read_int(CLOSED_POSITION_INDEX);
}

void PermanentStorage::_eeprom_write_int(int index, int value)
{
    _eepromr.write(index, value >> 8);
    _eepromr.write(index + 1, value & 0xFF);
    _eepromr.commit();
}

int PermanentStorage::_eeprom_read_int(int index)
{
    return (_eepromr.read(index) << 8) + _eepromr.read(index + 1);
}

void PermanentStorage::prepare_for_ota() {
    _eepromr.rotate(false);
    _eepromr.commit();
}

void PermanentStorage::error_during_ota() {
  _eepromr.rotate(true);
}
