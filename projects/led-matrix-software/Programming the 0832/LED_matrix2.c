//      LED_matrix2 - test routine for the SURE LED matrix
//      multiple board version, horizontal layout
//
//	boards are: CS0 -> CS1 -> CS2 -> CS3, CS0 is master
//
//      the definitions in dm2.h define the wiring:
//
//      portA:
//        bit 0 - CS0 (master)
//        bit 1 - RD
//        bit 2 - WR
//        bit 3 - data
//        bit 4 - CS1
//        bit 5 - CS2
//        bit 6 - CS3
//        bit 7 - available

#include <iom16v.h>             // device (ATMega16) definitions
#include <AVRdef.h>             // ICCAVR macros
#include "dm2.h"                // header for display matrix routines
#include "dm_chars.h"           // header for display character routines

//      prototypes
char reverse(char);             // reverse bit order
void init(void);                // general initialization
void wait(void);                // burn time

//      demo routines
void outline(void);             // outline 4 boards
void zigzag(void);              // slo-mo zigzag
void monofont(void);            // monospaced font
void proportional(void);        // proportional font
void block_write(void);         // write array of bytes
void active(void);              // vary active boards

volatile unsigned int pause;    // delay counter (ms to kill)

// ------------------------------------------------------------
//      main() - mainline

void main(void) {

  init();                       // initialize timers, etc.
  pause = 100;                  // 0.1 seconds
  wait();                       // wait until it passes

  while (1) {
    dm_init(4);                 // initialize 4 boards
    dm_blankall();              // and blank them

    outline();                  // draw an outline
    zigzag();                   // slo-mo zigzag
    monofont();                 // monospaced font message
    proportional();             // proportional font message
    block_write();              // block write
    active();                   // vary active boards
  }
}

// ------------------------------------------------------------
//      outline() - draw an outline around 4 boards
//
//      using fill and column writes, draw an outline

void outline(void) {

  dm_fill(1, 126, 0x81);        // fill 126 columns
  dm_wcol(0, 0xff);             // write the left column
  dm_wcol(127, 0xff);           // and right
  pause = 1000;
  wait();
}

// ------------------------------------------------------------
//      zigzag() - draw a moving zigzag
//
//      draw and erase a zigzag line using the point
//      plotting routines

void zigzag(void) {
  char pass, y, add, i;

  for (pass = 1; pass < 3; pass++) {    // draw, then erase
    y = 1;
    add = 1;                            // going up or down
    for (i = 1; i < 127; i++) {         // cols 1-126
      pause = 20;
      wait();
      dm_wbit(i, y, pass & 0x01);       // set or clear
      y = y + add;
      if (y == 7) {                     // reach top?
        y = 5;                          // yup, start down
        add = 255;                      // -1
      } else if (y == 0) {              // reach bottom?
        y = 2;                          // yup, start up
        add = 1;                        // +1
      }
    }
  }
  pause = 1000;
  wait();
  dm_blankall();
}

// ------------------------------------------------------------
//      monofont() - monospaced font message
//
//      write a message at various places across the display

void monofont(void) {
  char i, *msg = "Mono font";

  for (i = 0; i < 128; i += 13) {       // some random spots
    dm_str(i, msg, 0x09);               // fixed font, pad=1
    pause = 600;
    wait();
    dm_blankall();                      // blank old
  }
  pause = 400;
  wait();
}

// ------------------------------------------------------------
//      proportional() - proportional font message
//
//      while blinking, write a message and scroll it
//      right and left

void proportional(void) {
  char i, *msg = "Proportional font";
  int cnt;

  cnt = dm_str(0, msg, 0x41);           // blank=4, pad=1
  pause = 600;
  wait();

  dm_blink(1);                          // start blinking
  cnt &= 0x7f;                          // drop error flags
  cnt = 127 - cnt;                      // number of places to shift

  for (i = 0; i < cnt; i++) {           // scoot it to the right
    dm_rscroll(0);
    pause = 60;
    wait();
  }

  for (i = 0; i < cnt; i++) {           // scoot it to the left
    dm_lscroll(0);
    pause = 60;
    wait();
  }

  pause = 400;
  wait();
  dm_blink(0);                          // turn off blinking
  dm_blankall();
}

// ------------------------------------------------------------
//      block_write() - write array of bytes

