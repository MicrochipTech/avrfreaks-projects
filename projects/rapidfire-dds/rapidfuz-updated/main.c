/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#include <stdint.h>
#include <stdlib.h>

#include "config.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "globals.h"
#include "sintable.h"
#include "eeprom.h"

/*
 * Global variables
 */
union pax_u pax;
uint32_t pax_next;
volatile struct ratectl_s ratectl;
struct buttons_s buttons;
volatile struct isrflags_s isrflags;

/*
 * Startup hardware
 */
void ioinit(void)
{
    /* set pin for pwm output */

    PWMDDR |= 
	_BV(PWMA); 
    PWMDDR |= 
    	_BV(PWMB);

    /* set pin for output */
    LEDDDR |= 
	_BV(PIN_STATUS);
#   ifdef DEBUG
    LEDDDR |= 
    	_BV(PIN_CLOCK);
    LEDDDR |= 
    	_BV(PIN_SCLOCK);
    LEDDDR |= 
    	_BV(PIN_ISR);
#   endif

    /* 
     * set pins for input 
     */
    BUTTONDDR &=
	~_BV(PIN_MODE);
#	ifdef DEBUG
    BUTTONDDR &=
    	~_BV(PIN_LTRIG);
    BUTTONDDR &=
    	~_BV(PIN_RTRIG);
    BUTTONDDR &=
    	~_BV(PIN_MAC0);
    BUTTONDDR &=
    	~_BV(PIN_MAC1);
#	endif

#if defined(__AVR_ATmega16__)

    TCCR1A = _BV(COM1A1) | _BV(COM1B1) |  _BV(WGM10);
    TCCR1B = _BV(WGM12) | _BV(CS11);
    TIMSK |= _BV(TOIE1);
    OCR1A = 0;
#elif defined(__AVR_ATtiny45__)
    TCCR0A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS00);
    TIMSK |= _BV(TOIE1);
    OCR1A = 0;
#endif
    /*
     * main has to catch up after start 
     */
    /* TODO after timing move this down */
    wdt_enable(WDTO_120MS); 
}

/*
 * Reads buttons and only sets flags
 */
void readbuttons(void)
{

#   ifdef DEBUG
    uint32_t stepcopy;
    /*
     * ISR never writes
     * make local copy to speed up loading from volaitile 
     */
    stepcopy = ratectl.step;
#   endif

    /* need to rewrote as hash/switch structure or eleminate branching */
    if (BUTTONIN & _BV(PIN_MODE))
    {
	    buttons.flags |= _BV(B_MODE);
    }

#   if defined(__AVR_ATmega16__)
    if (BUTTONIN & _BV(PIN_LTRIG))
    {
	    buttons.flags |= _BV(B_LTRIG);
    }
    if (BUTTONIN & _BV(PIN_RTRIG))
    {
	    buttons.flags |= _BV(B_RTRIG);
    }
    if (BUTTONIN & _BV(PIN_MAC0))
    {
	    buttons.flags |= _BV(B_MAC0);
    }
    if (BUTTONIN & _BV(PIN_MAC1))
    {
	    buttons.flags |= _BV(B_MAC1);
    }
#   endif

#   ifdef DEBUG
    /*
     * Rate change buttons 
     */
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
    /* 
     * throw dice 
	 * worst case is ISR access and one (2^32/rate/table) cycle is trashed  
	 * not worth the protection for a minisule amount of jitter while changing
	 * rates already makes jitter anyway 
     */
    ratectl.step=stepcopy;
#   endif

}

/* 
 * simple, blocking protection 
 */
inline void clearflag(uint8_t f)
{
    cli();
    isrflags.flags &= ~_BV(f);
    sei();
}

/*
 * main loop blocked in for 
 */
int main(void)
{
    uint8_t flags;
    uint8_t cur_mode;

    uint8_t blink_cnt;
    uint8_t blink_dly;

    uint8_t lburst_start;
    uint8_t lburst_end;
    uint8_t rburst_start;
    uint8_t rburst_end;
    asm("nop"); /* stop gcc optomizer */

    /* 
     * init globals 
     */
    pax.acc = 1;
    ratectl.step = 9000000; /* random staring place */
    isrflags.flags = 0;
    buttons.flags = 0;
    cur_mode = 0;	
    blink_cnt = 0;
    blink_dly = 0;

    eepromload(0);
    ioinit();

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    LEDPORT |= _BV(PIN_STATUS);
	/* 
     * Main loop
     */
    for (;;) 
    {
#	ifdef DEBUG
	LEDPORT ^= _BV(PIN_CLOCK);
#	endif
	sleep_mode();
	/* Keep idle loop short to save battery */
    flags = isrflags.flags;
	/* Time hasn't expired yet */
    if (flags &  _BV(I_SOFTTICK))
	    continue;
	/* Soft timer needs to be run */
    wdt_reset();
    clearflag(I_SOFTTICK);
#	ifdef DEBUG
	/* flash on softtick */
	LEDPORT ^= _BV(PIN_SCLOCK);
#	endif
	readbuttons();
    /*
     * Big nasty event handler
     */
	if ( (flags & _BV(B_LTRIG)) && (flags & _BV(I_ZEROC)) )
	{
        
	}

	if ( (flags & _BV(B_RTRIG)) && (flags & _BV(I_ZEROC)) )
	{

	}

	if (flags & _BV(B_MODE))
	{
	    cur_mode++;
	    if (cur_mode > (MODES - 1))
		cur_mode = 0;
	    /* blink_cnt has to be doubled */
	    blink_cnt = cur_mode;
	    blink_dly = BLINKDELAY;
	    LEDPORT &= ~_BV(PIN_STATUS);
	}

	/* 
     * toggle light
     */
	if (blink_cnt)
	{
	    blink_dly--; 
	    if (!blink_dly)
	    {
		    blink_cnt--;
		    blink_dly = BLINKDELAY;
		    LEDPORT ^= _BV(PIN_STATUS);
	    }
	}
    }
}
