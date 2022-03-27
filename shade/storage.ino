#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

#define POSITION_INDEX 0
#define MAX_STEPS_INDEX 4
#define LEFT_OFFSET_INDEX 8
#define RIGHT_OFFSET_INDEX 12

#define EEPROM_SECTORS 1
#define EEPROM_SPACE 16

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

void storageSaveLeftOffset(int pos) {
  eepromWriteInt(LEFT_OFFSET_INDEX, pos); 
}

int storageReadLeftOffset() {
  eepromReadInt(LEFT_OFFSET_INDEX);
}

void storageSaveRightOffset(int pos) {
  eepromWriteInt(RIGHT_OFFSET_INDEX, pos); 
}

int storageReadRightOffset() {
  eepromReadInt(RIGHT_OFFSET_INDEX);
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