void block_write(void) {
  char add, w, w2, skip, blk[128], i;
  char use[] = { 0x18, 0x24, 0x42, 0x81 };  // < symbol
  char use2[] = { 0x81, 0x42, 0x24, 0x18 }; // > symbol
  char c1, msk, lng;

  add = 0;                              // space between symbols
  w = 0;                                // next column of <
  w2 = 3;                               // next column of >
  skip = 0;                             // blanks to insert

  for (i = 0; i < 64; i++) {            // half a display
    if (skip) {                         // between symbols?
      blk[i] = 0;                       // yes - blanks
      blk[127 - i] = 0;
      skip--;
    } else {                            // no - part of symbol
      blk[i] = use[w++];                // left side of display
      blk[127 - i] = use2[w2--];        // right side of display
    }
    if (w > 3) {                        // done with symbol?
      w = 0;                            // yes - start over
      w2 = 3;
      skip = add++;                     // # of blanks between them
    }
  }
  dm_wbytes(0, blk, 128);               // write full display
  pause = 2000;
  wait();

// we're going to progressively erase (fill) the middle of the display
  c1 = 50;                              // starting column
  msk = 1;                              // bottom LED on
  for (i = 0; i < 8; i++) {             // do 8 LEDs
    lng = (64 - c1) * 2;                // how wide the fill will be
    dm_fill(c1, lng, msk);              // fill it
    pause = 150;
    wait();
    msk += msk;                         // move LED up one row
    c1 -= 5;                            // and move starting point left
  }

  pause = 1000;
  wait();

  dm_rbytes(0, blk, 128);               // read full display
  for (i = 0; i < 128; i++)             // flip it vertically
    blk[i] = reverse(blk[i]);
  dm_wbytes(0, blk, 128);               // and rewrite it
  pause = 1000;
  wait();

  for (i = 0; i < 128; i++)             // now invert everything
    blk[i] ^= 0xff;                     // toggle all the bits
  dm_wbytes(0, blk, 128);               // and rewrite it
  pause = 800;
  wait();

  for (i = 8; i < 16; i++) {            // take intensity up full
    pause = 400;
    wait();
    dm_setpwm(i);
  }

  for (i = 14; i; i--) {                // and back down
    pause = 400;
    wait();
    dm_setpwm(i);
  }

  for (i = 0; i < 8; i++) {             // and back up half way
    pause = 400;
    wait();
    dm_setpwm(i);
  }
  pause = 1000;
  wait();
  dm_blankall();
}

// ------------------------------------------------------------
//      reverse(cc) - reverse the bit order for a byte
//
//      cc - incoming bit pattern
//      returns: inverted bit pattern

char reverse(char cc) {
  char i, msk, r;
  unsigned char msk2;

  msk = 0x01;
  msk2 = 0x80;
  r = 0;

  for (i = 0; i < 8; i++) {     // 8 bits in a byte
    if (msk & cc) r |= msk2;
    msk <<= 1;
    msk2 >>= 1;
  }
  return r;
}

// ------------------------------------------------------------
//      active() - vary the number of active boards
//
//      this shows how to put out characters manually
//      (as opposed to using dm_char() or dm_str())

void active(void) {
  char buf[7], *p, c, cnt, cc, pcnt, n;
  char msg[] = "  4 active boards";

  for (c = 0; c < 7; c++) buf[c] = 0;
  pcnt = 4;                     // number of active boards
  p = msg;                      // start at beginning of msg
  c = 0;                        // next column out
  cnt = 0;                      // message counter

  while (1) {                   // repeat count depends on message
    pause = 60;
    wait();                     // wait a while

    if (!c) {                   // are we done with prev char?
      cc = *p++;                // get next character out
      if (!cc) {                // are we done with message?
        p = msg;                // yes - start at beginning again
        cc = *p++;              // get first character
        cnt++;                  // count number of messages put out
        if (cnt > 1) {          // reach our preset limit?
          cnt = 0;              // yup - start over
          pcnt--;               // reduce board count
          if (!pcnt) break;     // if 1 --> 0, we're done
          dm_init(pcnt);        // initialize with reduced boards
          msg[2] = '0' + pcnt;  // insert board count into message
          if (pcnt == 1) msg[16] = 0; // for 1, drop plural
        }
      }
      n = dm_cload(cc, buf, 2);         // load up character definition
      if (cc == 32) n = 4; else n += 1; // blanks = 4, pad = 1
    }
    dm_lscroll(buf[c++]);    // put next column out

    if (c >= n) c = 0;          // last column out? start over
  }

  pause = 1000;
  wait();
  dm_init(4);                   // back to full complement of boards
  dm_blankall();
}

// ------------------------------------------------------------
//      wait() - burn time
//
//      this hangs until pause transitions from 1 to 0
//
//      to delay, set pause to the approximate number of milliseconds,
//        then call this
//      note: if you call this without setting pause, you die

void wait(void) {
  while (pause != 1) ;          // hang until pause = 1
  while (pause) ;               // hang until pause = 0
}

// ------------------------------------------------------------
//      timer0 interrupts - about 1 KHz

#pragma interrupt_handler timer0_comp_isr:iv_TIM0_COMP
void timer0_comp_isr(void)
{
  if (pause) pause--;
}

// ------------------------------------------------------------
//      init() - general initialization
void init(void) {
  CLI();
  UCSRB = 0x00;                 // disable while setting baud rate
  UCSRA = 0x00;
  UCSRC = BIT(URSEL) | 0x06;
  UBRRL = 0x19;                 // set baud rate lo
  UBRRH = 0x00;                 // set baud rate hi
  UCSRB = 0x18;

  TCCR0 = 0x00;                 // stop
  TCNT0 = 0xE1;                 // set count (about 1KHz)
  OCR0  = 0x1F;                 // set compare
  TCCR0 = 0x0C;                 // start timer

  MCUCR = 0x00;
  GICR  = 0x00;
  TIMSK = 0x02;                 // timer interrupt sources
  SEI();                        // re-enable interrupts
}
