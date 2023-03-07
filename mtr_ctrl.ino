#include <Kangaroo.h>

#define TXp1 17
#define RXp1 16

#define STOPPED 0
#define MOVING_FOR 1
#define MOVING_REV 2
#define TURNING_RIGHT 3
#define TURNING_LEFT 4
#define SPINNING 5

#define r1 27
#define r2 26
#define r3 25
#define r4 33

const int rev = 2550;

KangarooSerial  K(Serial1);
KangarooChannel Drive(K, 'D');
KangarooChannel Turn(K, 'T');

int ramping;

const int buff_len = 10;//chars plus null ternminator for atoi()

const int max_speed_per = 70;

int max_for = 1306; 
int max_rev = -1306;

int speed_per;
int turn_per;
int state;

void forward(int percent) {
  Drive.s(max_for * percent / 100, ramping);
  state = MOVING_FOR;
}

void reverse(int percent) {
  Drive.s(max_rev * percent / 100, ramping);
  state = MOVING_REV;
}

void stopMotors() {
//  if (state == MOVING_FOR || state == MOVING_REV) {
//    Drive.s(0, 250);
//  } else if (state == SPINNING) {
//    Turn.s(0, 250);
//  }
  Drive.s(0, ramping);
  Turn.s(0, ramping);
  state = STOPPED;
}

void stopDrive() {
  Drive.s(0, ramping);
}

void stopTurn() {
  Turn.s(0, ramping);  
}

void spinRight(int percent) {
  if (state == STOPPED) {
    Turn.s(max_rev * percent / 100, ramping);
    state = SPINNING;
  }
}

void spinLeft(int percent) {
  if (state == STOPPED) {
    Turn.s(max_for * percent / 100, ramping);
    state = SPINNING;
  }
}

void turnRight(int percent) {
  Turn.s(max_rev * percent / 100, ramping);
}

void turnLeft(int percent) {
  Turn.s(max_for * percent / 100, ramping);
}

void incrementSpeed() {
  if (speed_per < 50) {
    speed_per += 5;
  }
}

void decreaseSpeed() {
  if (speed_per > 4) {
    speed_per -= 5;
  }
}

void incrementTurn() {
  if (turn_per < 75) {
    turn_per += 5;
  }
}

void decreaseTurn() {
  if (turn_per > 4) {
    turn_per -= 5;
  }
}

void reinit() {
  speed_per = 15;
  turn_per = 50; 
}

int readPercent() {
  char buff[buff_len];
  Serial.readBytesUntil('\n', buff, buff_len);
//  Serial.println(buff);
//  for (int i = 0; i < buff_len; i++) {
//    buff[i] = Serial.read();
//  }
//  buff[buff_len-1] = 0;
  return atoi(buff);
}

int checkSpeed(int percent) {
  if (percent > max_speed_per) {
    return max_speed_per;
  }
  return percent;
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXp1, TXp1);
  delay(4000);
  
  Drive.start();
  Turn.start();
  
  Drive.pi(0);
  Turn.pi(0);
  
  max_for = Drive.getMax().value();
  max_rev = Drive.getMin().value();
  
  speed_per = 15;
  turn_per = 15;
  ramping = 1000;

  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  
  Serial.println("Setup complete.");
}

void loop()
{
  if (Serial.available()) {
    char com = Serial.read();
    Serial.println(com);
    if (com == 'w') {
      int percent = readPercent();
      speed_per = checkSpeed(percent);
      forward(speed_per);
    } else if (com == '!') {
      readPercent(); //just to clear serial buffer
      stopMotors();
    } else if (com == 's') {
      int percent = readPercent();
      speed_per = checkSpeed(percent);
      reverse(speed_per);
    } else if (com == 'a') {
      int percent = readPercent();
      speed_per = checkSpeed(percent);
      turnLeft(speed_per);
    } else if (com == 'd') {
      int percent = readPercent();
      speed_per = checkSpeed(percent);
      turnRight(speed_per);
    } else if (com == 'F') {
      readPercent();
      stopDrive();
    } else if (com = 'T') {
      readPercent();
      stopTurn();
    } else if (com == '1') {
      digitalWrite(r1, !digitalRead(r1));
    } else if (com == '2') {
      digitalWrite(r2, !digitalRead(r2));
    } else if (com == '3') {
      digitalWrite(r3, !digitalRead(r3));
    } else if (com == '4') {
      digitalWrite(r4, !digitalRead(r4));
    }
  }
}
