#include <Wire.h>

#define IN1 5
#define IN2 4
#define IN3 3
#define IN4 2

#define ENA 9
#define ENB 10

#define BUTTON 7

const int MPU = 0x68;

// gyro
float gyroAngle = 0;
float gyroBias = 0;
unsigned long prevTime;

// PID
float Kp = 2.0;
float Ki = 0.0;
float Kd = 0.4;

float error;
float integral = 0;
float derivative;
float prevError = 0;

int baseSpeed = 150;

// ramping
int rampStep = 5;
float msPerDegree = 6.5;

// state
bool running = false;
bool lastButton = HIGH;

// ---------------- MOTOR CONTROL ----------------

void setMotor(int leftSpeed, int rightSpeed){

  leftSpeed = constrain(leftSpeed,-255,255);
  rightSpeed = constrain(rightSpeed,-255,255);

  // LEFT MOTOR (reversed)
  if(leftSpeed >= 0){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  } else {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }

  // RIGHT MOTOR
  if(rightSpeed >= 0){
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  } else {
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  }

  analogWrite(ENA,abs(leftSpeed));
  analogWrite(ENB,abs(rightSpeed));
}

void stopMotors(){
  analogWrite(ENA,0);
  analogWrite(ENB,0);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}

// ---------------- BUTTON ----------------

void checkButton(){

  bool state = digitalRead(BUTTON);

  if(state == LOW && lastButton == HIGH){

    delay(40);

    running = !running;

    if(!running){
      stopMotors();
    }
  }

  lastButton = state;
}

// ---------------- GYRO ----------------

void calibrateGyro(){

  long total = 0;

  for(int i=0;i<1000;i++){

    Wire.beginTransmission(MPU);
    Wire.write(0x47);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU,2,true);

    int16_t gz = Wire.read()<<8 | Wire.read();

    total += gz;

    delay(2);
  }

  gyroBias = total / 1000.0;
}

void updateGyro(){

  Wire.beginTransmission(MPU);
  Wire.write(0x47);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU,2,true);

  int16_t gz = Wire.read()<<8 | Wire.read();

  float gyroZ = (gz - gyroBias) / 131.0;

  unsigned long now = millis();
  float dt = (now - prevTime)/1000.0;
  prevTime = now;

  gyroAngle += gyroZ * dt;
}

void resetGyro(){
  gyroAngle = 0;
}

// ---------------- PID ----------------

float computePID(float target){

  error = target - gyroAngle;

  integral += error;
  derivative = error - prevError;

  prevError = error;

  return Kp*error + Ki*integral + Kd*derivative;
}

// ---------------- RAMP ----------------

int rampSpeed(int current,int target){

  if(current < target)
    current += rampStep;

  else if(current > target)
    current -= rampStep;

  return current;
}

// ---------------- FORWARD ----------------

void fwd(float inches){

  resetGyro();

  int leftSpeed = 0;
  int rightSpeed = 0;
  
  unsigned long start = millis();
  unsigned long duration = inches * 340;

  while(millis()-start < duration && running){

    checkButton();
    updateGyro();

    float correction = computePID(0);

    int targetLeft = baseSpeed + correction;
    int targetRight = baseSpeed - correction;

    leftSpeed = rampSpeed(leftSpeed,targetLeft);
    rightSpeed = rampSpeed(rightSpeed,targetRight);

    setMotor(leftSpeed,rightSpeed);

    delay(10);
  }

  stopMotors();

  delay(300);

}

// ---------------- BACK ----------------

void back(float inches){

  resetGyro();

  int leftSpeed = 0;
  int rightSpeed = 0;

  unsigned long start = millis();
  unsigned long duration = inches * 340;

  // 400 went 2 + 5/16 inches over
  // 400 : 12 + 2 + 5/16 then x : 12
  // x = 335

  while(millis()-start < duration && running){

    checkButton();
    updateGyro();

    float correction = computePID(0);

    int targetLeft = -baseSpeed + correction;
    int targetRight = -baseSpeed - correction;

    leftSpeed = rampSpeed(leftSpeed,targetLeft);
    rightSpeed = rampSpeed(rightSpeed,targetRight);

    setMotor(leftSpeed,rightSpeed);

    delay(10);
  }

  stopMotors();

  delay(300);

}

// ---------------- TURN ----------------

void turn(float degrees){

  int turnSpeed = 0;

  int direction = (degrees > 0) ? 1 : -1;

  int target = direction * 150;

  turnSpeed = rampSpeed(turnSpeed, target);

  setMotor(turnSpeed, -turnSpeed);

  delay(2000);

  stopMotors();

  delay(300);

}

// ---------------- SETUP ----------------

void setup(){

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  pinMode(BUTTON,INPUT_PULLUP);

  Serial.begin(9600);

  Wire.begin();

  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  delay(1000);

  calibrateGyro();

  prevTime = millis();
}

// ---------------- LOOP ----------------

void loop(){

  checkButton();

  if(running){

    delay(200);

    fwd(12);
    turn(-90);
    fwd(12);
    turn(-90);
    back(12);

    running = false;
  }
}