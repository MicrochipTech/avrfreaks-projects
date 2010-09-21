// ad_leds.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// this maintains the bits indicating the leds' states
// the actual writing is multiplexed in the interrupt handler

#include "ad_leds.h"

char sa_led;            // bits to write to PORTA lower
char led_mask;          // mask for interrupt handler
char led_cnt;           // counter for interrupt handler

// -------------------------------------------------------------
// leds(who, what) - turn individual LEDs on/off
//
// who - LED number 1 (top) to 4 (bottom), 0 for all
// what - 0 for off, 1 for on, 2 for toggle (LED_ON, LED_OFF, LED_TOG)

void leds(char who, char what) {
  char j1, j2, i, mask;

  if (who < 1 || who > 4) {
    j1 = 1;                       // do all of them
    j2 = 4;
  } else {
    j1 = j2 = who;                // do only one
  }

  for (i = j1; i <= j2; i++) {
    mask = 0x10 >> i;             // LED #1 is bit 3, #4 is bit 0
    if (what == 1) {
      sa_led &= ~mask;            // clear bit (LED on)
    } else if (what == 2) {
      sa_led ^= mask;             // toggle the bit
    } else {
      sa_led |= mask;             // set bit (LED off)
    }
  }
}

// -------------------------------------------------------------
// get_led(who) - return LED state
//
// who - LED number (1 = top, 4 = bottom)
// returns: 0 if off, 1 if on

char get_led(char who) {
  char rtn = 0;

  if (who > 0 && who < 5) {
    rtn = (sa_led & (0x10 >> who)) ? 0 : 1;
  }
  return rtn;
}

// -------------------------------------------------------------
// init_leds() - initialize LEDs

void init_leds(void) {
  DDRA = (DDRA & 0xf0) | 0x0f;          // bits 0-3 are output
  PORTA = (PORTA & 0xf0) | 0x0f;        // all off (1 = off)
  sa_led = 0x0f;
  led_cnt = 0;                          // led interrupt counter
  led_mask = 0x01;                      // gotta start somewhere
}
