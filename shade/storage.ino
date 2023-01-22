#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

#define CLOSED_POSITION_INDEX 10
#define CURRENT_POSITION_INDEX 14

//Not sure about these values, can't find a good explanation of what I want. This is what one of the examples uses and some of the uses I saw on github.
#define EEPROM_SECTORS 4
#define EEPROM_SPACE 4096

void storageSetup() {
    EEPROMr.size(EEPROM_SECTORS);
    EEPROMr.begin(EEPROM_SPACE);
}

// current position is for the motor
void storageSaveCurrentPosition(int pos) {
  eepromWriteInt(CURRENT_POSITION_INDEX, pos); 
}

int storageReadCurrentPosition() {
  return eepromReadInt(CURRENT_POSITION_INDEX);
}

// closed position is used by the cover system
void storageSaveClosedPosition(int pos) {
  eepromWriteInt(CLOSED_POSITION_INDEX, pos); 
}

int storageReadClosedPosition() {
  return eepromReadInt(CLOSED_POSITION_INDEX);
}

void eepromWriteInt(int index, int value)
{
    EEPROMr.write(index, value >> 8);
    EEPROMr.write(index + 1, value & 0xFF);
    EEPROMr.commit();
}

int eepromReadInt(int index)
{
    return (EEPROMr.read(index) << 8) + EEPROMr.read(index + 1);
}

void storagePrepareForOTA() {
    EEPROMr.rotate(false);
    EEPROMr.commit();
}

void storageErrorDuringOTA () {
  EEPROMr.rotate(true);
}
