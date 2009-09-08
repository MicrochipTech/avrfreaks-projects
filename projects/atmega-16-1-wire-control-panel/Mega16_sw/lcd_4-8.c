/*************************************************************************
 Title  :   HD44780U LCD library
 Authors:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury

      Extended by Martin Thomas 3/2004, removed bugs(?), added functions
      and maybe added new bugs
     
      Extended by Andreas Heinzen 8/2008 E-Mail: heinzen@fh-koblenz.de, 
      removed bugs(?) 
      * correct cursor-positon after scroll 
      * redefine  lcd_waitbusy, lcd_getxy (maybe critical, API has changed)
      * correct inactive state after lcd_read (isn't realy neccessary)
      * made the timing configurable in lcd.h (look at #define DELAY_.... values)
      * added scroll functions for 2-Line Displays
      * added 8-Bit IO-MODE
      * and maybe added new bugs

      Trimmed by SA Development 8/2009, E-Mail: support@sadevelopment.com
      * removed memory mapped mode
      * removed support for 4 bit I/O on any pins except 0,1,2,3
      * removed scrolling and wrapping functions
      * optimized and combined code where possible
      * basically the idea was to fully support the HD44780 interface without
        any higher level functions or code such as wrapping, scrolling, etc.
      * hopefully no bugs introduced!

 Software:  AVR-GCC 3.3
 Hardware:  any AVR device
***************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd_4-8.h"
#include "delay.h"

// Constants/Macros
#define PIN(x) (*(&x - 2))     // Address of Data Direction Register of Port X
#define DDR(x) (*(&x - 1))     // Address of Input Register of Port X

#define lcd_e_high()    LCD_E_PORT  |=  _BV(LCD_E_PIN)
#define lcd_e_low()     LCD_E_PORT  &= ~_BV(LCD_E_PIN)
#if DELAY_MODE_CHECKBUSY==1
  #define lcd_rw_high()   LCD_RW_PORT |=  _BV(LCD_RW_PIN)
  #define lcd_rw_low()    LCD_RW_PORT &= ~_BV(LCD_RW_PIN)
#endif
#define lcd_rs_high()   LCD_RS_PORT |=  _BV(LCD_RS_PIN)
#define lcd_rs_low()    LCD_RS_PORT &= ~_BV(LCD_RS_PIN)


/*************************************************************************
toggle Enable Pin to initiate write
*************************************************************************/
void lcd_e_toggle(void)
  {
    Delay_ns(100);
    lcd_e_high();
    Delay_ns(500);
    lcd_e_low();
    Delay_ns(500);
  }


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data    
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/
#if DELAY_MODE_CHECKBUSY==1
uint8_t lcd_read(uint8_t rs)
  {
    uint8_t data;
    
    if (rs)
      lcd_rs_high();                             // RS=1: Read Data
    else lcd_rs_low();                           // RS=0: Read Busy Flag
    lcd_rw_high();                               // RW=1: Read Mode

	#if LCD_4BIT_MODE==1
      DDR(LCD_DATA_PORT) &= 0xF0;                // Configure Data Pins as Input

      Delay_ns(100);
      lcd_e_high();                              // Read High Nibble First
      Delay_ns(500);
      data = PIN(LCD_DATA_PORT) << 4;
      lcd_e_low();
      Delay_ns(500);

      lcd_e_high();                              // Read Low Nibble
      Delay_ns(500);
      data |= PIN(LCD_DATA_PORT)&0x0F;
      lcd_e_low();
      Delay_ns(500);

      DDR(LCD_DATA_PORT) |= 0x0F;                // Pins as Output
      LCD_DATA_PORT = 0x0F;                      // Pins High (Inactive)

	#else //using 8-Bit-Mode
      DDR(LCD_DATA_PORT) = 0x00;                 // Configure Data Pins as Input

      Delay_ns(100);
      lcd_e_high();
      Delay_ns(500);
      data = PIN(LCD_DATA_PORT);
      lcd_e_low();
      Delay_ns(500);

      DDR(LCD_DATA_PORT) = 0xFF;                 // Pins as Output
      LCD_DATA_PORT=0xFF;                        // Pins High (Inactive)
	#endif
    lcd_rw_low();
    return data;
  }
#endif

/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
#if DELAY_MODE_CHECKBUSY==1
void lcd_waitbusy(void)
  {
    register uint8_t c;

    while ( (c=lcd_read(0)) & (1<<LCD_BUSY))     // Wait Until Busy Flag is Cleared
      ;
  }
#endif

/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data
                 0: write instruction
