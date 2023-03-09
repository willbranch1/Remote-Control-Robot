/*
 * Program that checks each ultrasonic sensor every 250 ms and reports back to the main computer if there are any obstacles with accompanying data. 
 * It also includes additional functionality such as the ability to print out all of the sensors data at once, as well as the ability to plot
 * obstacles around the robot. 
 *
 * Currently, there are only four sensors that are able to be programmed. But, with the change of NUM_SENSORS and the array of structs u_sensors, you
 * are able to add additional sensors.
 */
struct Ultrasonic {
  int id;
  int angle;
  int trig_pin;
  int echo_pin;
  float last_reading;
};

#define SOUND_TO_IN 0.0135039
#define SAFE_DISTANCE 18  // inches
#define NUM_SENSORS 2
struct Ultrasonic u_sensors[NUM_SENSORS] = {{1, 0, 22, 21, 0.0},
                                            {2, 180, 17, 16, 0.0}};

//scans for obstacles from the given angle range
//the angle range is set up in four categories: front(315 to 45), right(45 to 135), rear(135 to 225), and left(225 to 315).
//does not allow for 
int scan_for_obstacles(int min_angle, int max_angle) {
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


//check if the obstacle is in the front
int check_front_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(315, 45);
  return (angle);
}

//check if obstacle is to the rear
int check_rear_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(135, 225);
  return (angle);
}

//checks if obstacle is to the right
int check_right_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(45, 135);
  return (angle);
}

//checks if obstacle is to the left
int check_left_obstacle(void) {
  int angle;
  angle = scan_for_obstacles(225, 315);
  return (angle);
}

//calls read_sensor for every available sensor
void read_all_sensors(void) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    read_sensor(&u_sensors[i]);
  }
}

//checks an individual, specified sensor
void read_sensor(struct Ultrasonic *u) {
  digitalWrite(u->trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(u->trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(u->trig_pin, LOW);
  u->last_reading = pulseIn(u->echo_pin, HIGH) * SOUND_TO_IN / 2;
}

//method that can be used to print the sensor data for each sensor line by line
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

//function that can be used to display a plot of the identified objects around the robot
void graph_sensor_data() {
  int x, y;
  for (int i = 0; i < NUM_SENSORS; i++) {
    x = u_sensors[i].last_reading * cos(u_sensors[i].angle);
    y = u_sensors[i].last_reading * sin(u_sensors[i].angle);
    //scale, color, then graph (x,y)
  }
}

void setup() {
  Serial.begin(115200); // open the serial port at 9600 bps:

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(u_sensors[i].trig_pin, OUTPUT);
    pinMode(u_sensors[i].echo_pin, INPUT);
  }

}

void loop() {
  delay(250); // every 250 msec, read all sensors
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
