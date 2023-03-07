#include <Kangaroo.h> // a library to control the motor driver

//serial transmit and receive pins
#define TXp1 17 
#define RXp1 16

//state machine definitions
#define STOPPED 0
#define MOVING_FOR 1
#define MOVING_REV 2
#define TURNING_RIGHT 3
#define TURNING_LEFT 4
#define SPINNING 5

//relay GPIO pins
#define r1 27
#define r2 26
#define r3 25
#define r4 33

//initializing the serial connection with the Kangaroo motor driver
KangarooSerial  K(Serial1);
KangarooChannel Drive(K, 'D');
KangarooChannel Turn(K, 'T');

//variable ramping speed that can be changed by the computer main brain
int ramping;

const int buff_len = 10;//chars plus null ternminator for atoi()

const int max_speed_per = 70;

int max_for = 1306; 
int max_rev = -1306;

int speed_per;
int turn_per;
int state;

//initiate forward movement
void forward(int percent) {
  Drive.s(max_for * percent / 100, ramping);
  state = MOVING_FOR;
}

//initiate reverse movement
void reverse(int percent) {
  Drive.s(max_rev * percent / 100, ramping);
  state = MOVING_REV;
}

//stop all motors while moving or spinning
void stopMotors() {
  if (state == MOVING_FOR || state == MOVING_REV) {
   Drive.s(0, ramping);
  } else if (state == SPINNING) {
   Turn.s(0, ramping);
  }
  state = STOPPED;
}

//stop motors while moving in a straight line
void stopDrive() {
  Drive.s(0, ramping);
}

//stop motors while turning
void stopTurn() {
  Turn.s(0, ramping);  
}

//spin to the right
void spinRight(int percent) {
  if (state == STOPPED) {
    Turn.s(max_rev * percent / 100, ramping);
    state = SPINNING;
  }
}

//spin to the left
void spinLeft(int percent) {
  if (state == STOPPED) {
    Turn.s(max_for * percent / 100, ramping);
    state = SPINNING;
  }
}

//turn right while moving forward or reverse
void turnRight(int percent) {
  Turn.s(max_rev * percent / 100, ramping);
}

//turn left while moving forward or reverse
void turnLeft(int percent) {
  Turn.s(max_for * percent / 100, ramping);
}

//increase speed percent
void incrementSpeed() {
  if (speed_per < 50) {
    speed_per += 5;
  }
}

//decrese speed percent
void decreaseSpeed() {
  if (speed_per > 4) {
    speed_per -= 5;
  }
}

//increment turn percent(turn radius)
void incrementTurn() {
  if (turn_per < 75) {
    turn_per += 5;
  }
}

//decrease turn percent(turn radius)
void decreaseTurn() {
  if (turn_per > 4) {
    turn_per -= 5;
  }
}

//reinitialize the turn and speed parameters to default values
void reinit() {
  speed_per = 15;
  turn_per = 50; 
}

//read percent given by main brain
int readPercent() {
  char buff[buff_len];
  Serial.readBytesUntil('\n', buff, buff_len);
  return atoi(buff);
}

//make sure the new speed percent is not greater than the max allowed speed
int checkSpeed(int percent) {
  if (percent > max_speed_per) {
    return max_speed_per;
  }
  return percent;
}

void setup()
{
  //initialize serial communication
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXp1, TXp1);
  delay(4000);
  
  //open the drive and turn channels with the kangaroo motor driver. set them to not moving
  Drive.start();
  Turn.start();
  
  Drive.pi(0);
  Turn.pi(0);
  
  //establish the bounds of the range that can be given to the kangaroo motor driver
  max_for = Drive.getMax().value();
  max_rev = Drive.getMin().value();
  
  speed_per = 15;
  turn_per = 15;
  ramping = 1000;

  //initialize relay pins
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  
  Serial.println("Setup complete.");
}

//The main loop whats for characters to appear from the main brain of the robot within the serial port. 
//Once characters are received, it parses them and executes a command based on the given character and percent.
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
