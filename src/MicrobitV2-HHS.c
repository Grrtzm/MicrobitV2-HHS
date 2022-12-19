/*
  Copyright (c) 2014 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  This version: Gert den Neijsel, december 2022, based on Adafruit/Sandeeps "wiring_digital.h" for nRF52833

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

#include <Arduino.h>
#include "MicrobitV2-HHS.h"

#if defined(NRF52_SERIES)

#ifdef __cplusplus
extern "C" {
#endif

  static NRF_PWM_Type* pwms[PWM_COUNT] = {
    NRF_PWM0,
    NRF_PWM1,
    NRF_PWM2,
#if PWM_COUNT > 3
    NRF_PWM3
#endif
  };

  static uint32_t pwmChannelPins[PWM_COUNT] = {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
#if PWM_COUNT > 3
    0xFFFFFFFF,
#endif
  };
  static uint16_t pwmChannelSequence[PWM_COUNT];

  void setServoAngleDegrees(uint8_t ulPin, uint8_t ulAngle) {
    if (ulAngle < 0)
      ulAngle = 0;
    else if (ulAngle > 180)
      ulAngle = 180;

    uint32_t ulMicroseconds = ((ulAngle - 0) * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / (180 - 0) + MIN_PULSE_WIDTH);  // map function

    setServoMicroseconds(ulPin, ulMicroseconds);
  }

  uint16_t getServoMicroseconds() {
		return pwmChannelSequence[0] & 0x7FFF;
  }
  
  void setServoMicroseconds(uint8_t ulPin, uint32_t ulMicroseconds) {
    if (ulMicroseconds < MIN_PULSE_WIDTH) ulMicroseconds = MIN_PULSE_WIDTH;
    else if (ulMicroseconds > MAX_PULSE_WIDTH) ulMicroseconds = MAX_PULSE_WIDTH;

    // 16 uSec per step countertop @ 125kHz
    uint32_t dutyCycle = ulMicroseconds / 16;
    setPWM(ulPin, PWM_PRESCALER_PRESCALER_DIV_128, 1250, dutyCycle);  // 20ms - 50Hz
  }

  void analogWritePin(uint8_t ulPin, uint16_t ulDutyCycle, uint32_t ulFrequency) {
    if (ulPin >= PINS_COUNT) {
      return;
    }
    if (ulFrequency > 1 && ulFrequency <= 2666666) {  // minimum frequency 1,9 Hz @ prescaler DIV_128 && freqval = 32767
      // Selecting a high PRESCALER_DIV gives a more fine-grained control over duty cycle [3 .. 32767 steps].
      // The higher the frequency, the lower the control over duty-cycle. At 80 kHz you can control duty-cycle in 1% steps.
      // Beyond 80 kHz, this control gets less fine-grained

      uint8_t prescaler = 0;
      if (ulFrequency > 244) prescaler = PWM_PRESCALER_PRESCALER_DIV_1;        // 0 -> 16MHz,	freq = 250 Hz .. 8 MHz
      else if (ulFrequency > 121) prescaler = PWM_PRESCALER_PRESCALER_DIV_2;  // 1 -> 8MHz,		freq = 125 Hz .. 4 MHz
      else if (ulFrequency > 61) prescaler = PWM_PRESCALER_PRESCALER_DIV_4;   // 2 -> 4MHz,		freq = 62 Hz .. 2 MHz
      else if (ulFrequency > 30) prescaler = PWM_PRESCALER_PRESCALER_DIV_8;   // 3 -> 2MHz,		freq = 31 Hz .. 1 MHz
      else if (ulFrequency > 15) prescaler = PWM_PRESCALER_PRESCALER_DIV_16;  // 4 -> 1MHz,		freq = 16 Hz .. 500 kHz
      else if (ulFrequency > 7) prescaler = PWM_PRESCALER_PRESCALER_DIV_32;   // 5 -> 500kHz,	freq = 8 Hz .. 250 kHz
      else if (ulFrequency > 3) prescaler = PWM_PRESCALER_PRESCALER_DIV_64;   // 6 -> 250kHz,	freq = 4 Hz .. 125 kHz
      else if (ulFrequency > 1) prescaler = PWM_PRESCALER_PRESCALER_DIV_128;  // 7 -> 125kHz,	freq = 2 Hz .. 62500 Hz
      else return;

      uint32_t countertop = (double)(1.0f / ulFrequency) / (double)(1.0f / (8000000.0f / pow(2, prescaler)));
      uint32_t dutyCycle = ((double)countertop / 1024.0f) * ulDutyCycle;

      setPWM(ulPin, prescaler, countertop, dutyCycle);
    }
  }

  void setPWM(uint8_t pin, uint8_t prescaler, uint16_t countertop, uint32_t dutyCycle) {
    if (pin >= PINS_COUNT || prescaler > 7 || countertop < 3 || countertop > 32767 || dutyCycle < 3 || dutyCycle > 32767) {
      return;
    }
    pin = g_ADigitalPinMap[pin];
    for (int i = 0; i < PWM_COUNT; i++) {
      if (pwmChannelPins[i] == 0xFFFFFFFF || pwmChannelPins[i] == pin) {
        pwmChannelPins[i] = pin;
        pwmChannelSequence[i] = dutyCycle | bit(15);

        NRF_PWM_Type* pwm = pwms[i];
        pwm->PSEL.OUT[0] = pin;
        pwm->PSEL.OUT[1] = pin;
        pwm->PSEL.OUT[2] = pin;
        pwm->PSEL.OUT[3] = pin;
        pwm->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
        pwm->PRESCALER = prescaler;
        pwm->MODE = (PWM_MODE_UPDOWN_UpAndDown << PWM_MODE_UPDOWN_Pos);  //PWM_MODE_UPDOWN_Up;
        pwm->COUNTERTOP = (countertop << PWM_COUNTERTOP_COUNTERTOP_Pos);
        pwm->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);  //0;
        pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
        //pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
        pwm->SEQ[0].PTR = (uint32_t)&pwmChannelSequence[i];
        pwm->SEQ[0].CNT = 1;
        pwm->SEQ[0].REFRESH = 0;  //1;
        pwm->SEQ[0].ENDDELAY = 0;
        pwm->TASKS_SEQSTART[0] = 0x1UL;

        break;
      }
    }
  }

  void noPwm(uint8_t ulPin) {
    if (ulPin >= PINS_COUNT) {
      return;
    }
    ulPin = g_ADigitalPinMap[ulPin];

    for (int i = 0; i < PWM_COUNT; i++) {
      if (pwmChannelPins[i] == 0xFFFFFFFF || pwmChannelPins[i] == ulPin) {
        pwmChannelPins[i] = ulPin;
        pwmChannelSequence[i] = 0x80 | bit(15);

        NRF_PWM_Type* pwm = pwms[i];

        pwm->PSEL.OUT[0] = ulPin;
        pwm->PSEL.OUT[1] = ulPin;
        pwm->PSEL.OUT[2] = ulPin;
        pwm->PSEL.OUT[3] = ulPin;
        //pwm->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
        pwm->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);

        pwm->LOOP = 0;
        pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
        //pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
        pwm->SEQ[0].PTR = (uint32_t)&pwmChannelSequence[i];
        pwm->SEQ[0].CNT = 1;
        pwm->SEQ[0].REFRESH = 1;
        pwm->SEQ[0].ENDDELAY = 0;
        pwm->TASKS_SEQSTART[0] = 0x1UL;

        break;
      }
    }
  }

#ifdef __cplusplus
}
#endif

#endif
