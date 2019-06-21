//file
// TODO: set time step using millis()
//       test that pointers can change the values in the function and then pass to serial
//       Test if millis works properly for dt
//       Implement offsets? and make it start from the correct angle when turning on
//       Low pass filter data first
//       implement bullshit filter for both
//       rename pitch to roll
//       include other axes?

#include <Wire.h>
const int MPU_ADDR=0b1101000;
const float ACCEL_SCALE_FACTOR=16384.0;
const float GYRO_SCALE_FACTOR=131.0;

long accelerometer_x, accelerometer_y, accelerometer_z;
long gyro_x, gyro_y, gyro_z;
float gForceX, gForceY, gForceZ; //processed vars
float rotX, rotY, rotZ; //processed vars

// setup filtered vars
float pitch, roll, yaw;
float *pitchPtr, *rollPtr, *yawPtr; 

// setup timing vars
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long dt = 10; // value is number of milliseconds e.g. 1000 = 1sec



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B); //gyroscope config
  Wire.write(0x00000000); //250 deg/sec
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C); //accelerometer config
  Wire.write(0b00000000); //+- 2g
  Wire.endTransmission();

  // init pointers
  pitch = 0;
  pitchPtr = &pitch;
  rollPtr = &roll;
  yawPtr = &yaw;
  
  startMillis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

  float pitchAcc;
  float testGyrPitch;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR, 6);
  while(Wire.available() < 6);
  accelerometer_x = Wire.read()<<8 | Wire.read();
  accelerometer_y = Wire.read()<<8 | Wire.read();
  accelerometer_z = Wire.read()<<8 | Wire.read();
  processAccelData();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6);
  while(Wire.available() < 6);
  gyro_x = Wire.read()<<8 | Wire.read();
  gyro_y = Wire.read()<<8 | Wire.read();
  gyro_z = Wire.read()<<8 | Wire.read();
  processGyroData();

  currentMillis = millis();
  if (currentMillis - startMillis >= dt)
  {
    *pitchPtr += (gyro_z / GYRO_SCALE_FACTOR) * dt / 1000;
    pitchAcc = atan2f(accelerometer_x, accelerometer_y) * 180 / PI;

    *pitchPtr = *pitchPtr * 0.98 + pitchAcc * 0.02;
    startMillis = currentMillis;
  }
  
//  Serial.print("aX= ");Serial.print(gForceX);
//  Serial.print(" | aY= ");Serial.print(gForceY);
//  Serial.print(" | aZ= ");Serial.print(gForceZ);
//
//  Serial.print(" | gX = ");Serial.print(rotX);
//  Serial.print(" | gY = ");Serial.print(rotY);
//  Serial.print(" | gZ = ");Serial.print(rotZ);
//  Serial.print(pitch);
  Serial.print(*pitchPtr);
  //Serial.print(pitchAcc);
  //Serial.print(testGyrPitch);
  Serial.println();

  
}

void processAccelData() {
  gForceX = accelerometer_x / ACCEL_SCALE_FACTOR;
  gForceY = accelerometer_y / ACCEL_SCALE_FACTOR;
  gForceZ = accelerometer_z / ACCEL_SCALE_FACTOR;
}

void processGyroData() {
  rotX = gyro_x / GYRO_SCALE_FACTOR;
  rotY = gyro_y / GYRO_SCALE_FACTOR;
  rotZ = gyro_z / GYRO_SCALE_FACTOR;
}

// init the variables you need and then pointers to those variables
// then pass the address of the variables to the function
