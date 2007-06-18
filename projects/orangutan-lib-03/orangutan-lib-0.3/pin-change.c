// pin-change
//
// Tom Benedict


// The ATMega processors can call an interrupt if any of their general
// purpose I/O pins changes state.  This is useful for a number of
// applications, including edge detection for mini-sumo robots,
// low-frequency quadrature encoders, user input on push buttons or
// digital joysticks, etc.  All the programmer needs to do is set up
// all the interrupt handling code and tell the ATMega which pins to
// listen to.
//
// To be fair this is NOT the most efficient way to get the job done.
// A lot of compromises were made in order to make it as generic as
// possible, and to make it relatively painless for someone to set up 
// and use.  If you're writing time-critical code or are concerned with
// code and variable space usage, it's going to be more efficient to
// write your own version of this.  If you're concerned with testing
// an idea and getting the ball rolling on your project, this should
// get you going.
//
// Also, fair warning the code in here is pretty opaque.  I did what I
// could to make it readable, but it's not my favorite example of how
// to write human-readable code.
//
// Those concerns aside, let's get to it!


// All device-specific dependencies are stored in this file.  You must
// edit it first before using any of the library routines!

#include "device.h"


#include <avr/io.h>

// This code depends heavily on interrupts

#include <avr/interrupt.h>


// The whole idea with this code is to cover, as completely as possible,
// all of the pin change interrupts the ATMega will let us use.  For each
// one, if the programmer tells us to associate a subroutine with that
// interrupt, we store that information and use it to call the right routine
// when that I/O pin changes state.
//
// There is one big chunk of information we need to store in order to pull
// this off: a table of user-provided subroutines, eight per I/O port,
// as well as the last known state of the PINx register for that port.
//
// We use this typedef to set up the storage for all this:

typedef struct {
	uint8_t 	last_pin;
	void (*funcs[8])(void);
} pci_port_type;


// And we use it to create our array to store the port information.
// Keep in mind that different ATMega chips have different numbers of
// pin change interrupts, and different port assignments for each.  The
// ATMega48/88/168 has three, assigned to PORTB, C, and D respectively.
// The ATMega644 has four, assigned to PORTA, B, C, and D respectively.
// The code has to be somewhat flexible as a result.

// Where we store our port settings
volatile pci_port_type pci_port[PCI_NUM];


// Now for the routines!  There's only one command for this subsystem: 
// pci_define().  It's where you say, "For a given PINx register and
// a given bit, run the following routine."  Of course there are caveats
// with how this is called:
//
// Let's say you have the following subroutines for running an incremental
// encoder with an index to indicate 0 degrees:
//
// 		void encoder_func(void)
// 		{
//			counter++;
//		}
//
//		void index_func(void)
//		{
//			counter = 0;
//		}
//
// To set these up on PB2 and PB3 respectively, you'd do:
//
//		pci_define(_SFR_IO_ADDR(PINB), 2, *encoder_func);
//		pci_define(_SFR_IO_ADDR(PINB), 3, *index_func);
//
// (If you've read through the servo.c code, the _SFR_IO_ADDR() will
// be familiar ground.)
//
// You can only call void func(void) style routines with this code.  No
// parameters.  Sorry.

void pci_define(uint8_t pinx, uint8_t bit, void (*pPCI_Func)(void))
{

	// Sort out what port we're using:
	if(pinx == _SFR_IO_ADDR(PCI_PIN0))
	{
		// Register the user's function in our storage array
		pci_port[0].funcs[bit] = pPCI_Func;

		// Save the state of PINx so we can tell which pin changes when
		// the Pin Change Interrupt for that port is called
		pci_port[0].last_pin = PCI_PIN0;

		// Enable the correct pin in the PCMSKx register for this port
		PCMSK0 |= (1 << bit);
		
		// Enable Pin Change Interrupt for this port
		PCICR |= (1 << PCIE0);

#if PCI_NUM > 1
// Only include this if we have more than one pin change interrupt		
	} else if(pinx == _SFR_IO_ADDR(PCI_PIN1))
	{
		// Register the user's function in our storage array
		pci_port[1].funcs[bit] = pPCI_Func;

		// Save the state of PINx so we can tell which pin changes when
		// the Pin Change Interrupt for that port is called
		pci_port[1].last_pin = PCI_PIN1;

		// Enable the correct pin in the PCMSKx register for this port
		PCMSK1 |= (1 << bit);
		
		// Enable Pin Change Interrupt for this port
		PCICR |= (1 << PCIE1);
#endif // PCI_NUM > 1

#if PCI_NUM > 2
// Only include this if we have more than two pin change interrupts	
	} else if(pinx == _SFR_IO_ADDR(PCI_PIN2))
	{
		// Register the user's function in our storage array
		pci_port[2].funcs[bit] = pPCI_Func;
		
		// Save the state of PINx so we can tell which pin changes when
		// the Pin Change Interrupt for that port is called
		pci_port[2].last_pin = PCI_PIN2;

		// Enable the correct pin in the PCMSKx register for this port
		PCMSK2 |= (1 << bit);
		
		// Enable Pin Change Interrupt for this port
		PCICR |= (1 << PCIE2);
#endif // PCI_NUM > 2

#if PCI_NUM > 3
// Only include this if we have more than three pin change interrupts	
	} else if(pinx == _SFR_IO_ADDR(PCI_PIN3))
	{
		// Register the user's function in our storage array
		pci_port[3].funcs[bit] = pPCI_Func;
		
		// Save the state of PINx so we can tell which pin changes when
		// the Pin Change Interrupt for that port is called
		pci_port[3].last_pin = PCI_PIN3;

		// Enable the correct pin in the PCMSKx register for this port
		PCMSK3 |= (1 << bit);
		
		// Enable Pin Change Interrupt for this port
		PCICR |= (1 << PCIE3);
#endif // PCI_NUM > 3
	}

	// Make sure interrupts are enabled
	sei();
}


