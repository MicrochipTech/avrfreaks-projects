/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define NUM_DIGITAL_PINS            31
#define NUM_ANALOG_INPUTS           8
#define analogInputToDigitalPin(p)  ((p < NUM_ANALOG_INPUTS) ? (p) + 24 : -1)

#define digitalPinHasPWM(p)         ((p) == 4 || (p) == 5 || (p) == 6 || (p) == 4 || (p) == 11)

const static uint8_t SS   = 12;
const static uint8_t MOSI = 13;
const static uint8_t MISO = 14;
const static uint8_t SCK  = 15;

const static uint8_t SDA = 17;
const static uint8_t SCL = 16;
const static uint8_t LED_BUILTIN = 1;

const static uint8_t A0 = 24;
const static uint8_t A1 = 25;
const static uint8_t A2 = 26;
const static uint8_t A3 = 27;
const static uint8_t A4 = 28;
const static uint8_t A5 = 29;
const static uint8_t A6 = 30;
const static uint8_t A7 = 31;

#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 31) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))
#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))
#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))

#define TIMER0 TIMER0A

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.
//                 ATMEL ATMEGA32 / ARDUINO
//                      +---\/---+
//           (D 8) PB0 1|        |40 PA0 (AI 0 / D24)
//           (D 9) PB1 2|        |39 PA1 (AI 1 / D25)
//      INT2 (D10) PB2 3|        |38 PA2 (AI 2 / D26)
//       PWM (D11) PB3 4|        |37 PA3 (AI 3 / D27)
//        SS (D12) PB4 5|        |36 PA4 (AI 4 / D28)
//      MOSI (D13) PB5 6|        |35 PA5 (AI 5 / D29)
//      MISO (D14) PB6 7|        |34 PA6 (AI 6 / D30)
//       SCK (D15) PB7 8|        |33 PA7 (AI 7 / D31)
//                 RST 9|        |32 AREF
//                VCC 10|        |31 GND
//                GND 11|        |30 AVCC
//              XTAL2 12|        |29 PC7 (D 23)
//              XTAL1 13|        |28 PC6 (D 22)
//       RX (D 0 )PD0 14|        |27 PC5 (D 21) TDI
//       TX (D 1) PD1 15|        |26 PC4 (D 20) TDO
//     INT0 (D 2) PD2 16|        |25 PC3 (D 19) TMS
//     INT1 (D 3) PD3 17|        |24 PC2 (D 18) TCK
//      PWM (D 4) PD4 18|        |23 PC1 (D 17) SDA
//      PWM (D 5) PD5 19|        |22 PC0 (D 16) SCL
//      PWM (D 6) PD6 20|        |21 PD7 (D 7)  PWM
//                      +--------+
//
// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &DDRA,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PINA,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PD, /* 0 ** PORTD PD0-7*/
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PB, /* 8 ** PORTB PB0-7*/
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PC, /* 16 ** PORTC PC0-7*/
	PC,
	PC,
	PC,
	PC,
	PC,
	PC,
	PC,
	PA, /* 24 ** PORTA PA0-7*/
	PA,
	PA,
	PA,
	PA,
	PA,
	PA,
	PA /* 31 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(0), /* 0, port D */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 8, port B */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 16, port C */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 24, port A */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7)  /* 31 */
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	NOT_ON_TIMER, /* 0 - port D */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER1B,
	TIMER1A,
	NOT_ON_TIMER,
	TIMER2,
	NOT_ON_TIMER, /* 8 - port B */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER0,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 16 - port C */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 24 - port A */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER
};

#endif

#endif