/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <interrupt.h>
#include <util/delay.h>

volatile uint8_t reset_source = 0;
volatile bool triggered = false;

int main (void)
{

//	board_init();

	// Insert application code here, after the board has been initialized.

	// save and clear MCUSR
	reset_source = MCUSR;					// store reset source information
	MCUSR = 0x00;							// clear register
	
	// configure CPU clock
	CLKPR = 0x80;							// CLKPCE
	CLKPR = 0x07;							// divide by 128 = 62.5 kHz CPU clock
	
	// set up 2 second timer for LED delay
	TCNT0 = 0;								// reset timer
	TCCR0A = (1 << WGM01);					// Clear Timer on Compare (CTC) mode
	OCR0A = 122;							// Compare A : 2 seconds at 61 Hz clock
	TIMSK = (1 << OCIE0A);					// interrupt on compare A
	TCCR0B = (1 << CS02) | (1 << CS00);		// Start timer : CPU clock / 1024 = 61 Hz counter clock
	
	// set up GPIO output for LEDs
	PORTB |= (1 << PB3) | (1 << PB4);		// Set PB3 and PB4 high for LED to be off
	DDRB |= (1 << PB3) | (1 << PB4);		// Enable outputs
	
	Enable_global_interrupt();
	
	while (1)
	{
		if(triggered)						// indicate source by blinking PB4
		{
			if (reset_source == ((1<<BORF) | (1<<PORF)))	// both bits set on power on if BOD fuse is enabled
			{
				reset_source = reset_source & ~(1<<BORF);	// so clear brown out flag
			}
			for(uint8_t x = reset_source; x > 0; x = x >> 1)
			{
				PORTB &= ~(1 << PB4);		// turn LED on
				_delay_ms(200);				// 
				PORTB |= (1 << PB4);		// turn LED off
				_delay_ms(200);				//
			}
			_delay_ms(1000);				// for repeat after triggered
		/*		WDT	BOR	EXT	POR		Blink
				0	0	0	1		1
				0	0	1	0		2
				0	1	0	0		3
				1	0	0	0		4
		*/
		}
	}
}

ISR(TIMER0_COMPA_vect)					// 2 second timer compare
{
	PORTB &= ~(1 << PB3);				// Clear to turn on LED
	TCCR0B = 0x00;						// Stop 2 second timer
	triggered = true;					// 
}
