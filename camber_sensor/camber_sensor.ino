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
#include <global.h>
#include <Canbus.h>
#include <mcp2515_defs.h>
#include <mcp2515.h>
#include <defaults.h>

const int MPU_ADDR=0b1101000;
const float ACCEL_SCALE_FACTOR=16384.0;
const float GYRO_SCALE_FACTOR=16.4;

long accelerometer_x, accelerometer_y, accelerometer_z;
long gyro_x, gyro_y, gyro_z;
float gForceX, gForceY, gForceZ; //processed vars
float rotX, rotY, rotZ; //processed vars

// setup filtered vars
float pitch, roll, yaw;
int canPitch; // temp var Pitch changed to split into digits
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
  Serial.println("CAN Write - Testing transmission of CAN Bus messages");
  delay(1000);

  if(Canbus.init(CANSPEED_500))  //Initialise MCP2515 CAN controller at the specified speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
    
  delay(1000);
  
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

  tCAN message;

  message.id = 0x631; //formatted in HEX
  message.header.rtr = 0;
  message.header.length = 8; //formatted in DEC

  if (abs(pitch) < 10.0) // change negs
  {
    if (pitch < 0.0)
    {
      canPitch = int(pitch * 100.0 * (-1));
      message.data[0] = 1;
      message.data[1] = 0;
      message.data[2] = 0;
      message.data[3] = 0; //formatted in HEX
      message.data[4] = 0;
      message.data[5] = (canPitch / 100) % 10;
      message.data[6] = (canPitch / 10) % 10;
      message.data[7] = canPitch % 10;
    }
    else
    {
      canPitch = int(pitch * 100.0);
      message.data[0] = 0;
      message.data[1] = 0;
      message.data[2] = 0;
      message.data[3] = 0; //formatted in HEX
      message.data[4] = 0;
      message.data[5] = (canPitch / 100) % 10;
      message.data[6] = (canPitch / 10) % 10;
      message.data[7] = canPitch % 10;
    }
    
  }

  else if (abs(pitch) < 100.0) // change negs
  {
    if (pitch < 0.0)
    {
      canPitch = int(pitch * 100.0 * (-1));
      message.data[0] = 1;
      message.data[1] = 0;
      message.data[2] = 0;
      message.data[3] = 0; //formatted in HEX???????????????????????????????????????????????????
      message.data[4] = (canPitch / 1000) % 10;
      message.data[5] = (canPitch / 100) % 10;
      message.data[6] = (canPitch / 10) % 10;
      message.data[7] = canPitch % 10;
    }
    else
    {
      canPitch = int(pitch * 100.0);
      message.data[0] = 0;
      message.data[1] = 0;
      message.data[2] = 0;
      message.data[3] = 0; //formatted in HEX??????????????????????????????????????????????????? CHECK OVERFLOW, MIGHT NEED ANOTHER VAR FOR BIGGER DIGITS >>> CHECK IF NEEDS TO BE HEX OUTPUT
      message.data[4] = (canPitch / 1000) % 10;
      message.data[5] = (canPitch / 100) % 10;
      message.data[6] = (canPitch / 10) % 10;
      message.data[7] = canPitch % 10;
    }
  }
    
  else
  {
    message.data[0] = 1;
    message.data[1] = 1;
    message.data[2] = 1;
    message.data[3] = 1; //formatted in HEX
    message.data[4] = 1;
    message.data[5] = 1;
    message.data[6] = 1;
    message.data[7] = 1;
  }

mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
mcp2515_send_message(&message);

delay(5);
  
}
