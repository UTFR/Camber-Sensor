//file
// TODO: set time step using millis()
//       test that pointers can change the values in the function and then pass to serial
//       Test if millis works properly for dt
//       Implement offsets? and make it start from the correct angle when turning on
//       rename pitch to roll
//       include other axes?
//       tune complementary filter -- lookup how to do this
//       setup initializing gyroscope

// possible fixes: change sampling rate on the chip itself, further increase baud, reduce prints, change scale manually, change dt

#include <Wire.h>
const int MPU_ADDR=0b1101000;
const float ACCEL_SCALE_FACTOR=16384.0;
const float GYRO_SCALE_FACTOR=16.4;

long accelerometer_x, accelerometer_y, accelerometer_z;
long gyro_x, gyro_y, gyro_z;
float gForceX, gForceY, gForceZ; //processed vars
float rotX, rotY, rotZ; //processed vars

// setup filtered vars
float pitch, roll, yaw;
float testGyrPitch;
float *pitchPtr, *rollPtr, *yawPtr;
float *testGyrPitchPtr; 

// setup timing vars
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long dt = 50; // value is number of milliseconds e.g. 1000 = 1sec



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B); //gyroscope config
  Wire.write(0b00011000); //250 deg/sec
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C); //accelerometer config
  Wire.write(0b00000000); //+- 2g
  Wire.endTransmission();

  // init pointers
  pitch = 0;
  testGyrPitch = 0;
  pitchPtr = &pitch;
  rollPtr = &roll;
  yawPtr = &yaw;
  testGyrPitchPtr = &testGyrPitch;
  
  startMillis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

  float pitchAcc;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR, 6);
  while(Wire.available() < 6);
  accelerometer_x = Wire.read()<<8 | Wire.read();
  accelerometer_y = Wire.read()<<8 | Wire.read();
  accelerometer_z = Wire.read()<<8 | Wire.read();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6);
  while(Wire.available() < 6);
  gyro_x = Wire.read()<<8 | Wire.read();
  gyro_y = Wire.read()<<8 | Wire.read();
  gyro_z = Wire.read()<<8 | Wire.read();

  currentMillis = millis();
  if (currentMillis - startMillis >= dt)
  {
    *pitchPtr += (gyro_z / GYRO_SCALE_FACTOR) * dt / 1000;
    *testGyrPitchPtr += (gyro_z / GYRO_SCALE_FACTOR) * dt / 1000;
    pitchAcc = atan2f(accelerometer_x, accelerometer_y) * 180 / PI;

    *pitchPtr = *pitchPtr * 0.95 + pitchAcc * 0.05;
    startMillis = currentMillis;
  }
  
//  Serial.print("aX= ");Serial.print(gForceX);
//  Serial.print(" | aY= ");Serial.print(gForceY);
//  Serial.print(" | aZ= ");Serial.print(gForceZ);
//
//  Serial.print(" | gX = ");Serial.print(rotX);
//  Serial.print(" | gY = ");Serial.print(rotY);
//  Serial.print(" | gZ = ");Serial.print(rotZ);

//  Serial.print(testGyrPitch); // below by about 40 deg
//  Serial.print(" ");
//  Serial.print(pitchAcc);
//  Serial.print(" ");
//  Serial.println(pitch);

  Serial.print(pitchAcc); // still below by aboutt 40 deg... so clearly did not help to change print order... try baud rate now
  Serial.print(" ");
  Serial.print(pitch);
  Serial.print(" ");
  Serial.println(testGyrPitch);
  
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
