/*
 * Single Read
 *
 * simple example to test the HMC5983 sensor from Honeywell
 *
 * (c) 2014 David Cuartielles, Arduino LLC
 */
 
#include <HMC5983.h>
#include <Wire.h>

HMC5983 compass;

void setup () {
  Serial.begin(9600);
  compass.begin(); // use "true" if you need some debug information
}

void loop() {
  float c = -999;
  c = compass.read();
  if (c == -999) {
    Serial.println("Reading error, discarded");
  } else {
    Serial.println(c);
  }
  delay(500);
}
