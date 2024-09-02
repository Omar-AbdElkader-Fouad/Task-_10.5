#include <Wire.h>

const int MPU6050_ADDR = 0x68;
const int PWR_MGMT_1 = 0x6B;
const int GYRO_XOUT_H = 0x43;
const int GYRO_CONFIG = 0x1B;

float yaw = 0;
long previousTime = 0;
float gyroYawOffset = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0x00);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(GYRO_CONFIG);
  Wire.write(0x00);
  Wire.endTransmission(true);

  calibrateGyro();
}

void loop() {
  long currentTime = millis();
  float dt = (currentTime - previousTime) / 1000.0;

  int16_t gyroYaw = readRawData(GYRO_XOUT_H);
  float gyroYawRate = (gyroYaw - gyroYawOffset) / 131.0;

  yaw += gyroYawRate * dt;
  previousTime = currentTime;

  Serial.print("Yaw: ");
  Serial.println(yaw);
  delay(100);
}

int16_t readRawData(int addr) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 2, true);
  int16_t highByte = Wire.read();
  int16_t lowByte = Wire.read();
  return (highByte << 8) | lowByte;
}

void calibrateGyro() {
  long sum = 0;
  const int numReadings = 1000;

  for (int i = 0; i < numReadings; i++) {
    sum += readRawData(GYRO_XOUT_H);
    delay(3);
  }

  gyroYawOffset = sum / numReadings;
}