// The rest of these are internal:


// These are the interrupt handlers for each of the pin change interrupts.
// Right now it's fairly ATMega48/88/168-centric, but this will be made more
// generic when the ATMega644 version is rolled.
//
// Originally there was a fourth routine called pin_changed() that actually
// handled the pin change stuff.  But it added one more function call to the
// whole train of events when a pin changed state, so it slowed things down.
//
// This is not my favorite way to do things.  Each of the interrupt handlers
// in here basically have the same code.  The only difference is an array
// index value.  It makes it harder to maintain and harder to read, but 
// on an 8MHz 168 it shaves 32 microseconds off a pin change.

volatile uint8_t bit, changed;

ISR(SIG_PIN_CHANGE0) 
{
	// Handle it

	// Find out which I/O pin changed (sorry, this is VERY ugly):
	changed = (PCI_PIN0 ^ pci_port[0].last_pin) & PCMSK0;

	// This is the fastest way I know of to find out which bit is
	// high.  This and the context-switching for the double
	// function call is where most of the time is lost.
	bit = 0;
	while (changed >>= 1)
	{
		bit++;
	}

	// Call the user's routine:
	pci_port[0].funcs[bit]();

	// Save our PINx value for the next compare
	pci_port[0].last_pin = PCI_PIN0;
}

#if PCI_NUM > 1
// Only include this if we have more than one pin change interrupt
ISR(SIG_PIN_CHANGE1) 
{
	// Handle it

	// Find out which I/O pin changed (sorry, this is VERY ugly):
	changed = (PCI_PIN1 ^ pci_port[1].last_pin) & PCMSK1;

	// This is the fastest way I know of to find out which bit is
	// high.  This and the context-switching for the double
	// function call is where most of the time is lost.
	bit = 0;
	while (changed >>= 1)
	{
		bit++;
	}

	// Call the user's routine:
	pci_port[1].funcs[bit]();

	// Save our PINx value for the next compare
	pci_port[1].last_pin = PCI_PIN1;
}
#endif // PCI_NUM > 1

#if PCI_NUM > 2
// Only include this if we have more than two pin change interrupts
ISR(SIG_PIN_CHANGE2)
{
	// Handle it

	// Find out which I/O pin changed (sorry, this is VERY ugly):
	changed = (PCI_PIN2 ^ pci_port[2].last_pin) & PCMSK2;

	// This is the fastest way I know of to find out which bit is
	// high.  This and the context-switching for the double
	// function call is where most of the time is lost.
	bit = 0;
	while (changed >>= 1)
	{
		bit++;
	}

	// Call the user's routine:
	pci_port[2].funcs[bit]();

	// Save our PINx value for the next compare
	pci_port[2].last_pin = PCI_PIN2;
}
#endif // PCI_NUM > 2

#if PCI_NUM > 3
// Only include this if we have more than three pin change interrupts
ISR(SIG_PIN_CHANGE3)
{
	// Handle it

	// Find out which I/O pin changed (sorry, this is VERY ugly):
	changed = (PCI_PIN3 ^ pci_port[3].last_pin) & PCMSK3;

	// This is the fastest way I know of to find out which bit is
	// high.  This and the context-switching for the double
	// function call is where most of the time is lost.
	bit = 0;
	while (changed >>= 1)
	{
		bit++;
	}

	// Call the user's routine:
	pci_port[3].funcs[bit]();

	// Save our PINx value for the next compare
	pci_port[3].last_pin = PCI_PIN3;
}
#endif // PCI_NUM > 3



// And that's it for the pin change interrupt stuff!