Returns:  none
*************************************************************************/
void lcd_write(uint8_t data,uint8_t rs)
  {
    #if DELAY_MODE_CHECKBUSY==1
      lcd_waitbusy();
    #endif

    if (rs)
      lcd_rs_high();                           // RS=1: Write Character
    else lcd_rs_low();                         // RS=0: Write Command
    #if DELAY_MODE_CHECKBUSY==1
      lcd_rw_low();                              // RW=0: Write Mode
    #endif
    
    #if LCD_4BIT_MODE==1
      DDR(LCD_DATA_PORT) |= 0x0F;                // Configure Data Pins as Output
      LCD_DATA_PORT = (LCD_DATA_PORT&0xF0) | (data>>4); // Output High Nibble First
      lcd_e_toggle();

      LCD_DATA_PORT = (LCD_DATA_PORT&0xF0) | (data&0x0F); // Output Low Nibble
      lcd_e_toggle();

      LCD_DATA_PORT = (LCD_DATA_PORT&0xF0) | 0x0F; // All Data Pins High (Inactive)
    #else //using 8-Bit_Mode
      DDR(LCD_DATA_PORT) = 0xFF;                 // Configure Data Pins as Output
      LCD_DATA_PORT = data;                      // Output Byte
      lcd_e_toggle();

      LCD_DATA_PORT=0xFF;                        // All Data Pins High (Inactive)
    #endif

    #if DELAY_MODE_CHECKBUSY==0
      if (!rs && data<=((1<<LCD_CLR) | (1<<LCD_HOME))) // Is command clrscr or home?
        {
          Delay_us(DELAY_CLRSCRHOME);
        }
      else
        {
          Delay_us(DELAY_COMMAND);
        }
    #endif
  }


/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd)
  {
    //lcd_waitbusy();
    lcd_write(cmd,0);
  }

/*************************************************************************
Set cursor to specified position
Input:    pos position
Returns:  none
*************************************************************************/
void lcd_goto(uint8_t pos)
  {
    lcd_command((1<<LCD_DDRAM)+pos);
  }


/*************************************************************************
Clear screen
Input:    none
Returns:  none
*************************************************************************/
void lcd_clrscr()
  {
    lcd_command(1<<LCD_CLR);
  }


/*************************************************************************
Return home
Input:    none
Returns:  none
*************************************************************************/
void lcd_home()
  {
    lcd_command(1<<LCD_HOME);
  }


/*************************************************************************
Display character
Input:    character to be displayed
Returns:  none
*************************************************************************/
void lcd_putc(char c)
  {
    //lcd_waitbusy();
    lcd_write(c,1);
  }


/*************************************************************************
Display string
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
  {
    register char c;

    while ((c=*s++)) 
      lcd_putc(c);
  }


/*************************************************************************
Display string from flash
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts_P(const char *progmem_s)
  {
    register char c;

    while ((c=pgm_read_byte(progmem_s++))) 
      lcd_putc(c);
  }


/*************************************************************************
Initialize display
Input:    none
Returns:  none
*************************************************************************/
void lcd_init()
  {
    //Set Pins as Output and Set Control Lines Low
    DDR(LCD_E_PORT)    |= _BV(LCD_E_PIN);
    lcd_e_low();
    DDR(LCD_RS_PORT)   |= _BV(LCD_RS_PIN);
    lcd_rs_low();
    #if DELAY_MODE_CHECKBUSY==1
      DDR(LCD_RW_PORT)   |= _BV(LCD_RW_PIN);
      lcd_rw_low();
    #endif
    DDR(LCD_DATA_PORT) |= (LCD_4BIT_MODE==1)?0x0F:0xFF;

    //Startup Delay
    Delay_us(DELAY_RESET);

    //Three Function Set Commands
    LCD_DATA_PORT |= (_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_8BIT)) >> 4;

    lcd_e_toggle();
    Delay_us(DELAY_INIT_1);

    lcd_e_toggle();
    Delay_us(DELAY_INIT_2);

    lcd_e_toggle();
    Delay_us(DELAY_INIT_2);

    //Set I/O Mode and Lines
    lcd_write(_BV(LCD_FUNCTION) | ((LCD_4BIT_MODE==1)?0:_BV(LCD_FUNCTION_8BIT)) | ((LCD_LINES>1)?_BV(LCD_FUNCTION_2LINES):0),0);
    Delay_us(DELAY_INIT_2);

    //Default Entry Mode Shift=Off, Cursor=Incrementing
    lcd_command(_BV(LCD_ENTRY_MODE) | _BV(LCD_ENTRY_INC));

    //Default Power and Cursor Mode Display=On, Cursor=Off
    lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON));
  }
  
/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor 
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;
    if ( pos < LCD_START_LINE3 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
        addressCounter = LCD_START_LINE4;
    else 
        addressCounter = LCD_START_LINE1;

    lcd_command((1<<LCD_DDRAM)+addressCounter);

}/* lcd_newline */

