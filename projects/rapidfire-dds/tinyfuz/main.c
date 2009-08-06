/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */

#include <stdint.h>
#include <stdlib.h>

#include "config.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "sintable.h"

/* Change fire rate */
/* move the decimal place two places right */
/* 1300 is 13.00 fires per second */
#define FIRERATE1 1300

#define PHASEONE2DIG 87617 
#define SOFTSCALE 255

/* Phase acculmulator */
/* 32 bits resolution, accessable by each byte */
union
{
    uint32_t acc;
    uint8_t b[4];
} pax;

/* Tuning word */
/* Changes fire rate */
volatile struct
{
    uint32_t step;
} ratectl;

/* Flags */
/* ISR only sets, main only clears */
#define SOFTTICK 0

volatile struct
{
    uint8_t flags;
} isrflags;

/* It's all done here */
ISR(TIMER1_OVF_vect)
{
    static uint16_t scaler = SOFT_SCALE;
    static uint8_t OCR = 0;

    /* output on two pins first to insure timing */
    OCR1A = OCR;
    OCR1B = OCR;

    /* bad juju */
    pax.acc += ratectl.step;
    OCR = pgm_read_byte
          (
              /* offset into program memory */
              &sintable
              [
                  /* grab the 8 most significant bits from union */
                  /* then shift bits out until size of table */
                  pax.b[3]>>(8-SINTAB2SIZE)
              ]
          );

    /* simple soft timers */
    if (--scaler == 0)
    {
        scaler = SOFT_SCALE;
        isrflags.flags |= _BV(SOFTTICK);
    }
}

void ioinit(void)
{
    /* set pin for pwm output */

    PWMDDR |= 
	_BV(PWMA) |
    	_BV(PWMB);

    TCCR0A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS00);
    TIMSK |= _BV(TOIE1);
    OCR1A = 0;

    /* main has to catch up after start */
    wdt_enable(WDTO_2S);
}

void readbuttons(void)
{
#ifdef DEBUG
    uint32_t stepcopy;

    /* Helps with debugging */
    /* ISR never writes */
    stepcopy = ratectl.step;

    if (BUTTONIN & _BV(PIN_SLOW))
    {
        if (stepcopy-CTL < stepcopy)
            stepcopy-=CTL;
    }
    if (BUTTONIN &  _BV(PIN_FAST))
    {
        if (stepcopy+CTL < (0xFFFFFFFFl>>2))
            stepcopy+=CTL;
    }
    */
    /* throw dice */
    /* worst case we access while in isr and one phase 
    (1/32)second is jittered */
    ratectl.step=stepcopy;
#endif
}

/* simple, blocking protection */
inline void clearflag(uint8_t f)
{
    cli();
    isrflags.flags &= ~_BV(f);
    sei();
}

int main(void)
{
    uint8_t flags;
    /* stop gcc optomizer */
    asm("nop");

    /* init globals */
    pax.acc = 1;
    ratectl.step = (PHASEONE2DIG * FIRERATE1);
    isrflags.flags = 0;

    ioinit();

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    /* main loop */
    for (;;) 
    {
        wdt_reset();
        flags = isrflags.flags;

	/* soft clock tick */
	/* uses less battery if only checking for updates less offen */
        if (flags &  _BV(SOFTTICK))
        {
            clearflag(SOFTTICK);
	    readbuttons();
        }

        /* it is running on battery */
	/* will sleep until next interupt */
	sleep_mode();
    }
}
