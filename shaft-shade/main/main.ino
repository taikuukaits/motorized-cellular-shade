/* 
 *  Encoder Library: http://www.pjrc.com/teensy/td_libs_Encoder.html for setup, repo is https://github.com/lrademacher/Encoder but is a fork of https://github.com/PaulStoffregen/Encoder that works with ESP, good resource: https://blog.squix.org/2016/05/esp8266-peripherals-ky-040-rotary-encoder.html
 */

#define ENCODER_PIN_ONE 14 // D5 - interrupt safe, see https://blog.hiebl.cc/posts/wemos-d1-mini-pro-digital-interrupt-pins/ - CLK pin
#define ENCODER_PIN_TWO 12 // D6 - interrupt safe - DT
#define MOTOR_PIN_ONE 16 // D0
#define MOTOR_PIN_TWO 13 // D7

#include <Encoder.h>

Encoder myEnc(ENCODER_PIN_ONE, ENCODER_PIN_TWO);

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");

  pinMode(MOTOR_PIN_ONE, OUTPUT);
  pinMode(MOTOR_PIN_TWO, OUTPUT);

  backward();
}

void forward() {
  digitalWrite(MOTOR_PIN_ONE, LOW);
  digitalWrite(MOTOR_PIN_TWO, HIGH);
}

void backward() {
  digitalWrite(MOTOR_PIN_ONE, HIGH);
  digitalWrite(MOTOR_PIN_TWO, LOW);
}

void halt() {
  digitalWrite(MOTOR_PIN_ONE, LOW);
  digitalWrite(MOTOR_PIN_TWO, LOW);
}

long last_position  = 0;
int target = 0;
bool moving = false;

void loop_position() {
  long new_position = myEnc.read();
  if (last_position != new_position) {
    last_position = new_position;
    
    Serial.print("Moved: ");
    Serial.println(new_position);
    
    if (moving) {
      if (last_position < target) {
        forward();
      } else if (last_position > target) {
        backward();
      } else {
        halt();
        moving = false;
      }
    }
  }
}


String inString = "";  // string to hold input
int loop_find_next_target() {
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      // clear the string for new input:
      target = inString.toInt();
      moving = true;
      inString = "";
    }
  }
}
void loop() {
  if (moving){ 
    loop_position();
  } else {
    loop_find_next_target();
  }
}
