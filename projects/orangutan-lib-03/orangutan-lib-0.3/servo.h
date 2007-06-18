// servo-only
//
// Tom Benedict


// One easy source of gear motors for robotics projects is the 
// multitude of servos designed for radio controlled airplanes,
// boats, and cars.  Because of the high volume of production,
// RC servos are typically inexpensive, are available with a wide
// variety of torques, speeds, and sizes, and almost all use the 
// same one-pin interface.
//
// For a robotics project involving a great number of servos, by
// far the most cost-effective method of driving them is to get a
// commercial serial servo controller.  They are inexpensive,
// dedicated pieces of hardware that can drive eight, sixteen, and
// in at least one case, thirty two servos at once.
//
// But for small robotics projects, needing only a handful of servos,
// it's almost always easier and cheaper to include a little more 
// code and just drive the servos off of the existing microcontroller.
//
// The code presented here is not the cleanest or the prettiest way
// to drive servos with a microprocessor.  In the case of a dedicated
// device, the programmer typically knows what pin each servo is going
// to be connected to, so a number of short-cuts can be taken to make
// the code smaller, faster, and better.
//
// This code started off as an example posted to the AVRFreaks forums,
// which was then pushed, pulled, and twisted into something a lot more
// generic, and, unfortunately, a lot harder to read.
//
// Of the example code, this one is likely to be the least readable,
// and the least likely to be copied, pasted, and used somewhere 
// else.  If you're interested in writing a robot program to drive 
// servos, do yourself a favor and just use the servo.c and servo.h 
// code from Orangutan-lib.  It's the same code as is presented here,
// but that way you won't have to look at it.
//
// All that aside, it does work.  So let's get to it!
//
// Tom Benedict

#ifndef _SERVO_
#define _SERVO_

// The original code assumed you'd dedicate an entire port to servo
// operation.  If you're building a device around an AVR processor,
// this is the most sane approach to take.  If you're using the 
// Baby-Orangutan, this is still a sane approach to take since the
// entire compliment of I/O pins from both PORTD and PORTC are brought
// out to headers.
//
// But on the Orangutan the I/O pins brought out to headers span
// several I/O registers, and with the exception of PC0-PC3, aren't
// really grouped in contiguous blocks.  Since PC0-PC3 also correspond
// to ADC0 - ADC3, tying up analog lines for servo operation didn't
// sound like a reasonable approach to take.
//
// So the code was changed to make servo assignment more generic,
// similar to how Procyon AVRlib does things.  You register a servo
// with the servo system, defining I/O pin, port registers, etc.
// At that point it's serviced by the servo system, and you can set
// its position on the fly.



// Maximum number of servos (don't exceed eight).  This can be
// expanded by also using TIMER1B, but if you're trying to drive
// sixteen servos, you're better off getting a serial servo
// controller!

#define MAX_SERVOS    8

// Servo timing values in milliseconds.  This is Futaba timing, and
// can differ from one manufacturer to the next.  1ms (1000 us) to 2ms
// (2000us) is typical.

#define SERVO_MIN    920 
#define SERVO_MAX   2120 
#define SERVO_MID   (SERVO_MIN + SERVO_MAX) / 2

// Time between servo pulses, also in microseconds.  20ms, or
// 20000us, is typical, and is what's used here.
 
#define SERVO_FRAME 20000 //(50Hz)


// Servo Routines


// servo_init()
//
// Initializes the servo subsystem.  The idea here is to set up 
// TIMER1A to run at full CPU speed and to set an interrupt on output
// compare.
//
// The routine also goes through the array of servo data, makes sure
// they're all set to be inactive, sets their default positions to
// midline (a safety measure), and sets their bitmask to zero to make
// sure that even if something goes awry and an undefined servo begins
// to be serviced, it doesn't actually do anything.

void servo_init(void);


// servo_define()
//
// This is a fairly simple routine that's unfortunately somewhat
// convoluted to use.  The idea is to pass it the data direction
// register, output port register, and bit number for a servo,
// and to get back a number that represents the servo for all 
// the other servo routines.  
//
// The unfortunate part is that you can't simply hand DDR and
// PORT registers to other routines and have things work out
// right.  To do that you need to use an AVR command called
// _SFR_IO_ADDR() to reference each of the registers.
//
// For example, to define two servos on PD6 and PD7, called 
// servo_left and servo_right respectively, you'd do:
//
//		uint8_t left, right;
//
//		left = servo_define(_SFR_IO_ADDR(DDRD), _SFR_IO_ADDR(PORTD), 6);
//		right = servo_define(_SFR_IO_ADDR(DDRD), _SFR_IO_ADDR(PORTD), 7);
//
// The nice thing is, once that's done you can use all the other
// servo routines using the servo_left and servo_right variables:
//
//		servo_set(left, 1500);
//		servo_set(right, 2000);
//
// You get the idea...

uint8_t servo_define(uint8_t ddr, uint8_t port, uint8_t bit);


// servo_active() / servo_inactive()
//
// Once they're defined, servos start off life inactive.  You need
// to turn them on using servo_active().  Once on, they can be 
// turned back off by using servo_inactive().
//
// The reason for this is that many of the servo applications in
// robotics tend to leave servos off for reasons of power management
// or to allow servos modified for continuous rotation to be able to
// coast.  If it's not a feature you're interested in, define the
// servo, activate it, and forget about it.

void servo_active(uint8_t servo_num);

void servo_inactive(uint8_t servo_num);


// servo_set()
//
// servo_set() assigns a pulse length to a given servo that you've
// already defined.  This time is in microseconds, and must lie
// between SERVO_MIN and SERVO_MAX, above.
//
// If you want to set up some routine to do scaling, say from
// -100 to 100, and pass the appropriate value to servo_set(), by
// all means do so.  For servos modified for continuous rotation,
// this is a good way to go.  It's also a good way to go for servos
// that need to be able to be positioned in degrees.  Or radians.
// Or binary radians.  Or if you have a servo turning a leadscrew,
// you may want a routine that scales to linear units of measure.
//
// But you get the idea...  Servos talk microseconds of pulse
// duration.  How you use that is up to you.

void servo_set(uint8_t servo_num, uint16_t time);

#endif // _SERVO_
