int motorLeft_B_1 = 16;
int motorLeft_B_2 = 5;
int motorLeft_A_1 = 4;
int motorLeft_A_2 = 0;

int sensorLeft = 14;

void setup() {
  pinMode(sensorLeft, INPUT_PULLUP);
  pinMode(motorLeft_B_1, OUTPUT);
  pinMode(motorLeft_B_2, OUTPUT);
  Serial.begin(9600);
  motorForward(motorLeft_B_1, motorLeft_B_2); 
}


bool state = false;
int count = 0;
int dir_count = 5;
bool dir = false;
bool sensor_tripped = false;
void loop() {
  bool sensor = digitalRead(sensorLeft);
  if (sensor && !sensor_tripped) {
    sensor_tripped = true;
    state = !state;
    count++;
    if (count >= dir_count) {
      count = 0;
      dir = !dir;
      if (dir) {
        motorForward(motorLeft_B_1, motorLeft_B_2);
      } else {
        motorBackward(motorLeft_B_1, motorLeft_B_2);
      }

    }
  } else if (!sensor) {
    sensor_tripped=false;
  }
  Serial.println(count);
}

void motorForward(int one, int two) {
  digitalWrite(one, HIGH);
  digitalWrite(two, LOW);
}

void motorBackward(int one, int two) {
  digitalWrite(one, LOW);
  digitalWrite(two, HIGH);  
}

void motorStop(int one, int two) {
  digitalWrite(one, LOW);
  digitalWrite(two, HIGH);  
}