// pin-change-test
//
// Tom Benedict

// This example makes use of the pin-change subsystem.  The idea is
// to write subroutines that will do something any time a particular
// I/O pin changes state, and set them up so they run each time that
// pin changes state.
//
// This will work on the Baby-O, Orangutan, and Orangutan-X2. 
// Unfortunately this is one of those examples that requires external
// hardware in order for it to work.  It also tests more than just
// the pin change interrupt subsystem, since it relies on the LCD
// in order to function.  The LCD stuff could just as easily be
// traded out with a flashing LED, serial UART, servo output, or 
// what have you.
//
// Take a quadrature encoder and plug its two outputs into PC0 and
// PC1.  The encoder this is written for is an ALPS mechanical 
// contact encoder, with the two channels' common connection being
// to ground.  This means the AVR's internal pull-up resistors must
// be used in order for this to work.  If you're using an Orangutan-X2,
// you will need to change the code to use pins on PORTA or PORTD since
// only these two ports are brought out to headers.
//
// One cautionary note when using this:  Mechanical switches
// are VERY very prone to "bounce".  That is their transitions
// from open to closed and back tend to be noisy.  For a lightbulb
// this doesn't mean a lot.  To a microprocessor running millions
// of instructions per second, each noisy jitter is seen as a 
// separate transition and will be handled that way.  Using a
// microswitch plugged into PC0 and clicking it, I typically got
// five to twelve transitions per click rather than the single one
// a clean transition would've given me.  Here there be dragons.
//
// The program reads the quadrature encoder and puts the value of a
// running counter up on the LCD.  The number is not allowed to
// go negative or higher than 999 (since the lcd_int() routine only
// prints three digits.)

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for the pin-change subsystem
#include "pin-change.h"

// LCD
#include "lcd.h"


// A global variable to store our counter value
volatile int encoder_counter = 0;
unsigned char enc_dir;

// Two different handlers are offered for servicing the encoder.
// One is similar to how the Savage Innovations OOPic processors
// handle quadrature encoders:

void pc0_interrupt_savage_innovations(void)
{
	// Rising edge?
	if(PINC & (1 << PC0))
	{
		// Encoder B channel high?
		if(PINC & (1 << PC1))
		{
			// Increment
			encoder_counter++;
		} else {
			// Decrement
			encoder_counter--;
		}

		// Make sure the counter is in range
		if (encoder_counter < 0)
			encoder_counter = 0;

		if (encoder_counter > 999)
			encoder_counter = 999;
	}
}

// The other is done the same way Jim Remington's code
// did in the encoder post he made to the Pololu Forum.  In Jim's
// code he used an algorithm Scott Edwards wrote in an article
// in Nuts&Volts V1 October 1995.  In his algorithm, the rightmost
// bit of old A,B is XORed with the leftmost bit of the new
// A,B.  This determines the direction of rotation.

void pc0_interrupt_scott_edwards(void)
{
	static unsigned char enc_last=0,enc_now;

	// Read the port pins and shift the result to the bottom
	// two bits.  Since we're using PC0 and PC1, they're
	// already shifted for us.
	enc_now = (PINC & (3));

	// Determine the direction of rotation
	enc_dir = (enc_last & 1)^((enc_now & 2) >> 1);

	// Update encoder position
	if(enc_dir == 0) encoder_counter++; else encoder_counter--;

	// Save state
	enc_last = enc_now;

	// Make sure the counter is in range
	if (encoder_counter < 0)
		encoder_counter = 0;

	if (encoder_counter > 999)
		encoder_counter = 999;
}


// And now for our main routine:
int main(void)
{

#ifdef _ENABLE_LCD_
	// LCD
	lcd_init();
#endif // _ENABLE_LCD_

	// Set up our two encoder channels as inputs:
	DDRC &= ~((1 << PC0) | (1 << PC1));

	// Set our pull-ups for both channels
	PORTC |= (1 << PC0) | (1 << PC1);

	// Pick one of these two routines to use for handling the
	// quadrature encoder:

	// Register our pseudo-interrupt routine using the Scott Edwards
	// algorithm:
//	pci_define(_SFR_IO_ADDR(PINC), PC0, *pc0_interrupt_scott_edwards);

	// Register our pseudo-interrupt routine using the Savage
	// Innovations algorithm:
	pci_define(_SFR_IO_ADDR(PINC), PC0, *pc0_interrupt_savage_innovations);


	// The endless loop

	for(;;)
	{
		// Notice that our endless loop doesn't actually do anything to
		// read the encoder.  That's all taken care of with the pin change
		// interrupt code.  The only thing that happens here is we print
		// out the value of the encoder.

#ifdef _ENABLE_LCD_
		lcd_gotoxy(0,0);
		lcd_string("ENC: ");
		lcd_int((unsigned int)(encoder_counter));
#endif // _ENABLE_LCD_
	}

	// We never get here, but return a zero if we do:
	return(0);
}
