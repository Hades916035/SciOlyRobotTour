#include <Wire.h>

const int MPU = 0x68;

// Motor driver pins
int IN1 = 5;
int IN2 = 4;
int IN3 = 3;
int IN4 = 2;

float gyroZ_offset = 0;
float angleZ = 0;

unsigned long prevTime;

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {

  Serial.begin(9600);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();

  // Use custom I2C pins
  Wire.begin(A4, A3);   // SDA, SCL

  // Wake MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  delay(1000);

  Serial.println("Calibrating gyro... Keep robot still.");

  long total = 0;

  for (int i = 0; i < 1000; i++) {
    total += readGyroZ();
    delay(2);
  }

  gyroZ_offset = total / 1000.0;

  Serial.println("Calibration done.");

  prevTime = millis();
}

void loop() {

  unsigned long currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0;
  prevTime = currentTime;

  float gyroZ = readGyroZ();

  float degPerSec = (gyroZ - gyroZ_offset) / 131.0;

  angleZ += degPerSec * dt;

  Serial.print("Angle: ");
  Serial.println(angleZ);

  delay(20);
}

int16_t readGyroZ() {

  Wire.beginTransmission(MPU);
  Wire.write(0x47);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 2, true);

  int16_t high = Wire.read();
  int16_t low = Wire.read();

  return (high << 8) | low;
}