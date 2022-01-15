//Ronald You, Nathan Lu, Quentin Fan-Chiang
//1/12/2022
//Mr. Wong
//Implementation of methods and logic for iCar to accurately follow a line and correct itself if off course.

//Motor Variables:
int MOTOR_LEFT_1 = 9; //pin 1 of left motor
int MOTOR_LEFT_2 = 3; //pin 2 of left motor
int MOTOR_RIGHT_1 = 10; //pin 1 of right motor
int MOTOR_RIGHT_2 = 11; //pin 2 of right motor

//Photoresistor Variables:
int PHOTORESISTORLEFT = A0; //left photoresistor
int PHOTORESISTORMID = A1; //middle photoresistor
int PHOTORESISTORRIGHT = A2; //right photoresistor

//Program Control Variables:
int lightLevelLeft = 0; //light level detected by left photoresistor
int lightLevelMid = 0; //light level detected by middle photoresistor
int lightLevelRight = 0; //light level detected by right photoresistor
int lightThreshold = 575; //threshold for brightness detected by photoresistors (below threshold: black line detected, above threshold: no black line detected)
int speed = 50; //speed of robot
int state = 0; //variable storing program state (0: move forward, 1: rotate left, 2: rotate right, 3: rotete indefinitely until line is detected)
long timeAllWhite = 0; //the latest time since program started running that all sensors detect white; used to detect if robot is off course
long startTimeAllWhite = 0; //start time since program started running that all sensors detect white; used to detect if robot is off course


//setting up pins to be "input" pins or "output" pins
void setup()
{
  pinMode(MOTOR_RIGHT_2, OUTPUT);
  pinMode(MOTOR_RIGHT_1, OUTPUT);
  pinMode(MOTOR_LEFT_2, OUTPUT);
  pinMode(MOTOR_LEFT_1, OUTPUT);
  pinMode(PHOTORESISTORLEFT, INPUT);
  pinMode(PHOTORESISTORRIGHT, INPUT);
  pinMode(PHOTORESISTORMID, INPUT);
}

//main program control function
void loop() {
  //reading light levels from photoresistors
  lightLevelLeft = analogRead(PHOTORESISTORLEFT);
  lightLevelMid = analogRead(PHOTORESISTORMID);
  lightLevelRight = analogRead(PHOTORESISTORRIGHT);
  
  //if structure that decides whether to go forward, rotate right, rotate left, or rotate indefinitely according until line is sensed to the program state variable
  if (state == 0) {
    ifGoingForward(lightLevelLeft, lightLevelMid, lightLevelRight);
  }
  else if (state == 1) {
    ifTurningLeft(lightLevelLeft, lightLevelMid, lightLevelRight);
    startTimeAllWhite = millis();
  }
  else if (state == 2) {
    ifTurningRight(lightLevelLeft, lightLevelMid, lightLevelRight);
    startTimeAllWhite = millis();
  }
  else if (state == 3) {
    rotateIndefinitly();
    startTimeAllWhite = millis();
  }
}

//function that dictates logic for robot moving forward; must decide whether to continue moving forward, rotate in one direction, or rotate indefinitely
void ifGoingForward(int lightLevelLeft, int lightLevelMid, int lightLevelRight) {
  //if sensors detect [dark][dark][light], the line turns to the left, and the robot should rotate left to follow the line
  if (lightLevelLeft < lightThreshold && lightLevelMid < lightThreshold && lightLevelRight > lightThreshold) {
    startTimeAllWhite = millis();
    state = 1;
  }
  //if sensors detect [dark][light][light], the line turns to the left, and the robot should rotate left to follow the line
  else if (lightLevelLeft < lightThreshold && lightLevelMid > lightThreshold && lightLevelRight > lightThreshold) {
    startTimeAllWhite = millis();
    state = 1;
  }
  //if sensors detect [light][dark][dark], the line turns to the right, and the robot should rotate right to follow the line
  else if (lightLevelLeft > lightThreshold && lightLevelMid < lightThreshold && lightLevelRight < lightThreshold) {
    startTimeAllWhite = millis();
    state = 2;
  }
  //if sensors detect [light][light][dark], the line turns to the right, and the robot should rotate right to follow the line
  else if (lightLevelLeft > lightThreshold && lightLevelMid > lightThreshold && lightLevelRight < lightThreshold) {
    startTimeAllWhite = millis();
    state = 2;
  }
  //if all sensors detect black, the robot is at a T-intersection; rotate right to begin navigating around it
  else if (lightLevelLeft < lightThreshold && lightLevelMid < lightThreshold && lightLevelRight < lightThreshold) {
    state = 2;
    startTimeAllWhite = millis();
  }
  //if all sensors detect white, robot may be off course; rotate indefinitely only if all white has been detected for an extended period of time
  else if (lightLevelLeft > lightThreshold && lightLevelMid > lightThreshold && lightLevelRight > lightThreshold) {
    timeAllWhite=millis();
    long diff = timeAllWhite - startTimeAllWhite; //find period of time for which all white has been detected (difference between latest time and start time)
    if (diff>1000) state = 3; //if period of time exceeds 1000 milliseconds, robot is off course; begin rotating indefintely until line is detected
  }
  else {
    startTimeAllWhite = millis();
  } 
  forward();
}

//function that decides whether to continue turning right or return to moving forward
void ifTurningRight(int lightLevelLeft, int lightLevelMid, int lightLevelRight) {
  //if the right sensor continues to detect the black line, continue turning right until it doesn't
  if (lightLevelRight<lightThreshold) {
    right();
  }
  else {
    state = 0;
  }
}

//function that decides whether to continue turning left or return to moving forward
void ifTurningLeft(int lightLevelLeft, int lightLevelMid, int lightLevelRight) {
  //if the left sensor continues to detect the black line, continue turning left until it doesn't
  if (lightLevelLeft<lightThreshold) {
    left();
  }
  else {
    state = 0;
  }
}

//function that forces robot to rotate in place indefinitly until a line is detected again
void rotateIndefinitly () {
  //continue rotating towards the right while staying in place until right photoresistor detects line
  if(lightLevelRight < lightThreshold){
     state=0;
  }
  else{
    right();
  }
}

//make robot move forward
void forward()
{
  digitalWrite(MOTOR_RIGHT_2, LOW);
  digitalWrite(MOTOR_LEFT_2, LOW);
  analogWrite(MOTOR_RIGHT_1, speed);
  analogWrite(MOTOR_LEFT_1, speed);
}

//make robot move backward
void backward()
{
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_LEFT_1, LOW);
  analogWrite(MOTOR_RIGHT_2, speed);
  analogWrite(MOTOR_LEFT_2, speed);
}

//make robot rotate right on the spot
void right()
{
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  analogWrite(MOTOR_RIGHT_2, speed);
  analogWrite(MOTOR_LEFT_1, speed);
}

//make robot rotate left on the spot
void left()
{
  digitalWrite(MOTOR_RIGHT_2, LOW);
  digitalWrite(MOTOR_LEFT_1, LOW);
  analogWrite(MOTOR_LEFT_2, speed);
  analogWrite(MOTOR_RIGHT_1, speed);
}
