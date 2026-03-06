#include <Wire.h>

const int MPU_ADDR = 0x68;

// Motor pins
const int FL1 = 2;
const int FL2 = 3;
const int FR1 = 4;
const int FR2 = 5;

const int BL1 = 8;
const int BL2 = 9;
const int BR1 = 10;
const int BR2 = 11;

float gyroZ_offset = 0;
float angleZ = 0;

unsigned long previousTime;

void stopMotors() {
  digitalWrite(FL1, LOW); digitalWrite(FL2, LOW);
  digitalWrite(FR1, LOW); digitalWrite(FR2, LOW);
  digitalWrite(BL1, LOW); digitalWrite(BL2, LOW);
  digitalWrite(BR1, LOW); digitalWrite(BR2, LOW);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Set motor pins
  pinMode(FL1, OUTPUT); pinMode(FL2, OUTPUT);
  pinMode(FR1, OUTPUT); pinMode(FR2, OUTPUT);
  pinMode(BL1, OUTPUT); pinMode(BL2, OUTPUT);
  pinMode(BR1, OUTPUT); pinMode(BR2, OUTPUT);

  // 🚨 STOP ALL MOTORS IMMEDIATELY
  stopMotors();

  // Wake up MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  // Set gyro full scale to ±250 deg/s
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  delay(1000);

  Serial.println("Calibrating... Keep robot still.");

  long total = 0;
  for (int i = 0; i < 1000; i++) {
    total += readGyroZRaw();
    delay(2);
  }

  gyroZ_offset = total / 1000.0;

  Serial.println("Calibration complete.");
  previousTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  float gyroZ = readGyroZRaw();
  float angularVelocity = (gyroZ - gyroZ_offset) / 131.0;

  angleZ += angularVelocity * deltaTime;

  Serial.print("Rotation (deg): ");
  Serial.println(angleZ);

  delay(20);
}

int16_t readGyroZRaw() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);

  int16_t high = Wire.read();
  int16_t low = Wire.read();
  return (high << 8) | low;
}