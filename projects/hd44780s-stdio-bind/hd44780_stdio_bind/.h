/*
 * Title    :   HD44780U LCD library
 * Authors:   
 * Based on Volker Oth's lcd library (http://members.xoom.com/volkeroth)
 * modified by Peter Fleury's (http://jump.to/fleury). Flexible pin
 * configuration by Markus Ermert. Adapted for the linuxfocus LCD display
 * by Guido Socher.Binded to be used with stdio.h by Fandi Gunawan
 * using lcd_putc_stream
 *
 * Software:  AVR-GCC with AVR-AS (old and new is OK since there is avr_compat.h)
 * Target:    any AVR device
 * Copyright: GPL
 */

#ifndef HD44780_H
#define HD44780_H
#include <avr/pgmspace.h>

/* you shouldn't need to change anything below this line */

typedef unsigned char  u08;
typedef unsigned short u16;


/* instruction register bit positions */
#define LCD_CLR             0      /* DB0: clear display               */
#define LCD_HOME            1      /* DB1: return to home position     */
#define LCD_ENTRY_MODE      2      /* DB2: set entry mode              */
#define LCD_ENTRY_INC       1      /* DB1: 1=increment, 0=decrement    */
#define LCD_ENTRY_SHIFT     0      /* DB2: 1=display shift on          */
#define LCD_ON              3      /* DB3: turn lcd/cursor on          */
#define LCD_ON_DISPLAY      2      /* DB2: turn display on             */
#define LCD_ON_CURSOR       1      /* DB1: turn cursor on              */
#define LCD_ON_BLINK        0      /* DB0: blinking cursor ?           */
#define LCD_MOVE            4      /* DB4: move cursor/display         */
#define LCD_MOVE_DISP       3      /* DB3: move display (0-> cursor)?  */
#define LCD_MOVE_RIGHT      2      /* DB2: move right (0-> left) ?     */
#define LCD_FUNCTION        5      /* DB5: function set                */
#define LCD_FUNCTION_8BIT   4      /* DB4: set 8BIT mode (0->4BIT mode)*/
#define LCD_FUNCTION_2LINES 3      /* DB3: two lines (0->one line)     */
#define LCD_FUNCTION_10DOTS 2      /* DB2: 5x10 font (0->5x7 font)     */
#define LCD_CGRAM           6      /* DB6: set CG RAM address          */
#define LCD_DDRAM           7      /* DB7: set DD RAM address          */
#define LCD_BUSY            7      /* DB7: LCD is busy                 */

/* set entry mode: display shift on/off, dec/inc cursor move direction */
#define LCD_ENTRY_DEC            0x04   /* display shift off, dec cursor move dir */
#define LCD_ENTRY_DEC_SHIFT      0x05   /* display shift on,  dec cursor move dir */
#define LCD_ENTRY_INC_           0x06   /* display shift off, inc cursor move dir */
#define LCD_ENTRY_INC_SHIFT      0x07   /* display shift on,  inc cursor move dir */

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF             0x08   /* display off                            */
#define LCD_DISP_ON              0x0C   /* display on, cursor off                 */
#define LCD_DISP_ON_BLINK        0x0D   /* display on, cursor off, blink char     */
#define LCD_DISP_ON_CURSOR       0x0E   /* display on, cursor on                  */
#define LCD_DISP_ON_CURSOR_BLINK 0x0F   /* display on, cursor on, blink char      */

/* move cursor/shift display */
#define LCD_MOVE_CURSOR_LEFT     0x10   /* move cursor left  (decrement)          */
#define LCD_MOVE_CURSOR_RIGHT    0x14   /* move cursor right (increment)          */
#define LCD_MOVE_DISP_LEFT       0x18   /* shift display left                     */
#define LCD_MOVE_DISP_RIGHT      0x1C   /* shift display right                    */

/* function set: set interface data length and number of display lines */
#define LCD_FUNCTION_4BIT_1LINE  0x20   /* 4-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
#define LCD_FUNCTION_8BIT_1LINE  0x30   /* 8-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_8BIT_2LINES 0x38   /* 8-bit interface, dual line,   5x7 dots */

#define LCD_MODE_DEFAULT     ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )


/* 
** function prototypes 
*/
extern void lcd_command(u08 cmd);
extern void lcd_gotoxy(u08 x, u08 y); /* line 1 y=0, line 2 y=1 */
extern void lcd_clrscr(void); /* clear lcd and set cursor to home position */
extern void lcd_home(void); /* set cursor to home position */
extern unsigned char lcd_putc(char c); /* print character at current cursor position */
extern void lcd_puts(const char *s); /* print string on lcd (no auto linefeed) */
#define ROLLLEN 15
/* shift a text string over a given ROLLLEN length
* as shown below. The ROLLBUF size should be the
* visible display lenght or a bit smaller
* 00: |              |
* 01: |             h|
* 02: |            he|
* 03: |           hel|
* 04: |          hell|
* 05: |         hello|
* 06: |        hello |
* 07: |       hello  |
* 08: |      hello   |
* 09: |     hello    |
* 10: |    hello     |
* 11: |   hello      |
* 12: |  hello       |
* 13: | hello        |
* 14: |hello         |
* 15: |ello          |
* 16: |llo           |
* 17: |lo            |
* 18: |o             |
* 19: |              |
* 20: |              |
*
*/
extern void rolltext_lcd_p(prog_char *text, int shiftlen);

/*
** if you hard code a string in the program then you need to decalare
** it like: char *str =PSTR("hello world");
** and then use lcd_puts_p(str);
*/

/*
** print string from program memory on lcd (no auto linefeed)
*/
extern void lcd_puts_p(const prog_char *progmem_s); 

/*
** initialize the LCD. Call this once
*/
extern void lcd_init(u08 dispAttr); 

/*
** To be used with fprintf or printf
** example:
** FILE lcd_str = FDEV_SETUP_STREAM(lcd_putc_stream, NULL, _FDEV_SETUP_WRITE);
** stderr = &lcd_str;
** fprintf(stderr, "Hello world!\nFandi Gunawan");
*/
extern int lcd_putc_stream(unsigned char c, FILE *unused);

/*
** macros for automatically storing string constant in program memory
*/
#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define lcd_puts_P(__s)         lcd_puts_p(P(__s))

#endif //HD47780_H

