/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: owilowlevel.c
* Purpose: Port level bit-banging funtions to access a Dallas Semiconductor 1-Wire bus
*
* Description:
*  Port level access to Dallas 1-Wire bus.  These functions ARE device
*  speed/timing dependent and have been adjusted to match timings present
*  in AN126
*
*  Based on Dallas Semiconductor Application Note AN126
*     "1-Wire Communication Through Software"
*  Based on code by Peter Dannegger and Martin Thomas
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\owi_lowlevel.c,v 1.1 2007-10-28 19:36:21+08 tblough Exp tblough $

#include <avr/io.h>
#include <avr/interrupt.h>

#include "owi_lowlevel.h"
#include "owi_defs.h"
#include "delay.h"


/*  This function initializes the 1-Wire bus(es) by releasing it and
 *  waiting until any presence sinals are finished.
 */
extern void owi_init()
{
    OWI_RELEASE_BUS();
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (H) to be 
    // sure that the real reset is interpreted correctly.
    delay_us( OWI_DELAY_H);
}

/*  Generates the waveform for transmission of a Reset pulse on the 
 *  1-Wire(R) bus and listens for presence signals.
 *
 *  returns OWI_BUS_OK if presence signal was detected or OWI_BUS_ERROR
 *  if no devices present or bus error exists
 */
extern uint8_t owi_reset()
{
	uint8_t sreg, b;

	// first part is long delay with significant timing leeway so
	// don't disable interrupts until after
	OWI_PULL_BUS_LOW();
	delay_us( OWI_DELAY_H + 12);

	sreg = SREG;
	cli();	// disable interrupts

	OWI_RELEASE_BUS();	// allow bus high
	delay_us( OWI_DELAY_I);

	b = ~OWI_GET_IN();

	SREG = sreg;	// restore interrupts

	// another long delay without max so re-enable interrupts before
	delay_us( OWI_DELAY_J);
	// check to see if the bus is stull pulled low (shorted?)
	return b & OWI_GET_IN();
}

/* write or read a single bit from the bus
 * note that owi_bit_io( b) is the same as owi_bit_wr( b)
 * and b = owi_bit_io( 0x01) is the same as b = owi_bit_rd();
 */
extern uint8_t owi_bit_io( uint8_t b)
{
	uint8_t sreg;

	sreg = SREG;
	cli();

	// attention pulse
	OWI_PULL_BUS_LOW();	// drive bus low
	// delay_us( OWI_DELAY_A);
	//delay_us( 1); //code was too long - replace with inline assembler delays
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();

	// if writing 1 (or reading), stop pulling low data period
	if( b)
		OWI_RELEASE_BUS();	// allow bus high

	// response wait
	//delay_us( OWI_DELAY_E);
	delay_us( 1);
	b = OWI_GET_IN();	// sample at end of response wait

	delay_us( OWI_DELAY_F - OWI_DELAY_D - 3);	// wait for rest of R/W time slot

	// recovery time
	OWI_RELEASE_BUS();	// allow bus high if we were reading or writing 0, else we were already there
	// delay_us( OWI_DELAY_D);
	//delay_us( 1); // code was too long - replace with assembler delays
	DELAY_1US();
	DELAY_1US();

	SREG = sreg; // sei();

	return b;
}

/* individual bit write and read functions are only needed for debugging and delay testing
 * normally we can use the combiner IO function to write and read a bit at the same time.
 * to reduce code size if not needed, comment out the OWI_LL_DEBUG definition in the header file.
 */
#ifdef OWI_LL_DEBUG
// write a single bit to the 1-Wire bus
extern void owi_bit_wr( uint8_t b)
{
	uint8_t sreg;

	sreg = SREG;	// stop interrupts to maintain timing
	cli();

	// do attention pulse
	OWI_PULL_BUS_LOW();
	// delay_us( OWI_DELAY_A);
	// delay_us( 1) code was too long - replace with inline assembler delays
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();
	
	// if writing 1, stop pulling low data period
	if( b)
		OWI_RELEASE_BUS();

	// data period
	delay_us( OWI_DELAY_B - OWI_DELAY_D - 4);

	OWI_RELEASE_BUS();
	// recovery time
	// delay_us( OWI_DELAY_D);
	// delay_us( 1) code was too long - replace with assembler delays
	DELAY_1US();
	DELAY_1US();

	SREG = sreg;	// sei();
}

//read a single bit from the 1-Wire bus
extern uint8_t owi_bit_rd()
{
	uint8_t sreg, b;

	sreg = SREG;
	cli();

	// attention pulse
	OWI_PULL_BUS_LOW();
	// delay_us( OWI_DELAY_A);
	//delay_us( 1); // code was too long - replace with inline assembler delays
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();
	DELAY_1US();

	// stop pulling low for data period
	OWI_RELEASE_BUS();

	//delay_us( OWI_DELAY_E);
	delay_us( 1);
	b = OWI_GET_IN();	// sample at end of response wait

	delay_us( OWI_DELAY_F - 11);	// wait for rest of time slot

	SREG = sreg; // sei();
	return b;
}
#endif	// OWI_DEBUG

