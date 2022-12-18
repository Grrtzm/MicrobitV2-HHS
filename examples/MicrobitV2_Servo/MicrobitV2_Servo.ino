/* 
 *  Servo demo on a Microbit Version 2
 *  using the MicrobitV2-HHS library
 *  Gert den Neijsel, Haagse Hogeschool, dec-2022
 *  This example code is in the public domain.
 *  Push the buttons on the micro:bit to change the angle of the servo.
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int angle = 90;  // variable to read the value from the analog pin
int currentValue = 0, lastValue = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(0);  // attaches the servo on pin 0 to the servo object
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
}

void loop() {
  if (!digitalRead(PIN_BUTTON_A)) {
    if (angle < 180) {
      angle++;
      Serial.println(angle);
    }
  }
  if (!digitalRead(PIN_BUTTON_B)) {
    if (angle > 0) {
      angle--;
      Serial.println(angle);
    }
  }
  currentValue = angle;
  if (currentValue != lastValue) {
    myservo.write(angle);  // sets the servo position according to the scaled value
  }
  lastValue = currentValue;
}