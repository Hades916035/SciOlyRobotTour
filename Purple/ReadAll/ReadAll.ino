// ---------------- ULTRASONIC PINS ----------------

#define TRIG_FRONT 13
#define ECHO_FRONT 12

#define TRIG_LEFT A2
#define ECHO_LEFT A3

#define TRIG_BACK A1
#define ECHO_BACK A0

#define TRIG_RIGHT A4
#define ECHO_RIGHT A5

// ---------------- FUNCTION ----------------

float getDistance(int trig, int echo){

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 25000); // timeout

  if(duration == 0) return -1; // no reading

  float distance = duration * 0.0133 / 2.0; // inches

  return distance;
}

// ---------------- SETUP ----------------

void setup(){

  Serial.begin(9600);

  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  pinMode(TRIG_BACK, OUTPUT);
  pinMode(ECHO_BACK, INPUT);

  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  Serial.println("Ultrasonic Sensor Test Starting...");
}

// ---------------- LOOP ----------------

void loop(){

  float front = getDistance(TRIG_FRONT, ECHO_FRONT);
  float left  = getDistance(TRIG_LEFT, ECHO_LEFT);
  float back  = getDistance(TRIG_BACK, ECHO_BACK);
  float right = getDistance(TRIG_RIGHT, ECHO_RIGHT);

  Serial.print("Front: ");
  Serial.print(front);
  Serial.print(" in | ");

  Serial.print("Left: ");
  Serial.print(left);
  Serial.print(" in | ");

  Serial.print("Back: ");
  Serial.print(back);
  Serial.print(" in | ");

  Serial.print("Right: ");
  Serial.print(right);
  Serial.println(" in");

  delay(200);
}