// ad_lcd.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// please note that this code will hang if the display is not plugged in!


#include "ad_lcd.h"

// Port B (the data port) is shared with the 7 segment displays,
// so we may need to play some tricks with it. they are interrupt
// driven, so we'll suspend the interrupts while we play.
//
// Port D bits used by lcd:
//   bit 2 - LCD - RS
//   bit 3 - LCD - R/W
//   bit 4 - LCD - E
//   bit 5 - LCD contrast
//
// these functions mostly follow the standard HD44780 command
// set. you should refer to a datasheet (the Hitachi one is
// recommended) for more detail.
//
// note that the automatic incrementing and decrementing of
// the memory addresses is not controlled in these routines.
// if you set the lcd to decrement, your accesses to the data
// and graphics memories may not be what you expect. being a
// computer, it will most likely do exactly what you tell it to.
//
// local routines
void putlcd(char, char);        // write lcd primitive
char getlcd(char);              // read lcd primitive

char lcd7;                      // if nonzero, protect port B
char lcd_laddr;                 // last address returned in busy check

// -------------------------------------------------------------
// lcd_put(row, col, what) - write byte to the display
//
// row - 0 for top row, 1 for bottom row
// col - 0 to 19
// what - character to write
//
// note: this won't write off the display. use the two
// direct routines (lcd_setaddr and lcd_next) for writing
// to the non-displayed memory

void lcd_put(char r, char c, char w) {
  char addr;

  addr = r ? 0x40 : 0x00;       // base address
  if (c < 20) addr += c;        // add column offset
  lcd_setaddr(LC_DD, addr);     // set the address

  lcd_next(w);                  // write the byte
}

// -------------------------------------------------------------
// lcd_gput(char, *pattern) - write user defined pattern
//
// char    - character being defined (0 - 7)
// pattern - pointer to 7 bytes of data (low 5 bits display)
//
// this writes 8 bytes (7 user bytes and a 0) to the
// graphics memory. for more complete control, use the
// lcd_setaddr and lcd_next functions directly.
//
// character 0 may be displayed by using 0 or 9,
// character 1 may be displayed by using 1 or 10, etc.

void lcd_gput(char cc, char *w) {
  char addr, i;

  addr = (cc & 0x07) << 3;      // address
  lcd_setaddr(L_CG, addr);      // set the address
  for (i = 0; i < 7; i++) {
    lcd_next(*w++);             // write their 7 bytes
  }
  lcd_next(0);                  // and a byte of zeros
}

// -------------------------------------------------------------
// lcd_str(row, col, &what) - write string to lcd data mem
//
// row - 0 for top row, 1 for bottom row
// col - 0 to 19
// what - address of null terminated string to write
//
// this will write beyond the displayed memory, although
// perhaps not as you would like. if you want direct access
// to the DDRAM, use lcd_setaddr() and lcd_next()

void lcd_str(char r, char c, char *w) {
  char cc, addr;

  if (cc = *w++) {
    lcd_put(r, c, cc);
    while (cc = *w++) lcd_next(cc);
  }
}

// -------------------------------------------------------------
// lcd_next(what) - write to lcd at next address
//
// what - character to write

void lcd_next(char w) {
  while (lcd_busy()) ;
  putlcd(L_RS1, w);
}

// -------------------------------------------------------------
// lcd_read() - read a byte from lcd memory
//
// returns - read data
//
// this will read from either CGRAM or DDRAM depending on
// which is active (from last call to lcd_setaddr)

char lcd_read(void) {
  while (lcd_busy()) ;
  return (getlcd(L_RS1));
}

// -------------------------------------------------------------
// lcd_home() - home the lcd cursor

void lcd_home(void) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_HOME);
}

// -------------------------------------------------------------
// lcd_clear() - clear display

void lcd_clear(void) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_CLEAR);
}

// -------------------------------------------------------------
// lcd_emode(id, s) - set entry mode
//
// id - L_INC to increment, L_DEC to decrement
// s  - shift (L_ACCOMP or L_STAY)

void lcd_emode(char id, char s) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_EMSET | id | s);
}

// -------------------------------------------------------------
// lcd_display(disp, curs, blink) - display params
//
// disp  - display (L_DOFF or L_DON)
// curs  - cursor (L_COFF or L_CON)
// blink - blink (L_BOFF or L_BON)

void lcd_display(char d, char c, char b) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_DISP | d | c | b);
}

// -------------------------------------------------------------
// lcd_shift(who, where) - shift cursor or display
//
// who   - display (L_DSH) or cursor (L_CSH)
// where - left (L_LEFT) or right (L_RGHT)

void lcd_shift(char who, char wh) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_CURS | who | wh);
}

// -------------------------------------------------------------
// lcd_funset(lines, font) - lcd function set
//
// lines - L_ONE for 1 line, L_TWO for 2 lines
// font  - L_5x8 for 5x8, L_5x10 for 5x10

void lcd_funset(char n, char f) {
  while (lcd_busy()) ;
  putlcd(L_RS0, LC_FSET | n | f);
}

// -------------------------------------------------------------
// lcd_setaddr(mode, addr) - set lcd address
//
// mode - L_CG for CGRAM, L_DD for DDRAM
// addr - 6 bit for CGRAM (0-63), 7 bit for DDRAM (0-79)
//
// once an address is set in one of the memories, that
// memory will be the one used for subsequent reads and
// writes. the address seen by the most recent busy check
// (which is stored in lcd_laddr) will depend on which
// memory is active.

