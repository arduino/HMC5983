/*
 * HMC5983.cpp - library class
 *
 * simple library to test the HMC5983 sensor from Honeywell
 *
 * (c) 2014 David Cuartielles, Arduino Verkstad AB
 */

#include "HMC5983.h"
#include <Wire.h>

void HMC5983::begin(int D){
	DEBUG = D;
	Wire.begin();
}

/*
From datasheet for the HMC5983
Below is an example of a (power-on) initialization process for “continuous-measurement mode” via I2C interface:
1. Write CRA (00) – send 0x3C 0x00 0x70 (8-average, 15 Hz default or any other rate, normal measurement)
2. Write CRB (01) – send 0x3C 0x01 0xA0 (Gain=5, or any other desired gain)
3. For each measurement query:
	Write Mode (02) – send 0x3C 0x02 0x01 (Single-measurement mode)
	Wait 6 ms or monitor status register or DRDY hardware interrupt pin
	Send 0x3D 0x06 (Read all 6 bytes. If gain is changed then this data set is using previous gain)
	Convert three 16-bit 2’s compliment hex values to decimal values and assign to X, Z, Y, respectively.
(Self addition:)
4. Convert the magnetic information into a compass value

REGARDING THE CALCULATION OF THE ACTUAL HEADING VALUE

From AN-203 http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
The magnetic compass heading can be determined (in degrees) from the magnetometer's x and y readings by using the
following set of equations:
	Direction (y>0) = 90 - [arcTAN(x/y)]*180/PI
	Direction (y<0) = 270 - [arcTAN(x/y)]*180/PI
	Direction (y=0, x<0) = 180.0
	Direction (y=0, x>0) = 0.0

MISSING : EARTH DECLINATION ANGLE

In other words, we are not making any compensation for the earth's north pole location vs the magnetic measurement

*/
float HMC5983::read(){
  // start transmission
  //Wire.beginTransmission(HMC5983_ADDRESS);

  // step 1
  //Wire.beginTransmission(HMC5983_ADDRESS);
  //Wire.write(HMC5983_WRITE);
  //Wire.write(0x00);
  //Wire.write(0x70);
  //Wire.endTransmission();

  // step 2
  //Wire.beginTransmission(HMC5983_ADDRESS);
  //Wire.write(HMC5983_WRITE);
  //Wire.write(0x01);
  //Wire.write(0xA0);
  //Wire.endTransmission();

  // step 3
  Wire.beginTransmission(HMC5983_ADDRESS);
  //Wire.write(HMC5983_WRITE);
  Wire.write(0x02);
  Wire.write(0x01);
  Wire.endTransmission();
	
  // time delays required by HMC5983 upon receipt of the command
  // Get Data. Compensate and Calculate New Heading : 6ms
  delay(6);

  // step 3: get the six data bytes, MSB and LSB for X, Z, Y (in that order)
  //Wire.beginTransmission(HMC5983_ADDRESS);
  //Wire.write(HMC5983_READ);
  //Wire.write(0x06);
  //Wire.endTransmission();

  Wire.requestFrom(HMC5983_ADDRESS, 6); //get the six data bytes, MSB and LSB for X, Z, Y (in that order)

  long t = 0;
  while (Wire.available() < 6 && t < 5) {
	t++;
        delay(1);
  };

  if (t >= 5) return -999;

  //"The heading output data will be the value in tenths of degrees
  //from zero to 3599 and provided in binary format over the two bytes."
  byte X_MSB = Wire.read();
  byte X_LSB = Wire.read();
  byte Z_MSB = Wire.read();
  byte Z_LSB = Wire.read();
  byte Y_MSB = Wire.read();
  byte Y_LSB = Wire.read();

  if (DEBUG) {
	Serial.print("X MSB: "); Serial.print(X_MSB);
	Serial.print(" - X LSB: "); Serial.println(X_LSB);
	Serial.print("Y MSB: "); Serial.print(Y_MSB);
	Serial.print(" - Y LSB: "); Serial.println(Y_LSB);
	Serial.print("Z MSB: "); Serial.print(Z_MSB);
	Serial.print(" - Z LSB: "); Serial.println(Z_LSB);
  }

  // calculate the heading values for all the three sensors
  // range is 0xF800 to 0x07FF or -2048 to 2047
  float HX = (X_MSB << 8) + X_LSB;
  float HZ = (Z_MSB << 8) + Z_LSB;
  float HY = (Y_MSB << 8) + Y_LSB;

  // convert the numbers to fit the 
  if (HX > 0x07FF) HX = 0xFFFF - HX;
  if (HZ > 0x07FF) HZ = 0xFFFF - HZ;
  if (HY > 0x07FF) HY = 0xFFFF - HY;
  
  // declare the heading variable we'll be returning
  float H = 0;

  if (HY == 0 && HX > 0) H = 180.0;
  if (HY == 0 && HX <= 0) H = 0.0;
  if (HY > 0) H = 90 - atan(HX/HY) * 180 / PI;
  if (HY < 0) H = 270 - atan(HX/HY) * 180 / PI;
  
  return H;
}


