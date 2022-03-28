#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

#define POSITION_INDEX 10
#define MAX_STEPS_INDEX 20

#define EEPROM_SECTORS 4
#define EEPROM_SPACE 4096

void storageSetup() {
    EEPROMr.size(EEPROM_SECTORS);
    EEPROMr.begin(EEPROM_SPACE);
}

void storageSavePosition(int pos) {
  eepromWriteInt(POSITION_INDEX, pos); 
}

int storageReadPosition() {
  eepromReadInt(POSITION_INDEX);
}

void storageSaveMaxSteps(int pos) {
  eepromWriteInt(MAX_STEPS_INDEX, pos); 
}

int storageReadMaxSteps() {
  eepromReadInt(MAX_STEPS_INDEX);
}

void eepromWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROMr.write(pos, *p);
    EEPROMr.write(pos + 1, *(p + 1));
    EEPROMr.write(pos + 2, *(p + 2));
    EEPROMr.write(pos + 3, *(p + 3));
    EEPROMr.commit();
}

int eepromReadInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROMr.read(pos);
  *(p + 1)  = EEPROMr.read(pos + 1);
  *(p + 2)  = EEPROMr.read(pos + 2);
  *(p + 3)  = EEPROMr.read(pos + 3);
  return val;
}

void storagePrepareForOTA() {
    EEPROMr.rotate(false);
    EEPROMr.commit();
}

void storageErrorDuringOTA () {
  EEPROMr.rotate(true);
}
