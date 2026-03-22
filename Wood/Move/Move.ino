const int FL1 = 2;
const int FL2 = 3;
const int FR1 = 4;
const int FR2 = 5;

const int BL1 = 8;
const int BL2 = 9;
const int BR1 = 10;
const int BR2 = 11;

const int MOVE_TIME = 150; // milliseconds (YOU WILL CALIBRATE THIS)

void setup() {
  pinMode(FL1, OUTPUT); pinMode(FL2, OUTPUT);
  pinMode(FR1, OUTPUT); pinMode(FR2, OUTPUT);
  pinMode(BL1, OUTPUT); pinMode(BL2, OUTPUT);
  pinMode(BR1, OUTPUT); pinMode(BR2, OUTPUT);
}

void stopMotors() {
  digitalWrite(FL1, LOW); digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW); digitalWrite(FR2, LOW);
  digitalWrite(BL1, LOW); digitalWrite(BL2, LOW);
  digitalWrite(BR1, LOW); digitalWrite(BR2, LOW);
}

void forward() {
  digitalWrite(FL1, HIGH); digitalWrite(FL2, LOW);
  digitalWrite(FR1, HIGH); digitalWrite(FR2, LOW);
  digitalWrite(BL1, HIGH); digitalWrite(BL2, LOW);
  digitalWrite(BR1, HIGH); digitalWrite(BR2, LOW);

  delay(MOVE_TIME);
  stopMotors();
}

void back() {
  digitalWrite(FL1, LOW); digitalWrite(FL2, HIGH);
  digitalWrite(FR1, LOW); digitalWrite(FR2, HIGH);
  digitalWrite(BL1, LOW); digitalWrite(BL2, HIGH);
  digitalWrite(BR1, LOW); digitalWrite(BR2, HIGH);

  delay(MOVE_TIME);
  stopMotors();
}

// STRAFE LEFT (mecanum pattern)
void left() {
  digitalWrite(FL1, LOW); digitalWrite(FL2, HIGH);
  digitalWrite(FR1, HIGH); digitalWrite(FR2, LOW);
  digitalWrite(BL1, HIGH); digitalWrite(BL2, LOW);
  digitalWrite(BR1, LOW); digitalWrite(BR2, HIGH);

  delay(MOVE_TIME);
  stopMotors();
}

// STRAFE RIGHT
void right() {
  digitalWrite(FL1, HIGH); digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW); digitalWrite(FR2, HIGH);
  digitalWrite(BL1, LOW); digitalWrite(BL2, HIGH);
  digitalWrite(BR1, HIGH); digitalWrite(BR2, LOW);

  delay(MOVE_TIME);
  stopMotors();
}

void loop() {
  forward();
  delay(1000000000);
  back();
  delay(1000);
  left();
  delay(1000);
  right();
  delay(2000);
}