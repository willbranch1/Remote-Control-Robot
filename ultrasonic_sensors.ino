struct Ultrasonic {
  int id;
  int angle;
  int trig_pin;
  int echo_pin;
  float last_reading;
};

#define SOUND_TO_IN 0.0135039
#define SAFE_DISTANCE 18  // inches
//#define NUM_SENSORS  6
//struct Ultrasonic u_sensors[NUM_SENSORS] = {{1,0,11,13,0.0},
//                                            {2,45,12,14,0.0},
//                                            {3,90,13,15,0.0},
//                                            {4,180,14,16,0.0},
//                                            {5,270,15,17,0.0},
//                                            {6,315,16,18,0.0}};

#define NUM_SENSORS 2
struct Ultrasonic u_sensors[NUM_SENSORS] = {{1, 0, 22, 21, 0.0},
                                            {2, 180, 17, 16, 0.0}};

void read_sensor(struct Ultrasonic *u) {
  // put code here
  // u->last_reading = ...
  digitalWrite(u->trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(u->trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(u->trig_pin, LOW);
  u->last_reading = pulseIn(u->echo_pin, HIGH) * SOUND_TO_IN / 2;
  //Serial.printf("%d read %f\n", u->angle, u->last_reading);
}

void read_all_sensors(void) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    read_sensor(&u_sensors[i]);
    //Serial.printf("%d:%f\n", u_sensors[i].angle, u_sensors[i].last_reading);
  }
}

void print_sensor_data(void) {
  char buffer[30];
  for (int i = 0; i < NUM_SENSORS; i++) {
    sprintf(buffer, "%d:%s", u_sensors[i].angle, u_sensors[i].last_reading);
    Serial.println(buffer);
  }
}

void graph_sensor_data() {
  int x, y;
  for (int i = 0; i < NUM_SENSORS; i++) {
    x = u_sensors[i].last_reading * cos(u_sensors[i].angle);
    y = u_sensors[i].last_reading * sin(u_sensors[i].angle);
    //scale, color, then graph (x,y)
  }
}

int scan_for_obstacles(int min_angle, int max_angle) {

//  int angle = -1;
//  int mmin_angle;
//  for (int i = 0; i < NUM_SENSORS; i++) {
//    if ((u_sensors[i].angle >= min_angle) && (u_sensors[i].angle <= max_angle)) {
//      if (u_sensors[i].last_reading <= SAFE_DISTANCE) {
//        angle = u_sensors[i].angle;
//        break;
//      }
//    }
//  }
//  return angle;
  int angle = -1;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (min_angle < max_angle) {
      if ((u_sensors[i].angle >= min_angle) && (u_sensors[i].angle <= max_angle)) {
        if (u_sensors[i].last_reading <= SAFE_DISTANCE) {
          angle = u_sensors[i].angle;
          break;
        }
      }
    } else {
      if ((u_sensors[i].angle >= min_angle) || (u_sensors[i].angle <= max_angle)) {
        if (u_sensors[i].last_reading <= SAFE_DISTANCE) {
          angle = u_sensors[i].angle;
          break;
        }
      }
    }
  }
  return angle;
}

int check_front_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(315, 45);
  return (angle);
}

int check_rear_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(135, 225);
  return (angle);
}

int check_right_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(45, 135);
  return (angle);
}

int check_left_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(225, 315);
  return (angle);
}

void print_sensor_data(int angle) {
  read_all_sensors();
  if (angle < 0) {
    Serial.printf("%d:%f\n", angle, 0.0);
  } else {
    for (int i = 0; i < NUM_SENSORS; i++) {
      if (u_sensors[i].angle == angle) {
        Serial.printf("%d:%f\n", angle, u_sensors[i].last_reading);
        break;
      }
    }
  }
}

void setup() {
  Serial.begin(115200); // open the serial port at 9600 bps:

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(u_sensors[i].trig_pin, OUTPUT);
    pinMode(u_sensors[i].echo_pin, INPUT);
  }
  
  read_all_sensors();

}

void loop() {
  delay(250); // every 250 msec, read all sensors
  read_all_sensors();
  if (Serial.available()) {
    char com = Serial.read();
    if (com == 'y') {
      Serial.println(check_front_obstacle());
    } 
    else if (com == 'h') {
      Serial.println(check_rear_obstacle());
    } 
    else if (com == 'g') {
      Serial.println(check_left_obstacle());
    } 
    else if (com == 'j') {
      Serial.println(check_right_obstacle());
    }
  }
}
