// Routine for reading QTI sensors
//
// Reading the datasheet, you'd think the Parallax QTI sensor was a
// strict analog device.  Not so!  The capacitor on the QTI isn't 
// for filtering noise.  It's a charge integrator.
//
// To use a QTI sensor, you must:
//
// 1 - Set the I/O line to output
// 2 - Bring the I/O line high to drain the capacitor
// 3 - Wait 0.230ms for the drain to finish
// 4 - Bring the I/O line low
// 5 - Set the I/O line as input
// 6 - Wait 0.230ms for the capacitor to charge
// 7 - Read the ADC for that channel
//
// Convoluted?  Yes.  But it does work.  Why'd they design it that
// way?  My guess is the photoreflector they used is noisy, and this
// is a good way not only to dump the noise, but to up the overall
// sensitivity since it's an integrated device.
//
// This uses two #defines from device.h: ADC_PORT and ADC_DDR.  On
// the ATMega168, the ADC inputs are on PORTC.  On the 644 they're on
// PORTA.  This lets the code work with either platform.
//
// Tom Benedict

// qti10 returns a 10-bit value representing the reflectance the QTI
// sensor sees.

#include "device.h"
#include <avr/io.h>
#include <util/delay.h>
#include "analog.h"

uint16_t qti10(unsigned char channel)
{
	// Make sure the channel is between 0 and 7
	channel &= 0x7;

	// Set PCx as output
	ADC_DDR |= (1 << channel);
	// Bring that pin high
	ADC_PORT |= (1 << channel);
	// Delay 0.230ms
	_delay_us(230);
	// Bring that pin low
	ADC_PORT &= ~(1 << channel);
	// Set PC0 as input
	ADC_DDR &= ~(1 << channel);
	// Delay 0.230ms
	_delay_us(230);
	// Read the channel
	return(analog10(channel));
}

// qti8 returns a 10-bit value representing the reflectance the QTI
// sensor sees.  If you've read through the analog.c file, you'll
// recognize the cheesiness I'm using to get the 8-bit value.

uint8_t qti8(unsigned char channel)
{
	return((qti10(channel) >> 2) & 0xFF);
}
