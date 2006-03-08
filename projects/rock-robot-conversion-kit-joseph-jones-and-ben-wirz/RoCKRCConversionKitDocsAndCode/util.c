/* util.c -- math and other utility functions */

#include <io8535.h>
#include <macros.h>
#include <stdlib.h>			  // ABS is here...
#include "defines.h"

// Code saving function, it saves ~4 bytes per use over in-line code
// Returns TRUE if current time exceeds start time by more than local_duration
unsigned char time_out(unsigned int start_time, unsigned int local_duration) {
  extern unsigned int  clock;
  return ((clock - start_time) > local_duration);
  }

// Limit a value to the range between two specified extremes
extern int clip (int value, int low, int high) {
  if (value < low)
    return low;
  else if (value > high)
    return high;
  else
    return value;
}

// Cosp is actually a sawtooth-shaped function rather than a cosine.
// The range and domain of cosp are both [0,256].  
// Examples: cosp(0)=256, cosp(64)=128, cosp(128)=0, cosp(192)=128
int cosp (int th) { //unsigned char th) {  <- doesn't reduce space (?)
  if (th < 129)
    return (256 - (th << 1));
  else
    return ((th << 1) - 256);
	}
	
// Sinp is just cosp shifted over by 90 degrees.  90 => 64 theta units
int sinp(int th) { //unsigned char th) {  <- doesn't reduce space (?)
  return cosp(0xff&(th - 64)); }

// Return the argument with the largest magnitude
int max_mag(int a, int b) {
  if (abs(a) > abs(b))
    return a;
  else
    return b;
}

//int max(int a, int b) {
//  return (abs(max_mag(a, b)));
//  }
  

// Value is in the range (0, in_range).  Convert value to a scaled 
// number in the range (0, out_range).  Use longs to avoid thinking
// about grungy overflow/underflow issues
// The AL2 device enables correct rounding.  The expression:
//    v = (x + y/2)/y rounds correctly for positive numbers.
int scale (int value, int in_range, int out_range) {
	long aL = (long) value;
	long AL = (long) in_range; 
//	long AL2 = (long) (in_range >> 1);	// Setup for rounding division
	long BL = (long) out_range;
//	return (int) (  ((aL * BL) + AL2) / AL );  
    return (int) (   (aL * BL) / AL );	// Do roundoff wrong to save ~42 bytes
  }

/* This is takes more space than the above function:
int scale (int value, int in_range, int out_range) {
	long aL = (long) value;
	long AL = (long) in_range; 
	long AL2 = (long) (in_range >> 1);	// Setup for rounding division
	long BL = (long) out_range;
//  return (int) ( ( (long) value * (long) out_range ) / in_range );
	return (int) (  ((( (long) value) * ((long) out_range)) + (long) (in_range >> 1)) / ((long) in_range) );  
  }
  
*/

// Mat_scale used to scale by subtracting 128 from a, b, c.  Now
// a, b, c, are signed coming from p_arr[] which is now a union.  
//   =>   return ((((a-128)*l) + ((b-128)*r) + c-128)<<2)
// The natural output is in the range [-64,64] (always a velocity)
// So, finally mat_scale mutiplies by 4 to get vel in range [-256, 256]
int mat_scale(signed char l, signed char r,
			  signed char a, signed char b, signed char c) {
			  /*
			  t1 = ((int)a*(int)l);
			  t2 = ((int)b*(int)r);
			  t3 = (int)c;
			  t1 = t1 + t2 + t3;
			  t1 = t1 << 2;
			  */
  return ((((int)a*(int)l) + ((int)b*(int)r) + (int)c)<<2);
  }
  
  
extern union { unsigned char u; signed char s;} p_arr[PARAM_MAX]; // from acquire.c

// Because of timing issues EEPROM writing must be done by the chip
// eewriter looks in some parameters of a high address, low address,
// data and an enable flag.  When the host sets the flag, eewriter
// writes data to the EEPROM and clears the flag.

// EEwriter parameters
//unsigned char pee_high_addr;
//unsigned char pee_low_addr;
//unsigned char pee_data;
//unsigned char pee_flag;

void eewriter(void) {
  //extern unsigned char p_arr[];
  int addr;
  if (pee_flag) {
    addr = (pee_low_addr + (pee_high_addr << 8));
    EEPROMwrite(addr,pee_data);
	pee_flag = 0;			   // Clear flag so it only writes once
	}
}

// Signum function returning unsigned char
// arg > 0 => 1, arg = 0 =>0, arg < 0 => -1
signed char sig (int arg) {
  if (arg > 0)
  	 return 1;	   // arg > 0 => 1
  if (arg < 0)
     return -1;	   // arg < 0 => -1
  return 0;	   // arg = 0 => 0
  }

  
// Activate only one of the LEDs
void led(unsigned char mask, unsigned char on_off) {
  if (on_off) bit_off(PORTC, mask);
  else        bit_on(PORTC, mask);
  }

// Activate all LEDs
// Reverse order of bits to match LED wiring order (better to change hardware)
void leds(unsigned char disp) {	  // Display the number disp on the LEDs
   led(8,(disp & 1));
   led(4,(disp & 2));
   led(2,(disp & 4));
   led(1,(disp & 8));
//  PORTC = ~disp & 0xF;		  	  // Turn LEDs on with low output
  }
