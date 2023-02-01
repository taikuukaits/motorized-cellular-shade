// Some wrappers around println to clean up the code a bit, so rather than serial.print("long string:"); serial.println(num); you just have log_num(num)
#include "Arduino.h"
#include "config.h"

void log_setup_welcome() {
  Serial.println("----------------------------------------------");
  Serial.println();
  Serial.println("Motorized Cellular Shade v" SOFTWARE_VERSION);
  Serial.println();
  Serial.println("----------------------------------------------");
}

void log_setup_starting_state(int shade_position, int closed_position) {
  Serial.println("State of the device.");
  Serial.print("Last Shade Position: ");
  Serial.println(shade_position);
  Serial.print("Closed Position: ");
  Serial.println(closed_position);
  Serial.println("----------------------------------------------");
}

void log_setup_complete() {
  Serial.println("Shade succesfully started.");
  Serial.println("----------------------------------------------");
}

void log_calibrated_position(int position) {
  Serial.print("Updating current shade position to ");
  Serial.println(position);
}

void log_calibrated_closed(int closed){ 
  Serial.print("Updating close position to ");
  Serial.println(closed);
}

void log_jogged(int jog_amount) {
  Serial.print("Jogging ");
  Serial.print(jog_amount);
  Serial.print(" shade position.");
}