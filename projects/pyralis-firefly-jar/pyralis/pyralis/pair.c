/* Name: pair.c
 * Author: Kelsey Jordahl
 * Copyright: Kelsey Jordahl 2009
 * License: GPLv3
 * Time-stamp: <Sat Dec 26 09:10:56 EST 2009> 

Simulate fireflies in a jar
Uses the same hardware circuit as the Jar-of-Fireflies Instructable
   http://www.instructables.com/id/EITCRQFB2JEWP86TYK/
with simpler software logic.

Simulate a pair of fireflies of species Photinus pyralis, the common
Eastern firefly.  Pulse shape and firefly behavior modeled after Case
[2004]. One male will flash every 6 s; a female responding 2 s later
will randomly appear and disappear.

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

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define WAIT 250		/* 250 ms delay */
#define PULSELEN 20		/* length of pulse */
#define IN1 (1 << PB1)		/* Pin  1 */
#define IN2 (1 << PB4)		/* Pin  4 */
#define OUT1 (1 << PB3)		/* Pin  3 */
#define OUT2 (1 << PB2)		/* Pin  2 */
#define OUT3 (1 << PB0)		/* Pin  0 */
#define IPINS (IN1 | IN2)
#define OPINS (OUT1 | OUT2 | OUT3)
#define ALL_ON 0x1F & ~OPINS	/* input pins high; output pins low */
#define ALL_OFF 0x1F & ~IPINS	/* input pins low; output pins high */

// declare functions
void flashall (void);
void start_timers (void);
void getmask (void);

// declare variables
volatile unsigned char inpin[2] = {IN1, IN2};
volatile unsigned char outpin[3] = {OUT1, OUT2, OUT3};
volatile unsigned char mpulse[20] = { /* pulse based on y=t*exp(t^2) */
  0, 90, 168, 223, 252, 255, 236, 202, 162, 122,
  86, 58, 37, 22, 12, 7, 3, 2, 1, 0
};
//volatile unsigned char fpulse[20] = { /* female signal is dimmer by 1/2 */
//  0, 45, 84, 112, 126, 128, 118, 101, 81, 61,
//  43, 29, 19, 11, 6, 4, 2, 1, 1, 0
//};
//volatile unsigned char fpulse[20] = { /* female signal is dimmer by 1/4 */
//  0, 23, 42, 56, 63, 64, 59, 51, 41, 31, 22, 15, 10, 6, 3, 2, 1, 1, 1, 0
//};
volatile unsigned char fpulse[20] = { /* female signal is dimmer by 1/10 */
  0, 9, 17, 23, 26, 26, 24, 21, 17, 13, 9, 6, 4, 3, 2, 1, 1, 1, 1, 0
};
 
volatile unsigned char count;	/* count ticks within a cycle */
volatile unsigned char fcount=78; /* ~2.0 s delay for female response */
volatile unsigned char mcount=234; /* ~6.0 s period for male */
volatile unsigned char fpin;	   /* output pin for females */
volatile unsigned char mpin;	   /* output pin for males */
volatile unsigned char malemask;   /* opins for males */
volatile int seed=0xD091;	   /* random seed */
volatile unsigned int temp;	   /* temporary storage of rand() */

// define needed bit flags
typedef struct {
  unsigned char bit0:1;
  unsigned char bit1:1;
  unsigned char bit2:1;
  unsigned char bit3:1;
  unsigned char bit4:1;
  unsigned char bit5:1;
  unsigned char bit6:1;
  unsigned char bit7:1;
} io_reg;
#define m1 ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit0
#define m2 ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit1
#define f1 ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit2
#define f2 ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit3
#define n1 ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit4
#define female ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit5
/* #define  ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit6 */
/* #define  ((volatile io_reg*) _SFR_MEM_ADDR(GPIOR0))->bit7 */

// interrupt timer function
ISR(TIM0_COMPA_vect) {

  // flash a random male
    if (++count<PULSELEN) {
      PORTB = ~malemask;
      if (m1) {
	OCR1A = mpulse[count];
      }
      if (m2) {
	OCR1B = mpulse[count];
      }	  
    }
    // female response
    if (female) {
      if ((count>fcount) & (count<(fcount+PULSELEN))) {
	PORTB = ~outpin[fpin];
	if (f1) {
	  OCR1A = fpulse[count-fcount];
	}
	if (f2) {
	  OCR1B = fpulse[count-fcount];
	}	  
      }
    }
    // see if we're done with this cycle
    if (count==mcount) {
      if ((rand() % 8)==0) {		 /* toggle female randomly */
	if (female) {
	  female=0;
	} else {
	  fpin = (rand() % 3);		/* pick an output pin for female */
	  f1=(rand() % 2); f2=~f1;	/* pick a side for female */
	  female=1;
	}
      }
      count=0;
      getmask();		/* randomize male location */
    }
}


int main(void)
{
  DDRB = 0x1F;			/* set pins 0-4 as outputs */
  PORTB = ALL_OFF;

  flashall();			/* boot sequence */

  srand( seed );
  
  getmask();

  set_sleep_mode(SLEEP_MODE_IDLE);
  start_timers();
  sei();

  for (;;) {	     /* do nothing; everthing happens in interrupts */
    sleep_mode();
  }

  return 1;			/* never gets here */
}

void start_timers(void) {
  // timer1 will generate PWM on both IN1 and IN2
  TCCR1 = (1 << COM1A1) | (1 << PWM1A) | (1 << WGM01) | (1 << WGM00);
  GTCCR = (1 << PWM1B) | (1 << COM1B1);
  OCR1A = 0; OCR1B = 0;
  // timer0 will be set to generate an interupt every ~25.6 ms
  TCCR0A = (1 << WGM01);	    /* CTC mode */
  TCCR0B = (1 << CS02 | 1 << CS00);		/* clock/1024 */
  TIMSK = (1 << OCIE0A);			/* enable interupts */
  OCR0A = 25;
}

// turn on all LEDs one at a time for boot sequence
void flashall(void) {
  unsigned char i;
  unsigned char j;

  for (j=0; j<2; j++) {
    PORTB = ((ALL_OFF) | inpin[j]);	/* one input pin on */
    for (i=0; i<3; i++) {
      PORTB &= ~outpin[i];	/* one output pin on */
      _delay_ms(WAIT);
      PORTB |= OPINS;		/* all outputs off */
    }
  }
}

// choose which LED will be lit for the male
// depends on global variables, not passed through arguments
// avoid using same pin as female, if on
void getmask() {

  temp=rand();		/* use one rand() call */

  mpin = (temp % 3);		/* pick an output pin for male */
  malemask=outpin[mpin];
  m1=(temp % 2);			/* pick a side */
  if ((female) & (mpin==fpin) & (m1==f1)) {	/* make sure different than female */
    m1=~f1;
  }
  m2=~m1;
}
