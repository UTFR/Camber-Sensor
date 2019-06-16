//file

#include <Wire.h>
const int MPU_ADDR=0b1101000;

long accelerometer_x, accelerometer_y, accelerometer_z;
long gyro_x, gyro_y, gyro_z;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00000000);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);
  Wire.write(0b00000000);
  Wire.endTransmission();
  

}

void loop() {
  // put your main code here, to run repeatedly:
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
  
  Serial.print("aX= ");Serial.print(accelerometer_x);
  Serial.print(" | aY= ");Serial.print(accelerometer_y);
  Serial.print(" | aZ= ");Serial.print(accelerometer_z);

  Serial.print(" | gX = ");Serial.print(gyro_x);
  Serial.print(" | gY = ");Serial.print(gyro_y);
  Serial.print(" | gZ = ");Serial.print(gyro_z);
  Serial.println();

  //delay(100);
}
