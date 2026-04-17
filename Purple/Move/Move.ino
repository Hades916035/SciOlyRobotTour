#define FL1 5
#define FL2 4

#define FR1 3
#define FR2 2

#define BR1 11
#define BR2 10

#define BL1 9
#define BL2 8

#define TRIG_FRONT 13
#define ECHO_FRONT 12

#define TRIG_LEFT A2
#define ECHO_LEFT A3

#define TRIG_BACK A1
#define ECHO_BACK A0

#define TRIG_RIGHT A4
#define ECHO_RIGHT A5

#define BUTTON 7

#define FRONT_BACK_LIMIT 1.5
#define SIDE_LIMIT 3.5

#define moe 0.1

int speedVal = 150;
bool bottle = false;

// ---------------- ULTRASONIC ----------------

float getDistance(int trig, int echo){

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 20000);

  if(duration == 0) return -1;

  float distance = duration * 0.0133 / 2.0;

  return distance;
}

// ---------------- PRINT ----------------

void printDistances(){

  float front = getDistance(TRIG_FRONT, ECHO_FRONT);
  float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
  float back  = getDistance(TRIG_BACK, ECHO_BACK);
  float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

  Serial.print("Front: "); Serial.print(front); Serial.print(" in | ");
  Serial.print("Left: ");  Serial.print(left);  Serial.print(" in | ");
  Serial.print("Back: ");  Serial.print(back);  Serial.print(" in | ");
  Serial.print("Right: "); Serial.print(right); Serial.println(" in");
}

// --------------- Centering ----------------

void center(float f, float r, float l, float b) {

  while(true) {

  float df = getDistance(TRIG_FRONT, ECHO_FRONT);
  float db  = getDistance(TRIG_BACK, ECHO_BACK);
  float dl  = getDistance(TRIG_LEFT, ECHO_LEFT);
  float dr = getDistance(TRIG_RIGHT, ECHO_RIGHT);

    if(f != -1) {
      if (f < df - moe) {
        forward(50);
      } else if (f > df + moe) {
        backward(50);
      } else {
        stopMotors();
        f = -1;
      }
    }

    if(r != -1) {
      if (r < dr - moe) {
        right(50);
      } else if (r > dr + moe) {
        left(50);
      } else {
        stopMotors();
        r = -1;
      }
    }

    if(l != -1) {
      if (l < dl - moe) {
        left(50);
      } else if (l > dl + moe) {
        right(50);
      } else {
        stopMotors();
        l = -1;
      }
    }

    if(b != -1) {
      if (b < db - moe) {
        backward(50);
      } else if (b > db + moe) {
        forward(50);
      } else {
        stopMotors();
        b = -1;
      }
    }

    if(f==-1 && r==-1 && l==-1 && b==-1) {
      break;
    }
  }
}

// --------------- Straighten ----------------

void bigStraight(bool f, bool b, bool l, bool r){

  rotateRight(500);
  delay(500);

  Serial.println("Straightening...");

  int scanTime = 1500;     // total scan duration (ms)
  int stepTime = 20;       // time per step
  int turnSpeed = 120;

  float bestError = 9999;
  int bestTime = 0;

  float start = millis();

  // int currentTime = 0;

  // -------- SCAN LEFT --------
  while(millis() - start < scanTime){

    // rotate LEFT
    setFL(-turnSpeed);
    setFR(turnSpeed);
    setBL(-turnSpeed);
    setBR(turnSpeed);

    delay(stepTime);
    // currentTime += stepTime;

    float front = getDistance(TRIG_FRONT, ECHO_FRONT);
    float back  = getDistance(TRIG_BACK, ECHO_BACK);
    float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
    float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

    printDistances();

    // skip bad readings
    if(front < 0 || back < 0 || left < 0 || right < 0) continue;

    // error = how "un-square" we are
    if (!f) {
      front=0;
    }
    if (!b) {
      back=0;
    }
    if (!l) {
      left=0;
    }
    if(!r) {
      right=0;
    }

    float error = front+back+right+left;

    if(error < bestError){
      bestError = error;
      bestTime = millis() - start;
    }
  }

  stopMotors();
  delay(200);

  // -------- RETURN TO BEST POSITION --------
  Serial.println("Returning to best alignment...");

  // currentTime = 0;

  start = millis();
  float error = 10000000;

  while(error > bestError + .3){

    float front = getDistance(TRIG_FRONT, ECHO_FRONT);
    float back  = getDistance(TRIG_BACK, ECHO_BACK);
    float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
    float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

    printDistances();

    // skip bad readings
    if(front < 0 || back < 0 || left < 0 || right < 0) continue;

    // error = how "un-square" we are
    if (!f) {
      front=0;
    }
    if (!b) {
      back=0;
    }
    if (!l) {
      left=0;
    }
    if(!r) {
      right=0;
    }

    error = front+back+right+left;

    // rotate RIGHT to go back
    setFL(turnSpeed);
    setFR(-turnSpeed);
    setBL(turnSpeed);
    setBR(-turnSpeed);

    delay(stepTime);
    // currentTime += stepTime;
  }

  stopMotors();

  Serial.println("Straightened.");
}

