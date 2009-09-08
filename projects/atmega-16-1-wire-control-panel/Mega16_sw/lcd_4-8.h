#ifndef HD44780_H
#define HD44780_H

/*************************************************************************

See C file for Title and Author Information.

You must go through the LCD Settings section below and configure the defines.

Supports LCD communications on as few as 6 pins or as many as 11 pins depending on
configuration.  If you set DELAY_MODE_CHECKBUSY=0 to use delays instead of reading
from the LCD busy flag, you don't need to connect the LCD RW pin, but you must tie
the RW pin low.

To init the display, clear the screen, and output "Hello World...":
  lcd_init();
  lcd_clrscr();
  lcd_puts("Hello World...");

To put a character:
  lcd_putc('A');

To turn off the display:
  lcd_command(_BV(LCD_DISPLAYMODE));

To turn on the display:
  lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON));

To turn on the display AND display an underline cursor:
  lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON) | _BV(LCD_DISPLAYMODE_CURSOR));

To turn on the display AND display a blinking cursor:
  lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON) | _BV(LCD_DISPLAYMODE_BLINK));

To move the cursor to the left:
  lcd_command(_BV(LCD_MOVE));
  
To move the cursor to the right:
  lcd_command(_BV(LCD_MOVE) | _BV(LCD_MOVE_RIGHT));

To move the cursor to a specific location:
  lcd_goto(0x40); //0x40 is often the beginning of the second line
                  //each LCD display will have its memory mapped
                  //differently

To create a custom character:
  lcd_command(_BV(LCD_CGRAM)+0); //up to eight may be created +0 through +7
  lcd_putc(0b00000); //5x8 bitmap of character, in this example a backslash
  lcd_putc(0b10000);
  lcd_putc(0b01000);
  lcd_putc(0b00100);
  lcd_putc(0b00010);
  lcd_putc(0b00001);
  lcd_putc(0b00000);
  lcd_putc(0b00000);

To display this custom character:
  lcd_putc(0);

To shift the display so that the characters on screen are pushed to the left:
  lcd_command(_BV(LCD_MOVE) | _BV(LCD_MOVE_DISP));  
  
To shift the display so that the characters on screen are pushed to the left:
  lcd_command(_BV(LCD_MOVE) | _BV(LCD_MOVE_DISP) | _BV(LCD_MOVE_RIGHT));

***************************************************************************/

#include <inttypes.h>

/*
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 303
#error "This library requires AVR-GCC 3.3 or later."
#endif
*/

//LCD Settings
#define F_CPU 16000000UL              // Set Clock Frequency

#define LCD_4BIT_MODE            1           // 1=4 Bit I/O Mode, 0=8 Bit I/O Mode
#define DELAY_MODE_CHECKBUSY     1           // 1=Checks the busy status by reading from the LCD, Requires RW Line
                                             // 0=Does not read busy status, delays instead, Does NOT Require RW Line
#define LCD_LINES                4           // Number of Lines, Only Used for Set I/O Mode Command


#define DELAY_RESET              16000       // All Delays in uS
#define DELAY_INIT_1             5000
#define DELAY_INIT_2             100

#define DELAY_CLRSCRHOME         1640        // Only Used when DELAY_MODE_CHECKBUSY=0
#define DELAY_COMMAND            40          // Only Used when DELAY_MODE_CHECKBUSY=0

#define LCD_DATA_PORT            PORTC       // 4 Bit I/O Mode = Lower 4 Pins, 8 Bit I/O Mode = All 8 Pins
#define LCD_E_PORT               PORTD       // Port for E line
#define LCD_E_PIN                7           // Pin for E line
#define LCD_RS_PORT              PORTC       // Port for RS line
#define LCD_RS_PIN               4           // Pin for RS line
#if DELAY_MODE_CHECKBUSY==1
  #define LCD_RW_PORT            PORTC       // Port for RW line
  #define LCD_RW_PIN             5           // Pin for RW line
#elif DELAY_MODE_CHECKBUSY!=0
  #error DELAY_MODE_CHECKBUSY Value is Invalid.
#endif
#if LCD_4BIT_MODE!=1 && LCD_4BIT_MODE!=0
  #error LCD_4BIT_MODE Value is Invalid.
#endif


//LCD Constants for HD44780
#define LCD_CLR                 0    // DB0: clear display

#define LCD_HOME                1    // DB1: return to home position

#define LCD_ENTRY_MODE          2    // DB2: set entry mode
#define LCD_ENTRY_INC           1    // DB1: 1=increment, 0=decrement
#define LCD_ENTRY_SHIFT         0    // DB0: 1=display shift on

#define LCD_DISPLAYMODE         3    // DB3: turn lcd/cursor on
#define LCD_DISPLAYMODE_ON      2    // DB2: turn display on
#define LCD_DISPLAYMODE_CURSOR  1    // DB1: turn cursor on
#define LCD_DISPLAYMODE_BLINK   0    // DB0: blinking cursor

#define LCD_MOVE                4    // DB4: move cursor/display
#define LCD_MOVE_DISP           3    // DB3: move display (0-> cursor)
#define LCD_MOVE_RIGHT          2    // DB2: move right (0-> left)

#define LCD_FUNCTION            5    // DB5: function set
#define LCD_FUNCTION_8BIT       4    // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES     3    // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS     2    // DB2: 5x10 font (0->5x7 font)

#define LCD_CGRAM               6    // DB6: set CG RAM address
#define LCD_DDRAM               7    // DB7: set DD RAM address

#define LCD_BUSY                7    // DB7: LCD is busy

#define LCD_START_LINE1  0x00        // DDRAM address of first char of line 1 
#define LCD_START_LINE2  0x40     	 // DDRAM address of first char of line 2 
#define LCD_START_LINE3  0x14     	 // DDRAM address of first char of line 3 
#define LCD_START_LINE4  0x54     	 // DDRAM address of first char of line 4 

//LCD High Level Functions
extern void lcd_goto(uint8_t pos);
extern void lcd_clrscr(void);
extern void lcd_home(void);
extern void lcd_putc(char c);
extern void lcd_puts(const char *s);
extern void lcd_puts_P(const char *progmem_s);
extern void lcd_init(void);
extern void LCD_Test_Msg(void);


//LCD Low Level Functions
extern void lcd_e_toggle(void);
#if DELAY_MODE_CHECKBUSY==1
  extern uint8_t lcd_read(uint8_t rs);
  extern void lcd_waitbusy(void);
#endif
extern void lcd_write(uint8_t data,uint8_t rs);
extern void lcd_command(uint8_t cmd);

#endif //LCD_H




