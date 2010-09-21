// ad_lcd.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_LCD

// ----------- function prototypes
void init_lcd(char);            // initialize LCD
char lcd_busy(void);            // lcd busy?
void lcd_put(char, char, char);    // write byte to lcd
void lcd_gput(char, char *);       // define special char
void lcd_str(char, char, char *);  // write string to lcd
void lcd_next(char);            // write to next memory cell
char lcd_read(void);            // read from next memory cell
void lcd_home(void);            // home the lcd cursor
void lcd_clear(void);           // clear the lcd display
void lcd_emode(char, char);     // set entry mode
void lcd_display(char, char, char);  // display parameters
void lcd_shift(char, char);	// cursor or display shift
void lcd_funset(char, char);    // lcd function set
void lcd_setaddr(char, char);   // set lcd RAM address

extern char lcd_laddr;          // last address returned in busy check

#define L_CG     0		// CGRAM
#define L_DD     1		// DDRAM

#define L_RS0    0x00           // RS = 0
#define L_RS1    0x04           // RS = 1

#define L_RW0    0x00           // RW = 0
#define L_RW1    0x08           // RW = 1

#define L_EON    0x10           // E = 1

#define L_MSKE   0xef           // mask to drop E
#define L_MSK    0xe3           // mask to drop RS, RW, and E

// ----- commands
#define   LC_CLEAR 0x01         // clear
#define   LC_HOME  0x02         // home

#define   LC_EMSET 0x04         // set entry mode
  #define L_ACCOMP 0x01         // -- shift display, not cursor
  #define L_STAY   0x00         // -- don't shift display
  #define L_INC    0x02         // -- incr addr
  #define L_DEC    0x00         // -- decr addr

#define   LC_DISP  0x08         // display parameters
  #define L_CON    0x02         // -- cursor on
  #define L_COFF   0x00         // -- cursor off
  #define L_DON    0x04         // -- display on
  #define L_DOFF   0x00         // -- display off
  #define L_BON    0x01         // -- blink on
  #define L_BOFF   0x00         // -- blink off

#define   LC_CURS  0x10         // -- cursor parameters
  #define L_CSH    0x00         // -- cursor shift
  #define L_DSH    0x80         // -- display shift
  #define L_LEFT   0x00         // -- shift left
  #define L_RGHT   0x04         // -- shift right

#define   LC_FSET  0x30         // function set (8 bit version)
  #define L5x8     0x00         // -- 5x8 display
  #define L5x10    0x04         // -- 5x10 display
  #define L_ONE    0x00         // -- one line display
  #define L_TWO    0x08         // -- two line display

#define   LC_CG    0x40         // set CG address
#define   LC_DD    0x80         // set DD address
