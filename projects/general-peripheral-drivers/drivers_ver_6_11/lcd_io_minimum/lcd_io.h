/*************************************************************************
 Title  :   C include file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:      6/Jun/2007
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 
 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display
       Based on Volker Oth's lcd library (http://members.xoom.com/volkeroth),
       Peter Fleury's work <pfleury@gmx.ch>  http://jump.to/fleury .
       and the very good lcd page at www.myke.com
       

       MEMORY MAPPED MODE IS NOT SUPPORTED BY THIS DRIVER !!!
       
LCD MODULE INFORMATION!
       The maximum time required by each instruction for the slowest lcd units is
       4.1 msecs for clearing the display or moving the cursor/display to the "home position",
       160 usecs for all other commands.   

       Usual HD44780 Pins connections 
       1 = Ground 
       2 = Vcc 
       3 = Contrast Voltage 
       4 = "R/S" _Instruction/Register Select 
       5 = "R/W" _Read/Write LCD Registers (See the connection table for additional information). 
       6 = "E" Clock 
       7 - 14 = Data I/O Pins 0 to 7 (0=7, 1=8,... 7=14 )
       15 - Vcc for the BACKLIGHTING (Check to be sure because some units have it for Gnd)
       16 - Gnd for the BACKLIGHTING (Check to be sure because some units have it for Vcc)


***************************************************************************/
#ifndef LCD_IO_H
#define LCD_IO_H



/*-----------------------------------------------------------------------------------*/
/* CONFIGURATION BLOCK STARTS HERE. Change these definitions to adapt setting */
/*-----------------------------------------------------------------------------------*/

/* DEFINE CLOCK FREQUENCY in case it is not defined within the makefile.  */
#ifndef F_CPU
#define F_CPU                     3686400     
#endif
 
/* In this mode one port is used and the data pins must be: 
   Px0=LCD_D4, Px1=LCD_D5, Px2=LCD_D6, Px3=LCD_D7
*/
#define LCD_PORT                  C
#define LCD_PORT_IS_IO            1

/*  The LCD RS and E pins can be on any remaining pins OF THE SAME PORT!
    BE SURE THAT THE R/W PIN OF THE LCD UNIT IS CONNECTED TO GROUND!!!  
*/
#define LCD_RS_PIN                4                /* RS pin number */
#define LCD_E_PIN                 5                /* E pin number */

#define LCD_CHARS_PER_LINE        20       /* visible chars per lcd line           */
#define LCD_LINES                 4        /* visible lines                        */
#define LCD_DELAY_TIME_US         160      
#define LCD_E_PULSE_WIDTH_US      5
  
#define LCD_PUTS_P_NEEDED         1  
#define LCD_PUTS_E_NEEDED         1
#define LCD_PUT_I_NEEDED          1
     

/* END OF 5 PIN CONFIGURATION BLOCK  */
/***********************************************************************************************/





/***********************************************************************************************/
/* CONFIGURATION BLOCK ENDS HERE.  */
/***********************************************************************************************/

/* you shouldn't need to change anything below this line */
/*-----------------------------------------------------------------------------------*/
/*  HD44780 PARAMETERS */
/*-----------------------------------------------------------------------------------*/

#define LCD_LINE_LENGTH          0x40     /* internal line length */
#define LCD_START_LINE1          0x00     /* DDRAM address of first char of line 1 */
#define LCD_START_LINE2          0x40     /* DDRAM address of first char of line 2 */
#define LCD_START_LINE3          0x14     /* DDRAM address of first char of line 3 */
#define LCD_START_LINE4          0x54     /* DDRAM address of first char of line 4 */


/* instruction register bit positions */
#define LCD_CLR                  0      /* DB0: clear display */
#define LCD_HOME                 1      /* DB1: return to home position */
#define LCD_ENTRY_MODE           2      /* DB2: set entry mode */
#define LCD_ENTRY_INC            1      /*   DB1: 1=increment, 0=decrement  */
#define LCD_ENTRY_SHIFT          0      /*   DB2: 1=display shift on        */
#define LCD_ON                   3      /* DB3: turn lcd/cursor on */
#define LCD_ON_DISPLAY           2      /*   DB2: turn display on */
#define LCD_ON_CURSOR            1      /*   DB1: turn cursor on */
#define LCD_ON_BLINK             0      /*     DB0: blinking cursor ? */
#define LCD_MOVE                 4      /* DB4: move cursor/display */
#define LCD_MOVE_DISP            3      /*   DB3: move display (0-> cursor) ? */
#define LCD_MOVE_RIGHT           2      /*   DB2: move right (0-> left) ? */
#define LCD_FUNCTION             5      /* DB5: function set */
#define LCD_FUNCTION_8BIT        4      /*   DB4: set 8BIT mode (0->4BIT mode) */
#define LCD_FUNCTION_2LINES      3      /*   DB3: two lines (0->one line) */
#define LCD_FUNCTION_10DOTS      2      /*   DB2: 5x10 font (0->5x7 font) */
#define LCD_CGRAM                6      /* DB6: set CG RAM address */
#define LCD_DDRAM                7      /* DB7: set DD RAM address */
#define LCD_BUSY                 7      /* DB7: LCD is busy */

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

/* Default LCD mode */
#define LCD_MODE_DEFAULT         ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )

/*-----------------------------------------------------------------------------------*/
/*  function prototypes */
/*-----------------------------------------------------------------------------------*/
extern void lcd_command(unsigned char cmd);
extern unsigned int lcd_getxy(void);
extern void lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y);
extern void lcd_clrscr(void);
extern void lcd_clrline(unsigned char line);
extern void lcd_home(void);
extern void lcd_putc(unsigned char c);
extern void lcd_puts(const unsigned char *s);
#if LCD_PUTS_P_NEEDED == 1
extern void lcd_puts_p(const unsigned char *progmem_s);
#endif
#if LCD_PUTS_E_NEEDED == 1
extern void lcd_puts_e(unsigned char *eeprom_s); 
#endif
#if LCD_PUT_I_NEEDED == 1
void lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars);
#endif
extern void lcd_init(void);

/*
1) Suppose we want to display a 16 bit var with a rvalue 0f 325
2) We give the command lcd_puti(var_name, 0); were var=325
   The display will show 325 
3) alternatives:
   a) We give the command lcd_puti(var_name,1);
      The display will show 32,5 
   b) We give the command lcd_puti(var_name,2);
      The display will show 3,25 
   c) We give the command lcd_puti(var_name,3);
      The display will show 0,325 
   d) We give the command lcd_puti(var_name,4);
      The display will show 0,0325 
   e) We give the command lcd_puti(var_name,1); var=-325
      The display will show -32,5
   f) We give the command lcd_puti(var_name,3); var=-325
      The display will show  -0,325

 */
/*
** macros for automatically storing string constant in program memory
*/

#define lcd_puti(x,y)           lcd_put_i(x,y,0xFF)

#ifndef P
#define P(s) ({static const unsigned char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define lcd_puts_P(__s)         lcd_puts_p(P(__s))


#endif //LCD_IO_H


