// bitfield.h
//
// Tom Benedict


// This is an implementation of the the bitfield-style behavior of 
// the CodeVision compiler in WinAVR.  For more information search 
// AVRFreaks on "bitfield".  Apparently there's a fair bit of 
// disagreement on using this approach.  (By the same token there's
// a fair bit of disagreement between owners of Fords and Chevys, 
// so keep this in mind when reading the threads on AVRFreaks.)

// In use you'd do something like this:
//
//	_PORTD.B7 = _PINC.B2;
//
// This sets bit 7 on PORTD to the value of bit 2 on PINC.
// Not as clean as the CodeVision implementation, but it works.
//
// Some other examples:
//
// _PORTD.B1 = 1;	// Set an output bit to true
//
// _PORTD.B1 = 0;	// Set an output bit to false
//
// _PORTD.B1 ^= 1;	// Toggle an output bit
//
// i = _PINC.B2;	// Assign a variable the value of an input pin
//
// You get the general idea.  It lets each bit in the register 
// behave like a variable (because it is!)
//
// It's beyond the scope of this example to discuss data direction 
// registers, but the upshot is for a PORT.B? to be used as an 
// output, the corresponding data direction register bit needs to be 
// set to 1 (DDR.B? = 1).  Likewise, to use it as an input it needs 
// to be set to zero (DDR.B? = 0).  Outputs are handled through PORT? 
// registers, inputs are handled through PIN? registers.
//
// In case you're reading this and thinking, "That's the neatest
// thing since sliced bread!" you're in good company.  I thought
// much the same.  Then I found out that bitfield implementation
// is highly compiler-dependent.  This works in WinAVR.  I believe
// it should work in stock GCC as well, but I haven't tested it.
// By all means use it.  But if you look at the library code for
// Orangutan-lib, you'll find no internal use of bitfields.  That's
// to make it easier to fix in case WinAVR or GCC change in the
// future and make it so bitfields don't work.
//
// But while it works I'll likely use it in my own code.
//
// So let's get to it!

#ifndef _BITFIELD_
#define _BITFIELD_

#include <avr/io.h>


// Now for the bitfield stuff.  Here's how to do it:  Define a 
// typedef struct that has one bit for each bit in the byte 
// (eight bits):

typedef struct
   {
      uint8_t B0 : 1;
      uint8_t B1 : 1;
      uint8_t B2 : 1;
      uint8_t B3 : 1;
      uint8_t B4 : 1;
      uint8_t B5 : 1;
      uint8_t B6 : 1;
      uint8_t B7 : 1;
   } bitfield;

// The way this is done, the whole struct only takes up one byte in memory.
// This is important!  Since the way you make this work is to typecast each
// register as a variable of our new type.  Here's how it would be done:
//
// #define _PORTB (*((volatile bitfield*)&PORTB))
// #define _PINB  (*((volatile bitfield*)&PINB))
// #define _DDRB  (*((volatile bitfield*)&DDRB))
// 
// The #define shorthand means every place you put _PORTB in your code,
// the corresponding definition shows up.  It doesn't take up any
// real variable space to have it living in your code, and it doesn't
// suffer any time penalties for making a subroutine call.  When possible,
// this is how I try to do stuff.
//
// Since different AVR chips have different registers available, this is
// a generic way to make sure we don't miss any, and don't define any for
// registers that don't exist:  PORTx is only defined if you're compiling
// for a device that actually has that I/O port available:

#ifdef PORTA
#define _PORTA (*((volatile bitfield*)&PORTA))
#define _PINA  (*((volatile bitfield*)&PINA))
#define _DDRA  (*((volatile bitfield*)&DDRA))
#endif // PORTA

#ifdef PORTB
#define _PORTB (*((volatile bitfield*)&PORTB))
#define _PINB  (*((volatile bitfield*)&PINB))
#define _DDRB  (*((volatile bitfield*)&DDRB))
#endif // PORTB

#ifdef PORTC
#define _PORTC (*((volatile bitfield*)&PORTC))
#define _PINC  (*((volatile bitfield*)&PINC))
#define _DDRC  (*((volatile bitfield*)&DDRC))
#endif // PORTC

#ifdef PORTD
#define _PORTD (*((volatile bitfield*)&PORTD))
#define _PIND  (*((volatile bitfield*)&PIND))
#define _DDRD  (*((volatile bitfield*)&DDRD))
#endif // PORTD

#ifdef PORTE
#define _PORTE (*((volatile bitfield*)&PORTE))
#define _PINE  (*((volatile bitfield*)&PINE))
#define _DDRE  (*((volatile bitfield*)&DDRE))
#endif // PORTE

// That really is it for the bitfield stuff.

#endif // _BITFIELD_
