// ad_switch.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// sw1 corresponds to the switch on C7, sw2 to C6.
// the board has external pullup on these lines.
// sw# will be 0x00 if the switch is closed and stable,
// 0xff if the switch is open and stable, and otherwise
// the switch is in transition.
//
// when the state changes, sw#_chg will be set nonzero:
// 1 means the switch opened, 2 if the switch closed.
// the user may zero these variables to acknowledge the
// transition.

#include "ad_switch.h"

volatile char sw1, sw2;           // global debounce state
volatile char sw1_chg, sw2_chg;   // state changed? (0 = no)
volatile char sw1_last, sw2_last; // previous stable state

// -------------------------------------------------------------
// init_switch() - initialize switches

void init_switch(void) {
  char j;

  DDRC &= 0x3f;         // C6 and C7 are inputs
  PORTC &= 0x3f;        // board has external pull-ups

  // give an "unstable" value to the debounce variables
  // we want bit 0 to be the opposite of what we read
  j = PINC;             // read the switches
  sw1 = (j & 0x80) ? 0xaa : 0x55;    // pin 7 = switch 1
  sw2 = (j & 0x40) ? 0xaa : 0x55;    // pin 6 = switch 2
  sw1_chg = sw2_chg = 0;             // state hasn't changed
  sw1_last = sw2_last = SW_OPEN;     // assume open on startup
}
