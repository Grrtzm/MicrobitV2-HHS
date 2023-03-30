/*
  Copyright (c) 2016 Arduino. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#if defined(ARDUINO_ARCH_NRF5)

#include <Arduino.h>
#include <Servo.h>
#include "MicrobitV2-HHS.h"

static servo_t servos[MAX_SERVOS];  // static array of servo structures

uint8_t ServoCount = 0;  // the total number of attached servos

Servo::Servo() {
  if (ServoCount < MAX_SERVOS) {
    this->servoIndex = ServoCount++;  // assign a servo index to this instance
  } else {
    this->servoIndex = INVALID_SERVO;  // too many servos
  }
}

uint8_t Servo::attach(int pin) {
  return this->attach(pin, 0, 2500);
}

uint8_t Servo::attach(int pin, int min, int max) {
  int servo_min, servo_max;
  if (this->servoIndex < MAX_SERVOS) {
    pinMode(pin, OUTPUT);  // set servo pin to output
    servos[this->servoIndex].Pin.nbr = pin;

    if (min < servo_min) min = servo_min;
    if (max > servo_max) max = servo_max;
    this->min = min;
    this->max = max;

    servos[this->servoIndex].Pin.isActive = true;
  }
  return this->servoIndex;
}

void Servo::detach() {
  servos[this->servoIndex].Pin.isActive = false;
}

void Servo::write(int value) {
  if (value < 0)
    value = 0;
  else if (value > 180)
    value = 180;
  value = map(value, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value) {
  uint8_t channel, instance;
  uint8_t pin = servos[this->servoIndex].Pin.nbr;
  setServoMicroseconds(pin, (uint32_t)value);
}

int Servo::read()  // return the value as degrees
{
  return map(getServoMicroseconds(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 180);
}

int Servo::readMicroseconds(){
  return getServoMicroseconds();
}	

bool Servo::attached() {
  return servos[this->servoIndex].Pin.isActive;
}

#endif  // ARDUINO_ARCH_NRF52
