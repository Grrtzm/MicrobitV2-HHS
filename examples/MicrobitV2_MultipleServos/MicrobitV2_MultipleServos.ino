/* 
 *  Demo for 4 servos on a Microbit Version 2
 *  using the MicrobitV2-HHS library
 *  Gert den Neijsel, Haagse Hogeschool, dec-2022
 *  This example code is in the public domain.
*/

#include <Servo.h>  // Include the Servo library

Servo servo1;  // Create a Servo object for each servo
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  servo1.attach(0);  // Attach each servo to a different pin
  servo2.attach(1);
  servo3.attach(2);
  servo4.attach(3);
}

void loop() {
  servo1.write(0);  // Set the servos to different positions
  delay(200);
  servo2.write(45);
  delay(200);
  servo3.write(90);
  delay(200);
  servo4.write(135);
  delay(200);

  servo1.write(180);  // Set the servos to different positions
  delay(200);
  servo2.write(135);
  delay(200);
  servo3.write(160);
  delay(200);
  servo4.write(45);
  delay(200);
}
