/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf.h"

#define MIN_PULSE_WIDTH 544   // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH 2400  // the longest pulse sent to a servo

  /*
 * \brief Writes an angle to a pin where a servo is connected to (Microbit V2).
 *
 * \param ulPin       (pin number [0..31] 27 is speaker)
 * \param ulAngle     (angle [0..180])
 */
  extern void setServoAngleDegrees(uint8_t ulPin, uint8_t ulAngle);

  /*
 * \brief Gets the last written servo pulse width in microseconds. (Microbit V2).
 */
  extern uint16_t getServoMicroseconds();
  
  /*
 * \brief Writes a PWM pulse to sensor on a pin (Microbit V2).
 *
 * \param ulPin       (pin number [0..31] 27 is speaker)
 * \param ulMicroseconds ([544..2400])
 */
  extern void setServoMicroseconds(uint8_t ulPin, uint32_t ulMicroseconds);

  /*
 * \brief Writes an analog value (PWM wave) to a pin (Microbit V2 Makecode compatible).
 *
 * \param ulPin       (pin number [0..31] 27 is speaker)
 * \param ulDutyCycle (duty cycle 0-100% [0..1023])
 * \param ulFrequency (frequency [2Hz - 2.67MHz])
 */
  extern void analogWritePin(uint8_t ulPin, uint16_t ulDutyCycle, uint32_t ulFrequency);

  /*
 * \brief Directly set PWM register values (Microbit V2)
 *
 * \param pin       (pin number [0..31] 27 is speaker)
 * \param prescaler (division factor [0..7])
 * \param countertop (sets frequency [3..32767])
 * \param dutyCycle (0..100% [3..32767])
 */
  extern void setPWM(uint8_t pin, uint8_t prescaler, uint16_t countertop, uint32_t dutyCycle);

  /*
 * \brief noPwm: Disables PWM frequency for a pin.
 *        Added by Gert den Neijsel for Microbit V2
 * \param ulPin
 */
  extern void noPwm(uint8_t ulPin);

#ifdef __cplusplus
}
#endif
