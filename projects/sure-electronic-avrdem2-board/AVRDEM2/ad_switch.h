// ad_switch.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_SWITCH

// SW_OPEN and SW_CLOSED may be used to recognized debounced states (sw1 and sw2)
#define SW_OPEN   0xff          // debounced and open
#define SW_CLOSED 0x00          // debounced and closed
#define SW_C2O       1          // closed to open transition
#define SW_O2C       2          // open to closed transition

extern volatile char sw1, sw2;           // global debounce state
extern volatile char sw1_chg, sw2_chg;   // state changed? (0 = no)
extern volatile char sw1_last, sw2_last; // previous stable state

// ----------- function prototypes
void init_switch(void);       // initialize switches
