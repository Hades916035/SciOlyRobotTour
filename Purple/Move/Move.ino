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

#define TRIG_RIGHT 6
#define ECHO_RIGHT 7

#define FRONT_BACK_LIMIT 1.5
#define SIDE_LIMIT 3.5

int speedVal = 150;

// ----------------- Function ------------------

float getDistance(int trig, int echo){

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 20000); // timeout

  float distance = duration * 0.0133 / 2.0; // inches

  return distance;
}

bool obstacleDetected(){

  float front = getDistance(TRIG_FRONT, ECHO_FRONT);
  float back  = getDistance(TRIG_BACK, ECHO_BACK);
  float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
  float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

  if(front > 0 && front < FRONT_BACK_LIMIT) return true;
  if(back  > 0 && back  < FRONT_BACK_LIMIT) return true;
  if(left  > 0 && left  < SIDE_LIMIT) return true;
  if(right > 0 && right < SIDE_LIMIT) return true;

  return false;
}

// ---------------- MOTOR CONTROL ----------------

void setMotor(int pin1, int pin2, int speedVal) {
  if (speedVal > 0) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else if (speedVal < 0) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

// Individual wheels
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

// Forward
void forward(){

  while(true){

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(speedVal);
    setFR(speedVal);
    setBL(speedVal);
    setBR(speedVal);

    delay(10);
  }
}

// Backward
void backward(){

  while(true){

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(-speedVal);
    setFR(-speedVal);
    setBL(-speedVal);
    setBR(-speedVal);

    delay(10);
  }
}

// STRAFE RIGHT
void right(){

  while(true){

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(speedVal);
    setFR(-speedVal);
    setBL(-speedVal);
    setBR(speedVal);

    delay(10);
  }
}

// STRAFE LEFT
void left(){

  while(true){

    if(obstacleDetected()){
      stopMotors();
      return;
    }

    setFL(-speedVal);
    setFR(speedVal);
    setBL(speedVal);
    setBR(-speedVal);

    delay(10);
  }
}

// Rotate Right
void rotateRight(){
  setFL(speedVal);
  setFR(-speedVal);
  setBL(speedVal);
  setBR(-speedVal);
}

// Rotate Left
void rotateLeft(){
  setFL(-speedVal);
  setFR(speedVal);
  setBL(-speedVal);
  setBR(speedVal);
}

// ---------------- SETUP ----------------

void setup(){

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

  stopMotors();
}

// ---------------- LOOP (TEST) ----------------

void loop(){

  backward();
  delay(2000);

}