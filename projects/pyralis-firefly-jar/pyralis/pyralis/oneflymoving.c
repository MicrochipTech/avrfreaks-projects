/* Name: oneflymoving.c
 * Author: Kelsey Jordahl
 * Copyright: Kelsey Jordahl 2009
 * License: GPLv3
 * Time-stamp: <Sat Dec 26 09:11:10 EST 2009> 

Simulate fireflies in a jar
Uses the same hardware circuit as the Jar-of-Fireflies Instructable
   http://www.instructables.com/id/EITCRQFB2JEWP86TYK/
with simpler software logic.

This version flashes six LEDs in sequence, about 6 seconds apart to
simulate a single male firefly (species Photinus pyralis, the common
Eastern firefly) moving around the jar.  Pulse shape and firefly
behavior modeled after Case [2004].

Case, J., Flight studies on photic communication by the firefly
     Photinus pyralis, Integrative and Comparative Biology, 44(3), 250,
     doi:10.1093/icb/44.3.250, 2004.

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.  A copy of the GPL
    version 3 license can be found in the file COPYING or at
    <http://www.gnu.org/licenses/>.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

 */

#include <avr/io.h>     
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define WAIT 250		/* 100 ms delay */
#define IN1 (1 << 1)		/* Pin  1 */
#define IN2 (1 << 4)		/* Pin  4 */
#define OUT1 (1 << 3)		/* Pin  3 */
#define OUT2 (1 << 2)		/* Pin  2 */
#define OUT3 (1 << 0)		/* Pin  0 */
#define IPINS (IN1 | IN2)
#define OPINS (OUT1 | OUT2 | OUT3)
#define ALL_ON 0x1F & ~OPINS	/* input pins high; output pins low */
#define ALL_OFF 0x1F & ~IPINS	/* input pins low; output pins high */

short int inpin[2] = {IN1, IN2};
short int outpin[3] = {OUT1, OUT2, OUT3};
char song[20] = {		/* pulse based on y=t*exp(t^2) */
  0, 90, 168, 223, 252, 255, 236, 202, 162, 122,
  86, 58, 37, 22, 12, 7, 3, 2, 1, 0
};

int main(void)
{  short int i;
  short int j;
  short int k;

  DDRB = 0x1F;			/* set pins 0-4 as outputs */
  PORTB = ALL_OFF;
  flashall();			/* boot sequence */
  ioinit();			/* start PWM timers */

  for (;;) {

    for (j=0; j<2; j++) {
      PORTB = ALL_OFF;
      for (i=0; i<3; i++) {
	_delay_ms(5500);	    /* wait 5.5 s between songs */
	PORTB &= ~outpin[i];	    /* one output pin on */
	for( k = 0; k < 20; k++ ) { /* play a song */
	  if (j) {
	    OCR1B = song[k];	/* PWM on IN1 */
	  }
	  else {
	    OCR1A = song[k];	/* PWM on IN2 */
	  }
	  _delay_ms(25);
	} 
	PORTB |= OPINS;		/* all output pins off */
      }
    }
    
  }

  return 1;
}

void ioinit(void) {
  // init timer1 for PWM on both IN1 and IN2
  TCCR1 = (1<<COM1A1)|(1 << PWM1A) | (1<<WGM01)|(1<<WGM00);
  GTCCR = (1 << PWM1B) | (1 << COM1B1);
}

// turn on all LEDs one at a time for boot sequence
void flashall(void) {
  short int i;
  short int j;

  for (j=0; j<2; j++) {
    PORTB = ALL_OFF;
    PORTB |= inpin[j];		/* one input pin on */
    for (i=0; i<3; i++) {
      PORTB &= ~outpin[i];	/* one output pin on */
      _delay_ms(WAIT);
      PORTB |= OPINS;		/* all outputs off */
    }
  }
}
