#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
/* Principle of operation:

Brightness
                        /\
      /\              /    \
  ___/  \___________/        \____
    Time ->

  The LEDs are faded in and out by a software PWM generator.
  the PWM cycle is increased from 0 to a random maximum by a random value,
  so the ON-time varies every cycle. After reaching zero brightness, a
  random delay is inserted. Well, that's all but looks great, when applied to
  15 LEDs of differnt colors.
  Works best, if all LEDs have more or less the same brightness. Super-/ultrabright
  LEDs preferred. If your LEDs (like mine) are highly directional, grind the lens flat.
  Looks much better ....
  And now.... have fun.
*/

// for attiny 26, adjust ports for other devices
// clock should be 8 MHz, maybe it's ok w/ 4Mhz, otherwise the effects get to slow
// be sure to set fuses on tiny26 to internal osc. 8 Mhz

#define MX 16                // # of leds (well, reset line cannot be used for output, we ignore this fact ...)
volatile uint8_t pwm[16];    // current pwm value = brightness
volatile int8_t inc[16];    // random increase brightness over time
volatile uint8_t dly[16];   // random delay before turning on again
volatile uint8_t mx[16];    // random maximum brightness 

 

ISR(SIG_OVERFLOW0)
{
  register uint8_t i;

  for(i = 0; i < MX; i++)
  {
    if(inc[i])              //  still in pwm cycle
    {
     pwm[i] += inc[i];   
     if(pwm[i] > mx[i])  inc[i] = -inc[i];     //we use signed add, so we need no flag for up/down counting
     else if(pwm[i] == 0)  inc[i] = 0;         // one cycle complete ... delay
     continue;
    }
    if(dly[i])                                 // delay phase
    {
     dly[i]--;
     continue;
    }
    // now inc == 0 & dly == 0                     // end of delay phase, compute new values for next cycle
    dly[i] = 127 + (uint8_t)rand() & 127;
    inc[i] = 1 + ((uint8_t)rand() & 7);
    mx[i] = 100 + ((uint16_t)rand() & 127);
  }
}

int main()
{
  register uint8_t i;
  register uint8_t tv = TCNT0;
  register uint8_t v=0; 
  
  
  DDRB = 0xff;        // all output
  DDRA = 0xff;        // all output
  TCCR0 = 4;          // ck/1024 ~ 8kHz  
  
  TIFR = _BV(TOV0);   
  TIMSK = _BV(TOIE0);
  
  sei();

  srand(17);   // TODO: insert really random value, in case of multiple devices 
               // doesn't look good if they are more or less in sync
  while(1)
  {
    tv = TCNT0;
    v = 0;
    for(i = 0; i < 8; i++)           // lower half is PORTB
    {    
      v <<= 1;
      if(tv >= pwm[i])  v |= 1;      // we use outputs as sink, so pwm is inverted
    }
    PORTB = v;
    v = 0;
    for(i = 8; i < MX; i++)         // lower half is PORTB
    {    
      v <<= 1;
      if(tv >= pwm[i])  v |= 1;
    }
    PORTA = v;

  }
  
  return(0);
}
