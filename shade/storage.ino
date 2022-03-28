#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

#define MAX_STEPS_INDEX 10
#define POSITION_INDEX 14

//Not sure about these values, can't find a good explanation of what I want. This is what one of the examples uses and some of the uses I saw on github.
#define EEPROM_SECTORS 4
#define EEPROM_SPACE 256

bool storageSetup() {
    EEPROMr.size(EEPROM_SECTORS);
    EEPROMr.begin(EEPROM_SPACE);   
}

void storageSavePosition(int pos) {
  eepromWriteInt(POSITION_INDEX, pos); 
}

int storageReadPosition() {
  return eepromReadInt(POSITION_INDEX);
}

void storageSaveMaxSteps(int pos) {
  eepromWriteInt(MAX_STEPS_INDEX, pos); 
}

int storageReadMaxSteps() {
  return eepromReadInt(MAX_STEPS_INDEX);
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