void lcd_setaddr(char mode, char addr) {
  while (lcd_busy()) ;

  if (mode == L_CG) {
    addr = (addr & 0x3f) | LC_CG;    // keep 6 bits
  } else {
    addr = (addr & 0x7f) | LC_DD;    // keep 7 bits
  }
  putlcd(L_RS0, addr);
}

// -------------------------------------------------------------
// init_lcd(s7) - initialize the LCD
//
// s7 - if zero, we have port B to ourselves
//    - if one, we have to be tricky
//
// this leaves the display turned off, so be sure to remember
// to turn it on when you use it
//
// this is meant for internal use, but if the user needs to
// call it, use 0 as the argument if the 7 segment display is
// not being used, and 1 as the argument if it is. you may check
// the symbol OK_7SEG to make this determination (see the init()
// function in avrdem.c for an example). it can't be checked here
// because this code might load prior to the 7seg code.

void init_lcd(char s7) {
  volatile unsigned int i;

  DDRD |= 0x3c;         // bits 2-5 outputs
  PORTD &= 0xc3;        // all lcd lines to 0
  lcd7 = s7;            // remember possible conflicts
  // port b is sometimes in, sometimes out

  // we'll do a software reset here by hitting it with
  // a stick as shown in the datasheet

  // wait 15 ms (180,000 instructions)
  for (i = 0; i < 60000; i++) ;
  for (i = 0; i < 60000; i++) ;
  putlcd(L_RS0, 0x30);

  // wait 4.1 ms (49,200 instructions)
  for (i = 0; i < 25000; i++) ;
  putlcd(L_RS0, 0x30);

  // wait 100us (1200 instructions)
  for (i = 0; i < 600; i++) ;
  putlcd(L_RS0, 0x30);

  // now the busy line is working, so use the canned routines
  lcd_funset(L5x8, L_TWO);              // set lines & font
  lcd_display(L_DOFF, L_COFF, L_BOFF);  // display, cursor, blink off
  lcd_clear();                          // clear the display
  lcd_emode(L_INC, L_STAY);             // entry mode set
}

// -------------------------------------------------------------
// getlcd(rs) - primitive to read the lcd
//
// rs - register select (for RS line. RW = 1)
// returns: data
//
// this is for internal use only - the user should not call it
//
// the datasheet says E should be 230ns, which is 3 instructions
// at 12MHz (each is 83ns)

char getlcd(char rs) {
  char holda, holdb, d;

  // if they're using the 7 segment displays, we have to shut
  // them down while we tweak the lcd. also PORTD gets tweaked
  // by the various interrupt handlers, so be safe

  CLI();                        // keep it simple
  if (lcd7) {
    holda = PORTA;              // save the whales
    PORTA &= 0xf0;              // drop the 7 seg drivers
    holdb = PORTB;              // save the dolphins too
  }

  DDRB = 0x00;                  // port B is input for this
  d = PORTD & L_MSK;            // clear RS, RW, and E
  d |= (rs | L_RW1);            // OR in the RS and RW bits
  PORTD = d;                    // get those out there
  PORTD |= L_EON;               // E on
  asm("nop");                   // wait a tad
  asm("nop");
  d = PINB;                     // read the data
  PORTD &= L_MSKE;              // drop E

  if (lcd7) {
    DDRB = 0xff;                // 7segs are all outputs
    PORTB = holdb;              // restore original
    PORTA = holda;              // and restore drivers
  }
  SEI();                        // start it up again
  return d;
}

// -------------------------------------------------------------
// putlcd(rs, val) - primitive to write the lcd
//
// rs  - register select (RS line. RW = 0)
// val - data to write
//
// this is for internal use only - the user should not call it
//
// the datasheet says E should be 230ns, which is 3 instructions
// at 12MHz (each is 83ns)

void putlcd(char rs, char v) {
  char holda, holdb, d;

  // if they're using the 7 segment displays, we have to shut
  // them down while we tweak the lcd. also PORTD gets tweaked
  // by the various interrupt handlers, so be safe

  CLI();                        // keep it simple
  if (lcd7) {
    holda = PORTA;              // save the whales
    PORTA &= 0xf0;              // drop the 7 seg drivers
    holdb = PORTB;              // save the dolphins too
  }

  DDRB = 0xff;                  // all Bs are output
  d = PORTD & L_MSK;            // clear RS, RW, and E
  d |= (rs | L_RW0);            // OR in the RS and RW bits
  PORTD = d;                    // get those out there
  PORTB = v;                    // put the data to write out there
  PORTD |= L_EON;               // E on
  asm("nop");                   // wait a tad
  asm("nop");
  PORTD &= L_MSKE;              // drop E

  if (lcd7) {
    PORTA = holda;              // tidy up
    PORTB = holdb;
  }
  SEI();                        // start it up again
}

// -------------------------------------------------------------
// lcd_busy() - is the lcd busy?
//
// returns: 0 if not busy, 0x80 if not busy
//
// the user probably shouldn't be calling this directly

char lcd_busy(void) {
  char r;

  r = getlcd(L_RS0);            // read the busy bit
  lcd_laddr = r & 0x7f;         // last address (may be data or graphics)
  return (r & 0x80);            // return the busy bit
}
