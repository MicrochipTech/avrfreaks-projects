/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "globals.h"
#include "config.h"
#include "sintable.h"

ISR(TIMER1_OVF_vect)
{
    static uint16_t scaler = SOFT_SCALE;
    static uint8_t OCR = 0;

    /* output on two pins first before any branches */ 
    OCR1A = OCR;
    OCR1B = OCR;

    pax_next = pax.acc + ratectl.step;
    /* count overflow */
    if (pax_next < pax.acc){
        isrflags.flags |= _BV(I_ZEROC);
	    ratectl.count++;
    }
    pax.acc = pax_next;
    /* bad juju */
    OCR = pgm_read_byte
          (
              /* offset into program memory */
              &sintable
              [
                  /* grab the 8 most significant bits from union */
                  /* then shift bits out unti size of table */
                  pax.b[3]>>(8-SINTAB2SIZE)
              ]
          );

    /* simple soft timers */
    if (--scaler == 0)
    {
        scaler = SOFT_SCALE;
        isrflags.flags |= _BV(I_SOFTTICK);
    }
#   ifdef DEBUG
    LEDPORT ^= _BV(PIN_ISR);
#   endif
}