// ---------------- OBSTACLE ----------------

bool obstacleDetected(){

  float front = getDistance(TRIG_FRONT, ECHO_FRONT);
  float back  = getDistance(TRIG_BACK, ECHO_BACK);
  float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
  float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

  if(!bottle) {
    if(front > 0 && front < FRONT_BACK_LIMIT) return true;
  }
  if(back  > 0 && back  < FRONT_BACK_LIMIT) return true;
  if(left  > 0 && left  < SIDE_LIMIT) return true;
  if(right > 0 && right < SIDE_LIMIT) return true;

  return false;
}

// ---------------- MOTOR CONTROL ----------------

void setMotor(int pin1, int pin2, int speedVal){
  if(speedVal > 0){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  }
  else if(speedVal < 0){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  }
  else{
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

void setFL(int s){ setMotor(FL1, FL2, s); }
void setFR(int s){ setMotor(FR1, FR2, -s); }
void setBR(int s){ setMotor(BR1, BR2, s); }
void setBL(int s){ setMotor(BL1, BL2, -s); }

void stopMotors(){
  setFL(0);
  setFR(0);
  setBR(0);
  setBL(0);
}

// ---------------- MECANUM MOVEMENT ----------------

void forward(float goal){

  float start = millis();

  while(true && millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(speedVal);
    setFR(speedVal);
    setBL(speedVal);
    setBR(speedVal);

    delay(20);
  }

    stopMotors();

}

void backward(float goal){

  float start = millis();

  while(true && millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(-speedVal);
    setFR(-speedVal);
    setBL(-speedVal);
    setBR(-speedVal);

    delay(20);
  }

    stopMotors();

}

void right(float goal){

  float start = millis();

  while(true && millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(speedVal);
    setFR(-speedVal);
    setBL(-speedVal);
    setBR(speedVal);

    delay(20);
  }

    stopMotors();

}

void left(float goal){

  float start = millis();

  while(true && millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(-speedVal);
    setFR(speedVal);
    setBL(speedVal);
    setBR(-speedVal);

    delay(20);
  }

  stopMotors();

}

void rotateRight(float goal){

  float start = millis();

  while(millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(speedVal);
    setFR(-speedVal);
    setBL(speedVal);
    setBR(-speedVal);

    delay(20);
  }

  stopMotors();

}

void rotateLeft(float goal){

  float start = millis();

  while(true && millis() - start < goal){

    printDistances();

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(-speedVal);
    setFR(speedVal);
    setBL(-speedVal);
    setBR(speedVal);

    delay(20);
  }

    stopMotors();

}

// ---------------- SETUP ----------------

void setup(){

  Serial.begin(9600);

  pinMode(FL1, OUTPUT);
  pinMode(FL2, OUTPUT);

  pinMode(FR1, OUTPUT);
  pinMode(FR2, OUTPUT);

  pinMode(BR1, OUTPUT);
  pinMode(BR2, OUTPUT);

  pinMode(BL1, OUTPUT);
  pinMode(BL2, OUTPUT);

  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  pinMode(TRIG_BACK, OUTPUT);
  pinMode(ECHO_BACK, INPUT);

  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  pinMode(BUTTON, INPUT_PULLUP);

  stopMotors();

  Serial.println("System Ready");
}

// ---------------- LOOP ----------------

void loop(){

  printDistances();

  if(digitalRead(BUTTON) == LOW){

    // center(-1, 5.13, -1, 5);
    bigStraight(true, true, false, false);

  }

  delay(200);
}

// straight --> front, back, left, right
//  right: 5.13  left: 5.75  front: 5.01   back: 4.99      (all inches)
//fwd & back 2700 ms = 50 cm
//right & left 3100 ms = 50 cm     right go less than left
//rotate 1860 ms = 90 degrees