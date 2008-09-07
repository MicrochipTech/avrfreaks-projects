// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\owi_lowlevel.h,v 1.1 2007-10-28 19:36:42+08 tblough Exp tblough $

#ifndef _OWI_LOWLEVEL_
#define _OWI_LOWLEVEL_

#include <inttypes.h>
#include "owi_defs.h"

#define OWI_GET_IN() ((OWI_IN & (1<<OWI_PIN)) >> OWI_PIN)

#define OWI_PULL_BUS_LOW() \
            OWI_DDR |= (1 << OWI_PIN);  /* output */ \
            OWI_OUT &= ~(1 << OWI_PIN); /* low */
 
#define OWI_FORCE_BUS_HIGH() \
            OWI_DDR |= (1 << OWI_PIN); /* output */ \
            OWI_OUT |= (1 << OWI_PIN); /* high */
           
#define OWI_RELEASE_BUS() \
            OWI_DDR &= ~(1 << OWI_PIN); /* input */ \
            OWI_OUT &= ~(1 << OWI_PIN); /* disable pullup */

// function declarations
void owi_init( void);
extern uint8_t owi_reset( void);
extern uint8_t owi_bit_io( uint8_t b);

/* low level routines to read a bit and write a bit on the 1-Wire interface
 * useful for debugging delay times.  These functions are replaced by owi_bit_io()
 * which combines reading and writing.  These are only needed for debuging,
 * so comment out OWI_LL_DEBUG to reduce code size if not using
 */
//#define OWI_LL_DEBUG
#ifdef OWI_LL_DEBUG
void owi_bit_wr( uint8_t b);
uint8_t owi_bit_rd( void);
#endif

#endif	// _OWI_LOWLEVEL_
