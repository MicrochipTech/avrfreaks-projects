/*******************************************************************************************************
 Title  :   C include file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      2/2/2003 6:35:06 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display
       Based on Volker Oth's lcd library (http://members.xoom.com/volkeroth),
       Peter Fleury's work <pfleury@gmx.ch>  http://jump.to/fleury .
       and the very good lcd page at www.myke.com

       With this driver you can select from 4 different I/O modes.
       1) MODE 2 - 2 pin I/O mode with an additional 74LS174 (Lcd reading not possible).
       2) MODE 3 - I2C connection with the use of a PCF8574 (Lcd reading or not can be selected). 
       3) MODE 6 - 6 or 7 pin I/O mode (6 pin when reading is not needed, 7 if lcd reading is needed).
       4) MODE 7 - multiple lcd unit mode. (Lcd reading is not implemented in this mode yet). 

NOTICE:  MEMORY MAPPED MODE IS NOT SUPPORTED BY THIS DRIVER !!!

MODE 6 INFORMATION
       mode 6 now needs only 6 pins as long the pins can function as output
       freeing two bits to be used as general i/o when lcd reading is not needed, and
       7 pins of which at least the 4 data bit pins must be bidirectional if lcd reading is needed. 
       R/W pin on the lcd must be connected to ground if lcd reading is not needed.
       See the connection table for additional information.
       Also the pins can be scattered all over the place (any pin in any port).
       ( the other pins of the port(s) are not affected )
       Be carefull not to change the status of the E lcd pin by using the outp()
       command (both in PORT(X) and DDR(X) ) elsewhere in your  program  otherwise
       your program will crash and you will have a lot of fun for the whole family.
       Use sbi() and cbi() commands instead.

MODE 7 INFORMATION
       mode 7 supports up to 3 lcd units and all the characteristics of mode 6
       except that lcd reading is not possible.
       The lcd diplays can be of different type.
       Lcd reading is not implemented yet so the R/W pin must be connected to Gnd.


MODE 2 INFORMATION
       mode 2 uses only 2 pins in any combination you want as long the pin
       can function as output.
       You also need to make a very simple cirquit (really silly) with 74LS174.
       See the included JPG photo for the schematic.
       Also the pins can be scattered all over the place (any pin in any port).
       ( the other pins of the port(s) are not affected )
       YOU MAY NOT! MULTIPLEX THE DATA AND CLOCK PINS WITH OTHER DEVICES 
       The pins are automatically set as outputs when you call an lcd function
       Be carefull not to change the status of the CLOCK and DATA pins by using the outp()
       command (both in PORT(X) and DDR(X) ) elsewhere in your  program  otherwise
       your program will crash and you will have a lot of fun for the whole family (again!).
       Use sbi() and cbi() commands instead.     

MODE 3 INFORMATION
       mode 3 uses the I2C bus and a PCF8574. Up to 8 different lcd units can be connected.
       You also need to make a very simple cirquit with PCF8574 (one for each lcd).
       Also the pins can be scattered all over the place (any LCD pin in any PCF8574 port pin).
       You must also have an I2C Master driver (preferably my I2C master driver).
       Be sure to edit the i2c.h to reflect the AVR pins where the PCF8574 SDA & SCL pins are connected.
       DO NOT FORGET THE SDA & SCL PULL UP RESISTORS!!! 

              The PCF8534 PINOUT IS GIVEN BELOW   
                                                    VCC      VCC
                                                     |        |
                        -------------                |        |
GND or VCC    ADD1  1  |             |  16  VCC   1.2 Kohm  1.2 Kohm  
                       |             |               |        |
GND or VCC    ADD2  2  |             |  15  SDA  ----|--------X--- TO AVR SDA
                       |             |               |
GND or VCC    ADD3  3  |             |  14  SCL  ----X------------- TO AVR SCL
                       |             |
   LCD D4 ->   P0   4  |             |  13  INT   NOT CONNECTED
                       | PCF8534 AP  |
   LCD D5 ->   P1   5  |             |  12  P7    NOT CONNECTED
                       |             |
   LCD D6 ->   P2   6  |             |  11  P6 -> LCD E
                       |             |
   LCD D7 ->   P3   7  |             |  10  P5 -> LCD RW
                       |             |           
               GND  8  |             |  9   P4 -> LCD RS
                        -------------

OF COURSE YOU CAN ASSIGN ANY PCF8574 PIN TO ANY LCD SIGNAL
ALSO REMEMBER THAT ONLY A PAIR OF PULL UP RESISTORS ARE NEEDED ON THE I2C BUS.
SO IF YOU USE MULTIPLE DEVICES ON THE I2C BUS DO NOT CONNECT MORE THAN
1 RESISTOR TO THE SCL SIGNAL AND MORE THAN 1 RESISTOR TO THE SDA SIGNAL.



LCD MODULE INFORMATION!
       The maximum time required by each instruction for the slowest lcd units is
       4.1 msecs for clearing the display or moving the cursor/display to the "home position",
       160 usecs for all other commands. (i use 50 because i use good units)  

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

TYPICAL CONNECTION TABLE AS USED IN THIS lcd_io.h FILE.
ACTUALLY YOU CAN USE ANY PIN AS LONG AS THE PIN CARRIES THE CORRECT SIGNAL TO THE CORRECT LCD PIN.       

port_pin0  -> lcd_D4_pin11 
port_pin1  -> lcd_D5_pin12 
port_pin2  -> lcd_D6_pin13 
port_pin3  -> lcd_D7_pin14 
port_pin4  -> lcd_RS_pin4  
port_pin5  -> lcd_E_pin6   
GND        -> lcd_RW_pin5 if lcd reading is not needed or when using mode 2 or mode 7
port_pin6  -> lcd_RW_pin5 if lcd reading is needed in mode 6 only
 



CAUTION!!!  FOR MODES WHERE LCD READING IS NOT POSSIBLE OR NOT ENABLED READ THE FOLLOWING.
            FOR SLOW LCD UNITS INCREASE THE "LCD_DELAY_TIME_US"  VALUE!!!  
            The driver does not read anything back from the lcd because that way i only need 6
            I/O lines and furthermore i can use the output only port found in MEGA103.
            Since i dont read anything back, i just wait for commands to be executed
            so below i define the time to wait for the two categories of commands of HD44780 .    
            The two CATEGORIES are:
            1) 2ms for clear screen and return home commands (nominal value=1,52 ms)
               and it is derived from "LCD_DELAY_TIME_US" .  
            2) 50us for all other commands (nominal value=37 us) 
  
            Even the slowest units will work reliably with LCD_DELAY_TIME_US=160. 
            The Longer delay needed for CATEGORY 1 is calculated from the "LCD_DELAY_TIME_US" value.
            The specifications for HD44780 @ 270KHZ are 37 us for CATEGORY 2 and 1,52 ms for CATEGORY 1 

 
*******************************************************************************************************/
#ifndef LCD_IO_H
#define LCD_IO_H

/*######################################################################################################*/
/*           CONFIGURATION BLOCK STARTS HERE. Change these definitions to adapt setting                 */
/*######################################################################################################*/

/***********************************************************************************************/
/*                GLOBAL SETTINGS (settings described here are applyied everywhere)            */
/***********************************************************************************************/
/*  */

#ifndef F_CPU
#define F_CPU                     3686400   /* CPU CLOCK FREQUENCY         */
#endif

#define LCD_IO_MODE               6         /* 6=6 PIN I/O, 2=2 PIN I/O, 3=I2C, 7=multi lcd */
#define LCD_AUTO_LINE_FEED        0         /* 1 = Auto line feed, 0 = no Auto line feed */
#define LCD_DELAY_TIME_US         100       /* THE TYPICAL TIME THE LCD NEEDS TO COMPLETE A COMMAND  */ 
#define LCD_E_PULSE_WIDTH_US      1         /* THE E PULSE WIDTH IN MICROSECONDS (Timing is accurate)*/
#define LCD_DECIMAL_POINT         ','       /* The decimal point punctuation mark char for lcd_get_i() */

/* WAYS TO REDUCE CODE SIZE BY NOT COMPILING UNWANTED FUNCTIONS OR CODE PORTIONS */
#define LCD_PUT_I_NEEDED          1         /* 1=function available, 0=not available. */
#define LCD_PUTS_NEEDED           1         /* 1=function available, 0=not available. */
#define LCD_PUTS_P_NEEDED         1         /* 1=function available, 0=not available. */
#define LCD_PUTS_E_NEEDED         1         /* 1=function available, 0=not available. */
#define LCD_PUTC_CGRAM_NEEDED     1         /* 1=function available, 0=not available. */
#define LCD_CLRLINE_NEEDED        1         /* 1=function available, 0=not available. */
#define LCD_GETXY_NEEDED          1         /* 1=function available, 0=not available. */
/* 1=limit, error & control chars checking disabled. Also auto line feed off. For experts only! */
#define LCD_SAVE_MORE_CODE_SPACE  0         

/***********************************************************************************************/
/*                         START OF SELECTED MODE CONFIGURATION BLOCK                          */
/***********************************************************************************************/


/***********************************************************************************************/
#if LCD_IO_MODE == 2    /*    START OF MODE 2 CONFIGURATION BLOCK                              */
/***********************************************************************************************/

/* 
    CONFIGURATION OF BELOW LINES ONLY NECESSARY IF YOU SELECTED MODE 2 IN GLOBAL SETTINGS  
    If you plan to use just one port for all pins then just edit "LCD_PORT" 
    otherwise you must specify the port of each lcd signal. the port(s) must be able
    to function as output.  It can be any combination!
    USE CAPITAL LETTER FOR PORT DESIGNATION! (A,B,C,D...etc.)
*/                             
#define LCD_CHARS_PER_LINE       20        /* visible chars per lcd line */
#define LCD_LINES                4         /* visible lines */

#define LCD_PORT                 C  

#define LCD_CLOCK_SIGNAL_PORT    LCD_PORT      /* Put your lcd clock port here (A,B,C...etc.)  */
#define LCD_CLOCK_PIN            0             /* Put your lcd clock pin here */

#define LCD_DATA_SIGNAL_PORT     LCD_PORT      /* Put your lcd data port here (A,B,C...etc.)    */
#define LCD_DATA_PIN             1             /* Put your lcd data pin here */

#endif /* #if LCD_IO_MODE == 2 */
/***********************************************************************************************/
/*                            END OF 2 PIN CONFIGURATION BLOCK                                 */
/***********************************************************************************************/

/***********************************************************************************************/
#if LCD_IO_MODE == 3    /*    START OF MODE 3 CONFIGURATION BLOCK                              */
/***********************************************************************************************/
 
/* CONFIGURATION OF BELOW LINES ONLY NECESSARY IF YOU SELECTED MODE 3 IN GLOBAL SETTINGS  */

#define LCD_READ_REQUIRED        0         /* 0= read functions not available */
/*
    if "LCD_DONT_READ_BUSY_FLAG" is set to 1, a delay loop is used instead of reading the busy flag,
    since I2c data transfer even @ 400 khz is much slower than the slowest LCD unit responce time.
    If we start reading the BUSY FLAG in the morning it will be late noon when we finally read it
    even once. Also the lcd_getxy() returns the lcd coordinates without actually reading the lcd.
    Data reading is done normally by reading the lcd.
     
*/
#define LCD_DONT_READ_BUSY_FLAG  1         /* 1=much faster data transfers */
/*
   This enables the backup functions that backup and restore the lcd display.
   LCD_LINES X LCD_CHARS_PER_LINE bytes are needed (That means 80 bytes for a 4 x 20 lcd display).
   The buffer size when lcd backup is required is set upon LCD_0 so make LCD_0 the largest lcd.
   0=lcd screen backup location is eeprom, 1= lcd screen backup location is ram. 
   If you need to backup less, use the lcd_getc function.
   When using the lcd_getc() function the Address Counter (AC) is auto incremented or decremented
   according to the ENTRY MODE selected during initialization, just like the lcd_putc() function works, 
   so when doing a sequential read, there is no need to reposition the cursor each time a read is performed.
   The backup & restore settings are valid only if LCD_READ_REQUIRED == 1 
*/
#define LCD_BACKUP_REQUIRED      0                
#define LCD_BACKUP_LOCATION      0                

#define NUMBER_OF_LCD_UNITS      1         /* The total lcd units that you will connect. max=8 */

#define LCD_0_I2C_ADDRESS        0x70
#define LCD_0_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_0_LINES              4         /* visible lines */

#define LCD_1_I2C_ADDRESS        0x72
#define LCD_1_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_1_LINES              4         /* visible lines */

#define LCD_2_I2C_ADDRESS        0x74
#define LCD_2_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_2_LINES              4         /* visible lines */

#define LCD_3_I2C_ADDRESS        0x76
#define LCD_3_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_3_LINES              4         /* visible lines */

#define LCD_4_I2C_ADDRESS        0x78
#define LCD_4_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_4_LINES              4         /* visible lines */

#define LCD_5_I2C_ADDRESS        0x7A
#define LCD_5_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_5_LINES              4         /* visible lines */

#define LCD_6_I2C_ADDRESS        0x7C
#define LCD_6_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_6_LINES              4         /* visible lines */

#define LCD_7_I2C_ADDRESS        0x7E
#define LCD_7_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_7_LINES              4         /* visible lines */

#define LCD_D4_PIN               0         /* PCF8574 port pin number */
                                        
#define LCD_D5_PIN               1         /* PCF8574 port pin number */

#define LCD_D6_PIN               2         /* PCF8574 port pin number */       

#define LCD_D7_PIN               3         /* PCF8574 port pin number */

#define LCD_RS_PIN               4         /* PCF8574 port pin number */

#define LCD_E_PIN                6         /* PCF8574 port pin number */

/* YOU NEED TO EDIT  "LCD_RW_SIGNAL_PORT" AND "LCD_RW_PIN" ONLY IF "LCD_READ_REQUIRED == 1" */
#if LCD_READ_REQUIRED == 1
#define LCD_RW_PIN               5         /* PCF8574 port pin number */
#endif

#endif  /* #if LCD_IO_MODE == 3 */
/***********************************************************************************************/
/*                            END OF 3 PIN CONFIGURATION BLOCK                                 */
/***********************************************************************************************/

/***********************************************************************************************/
#if LCD_IO_MODE == 6    /*    START OF MODE 6 CONFIGURATION BLOCK                              */
/***********************************************************************************************/
 
/* 
    CONFIGURATION OF BELOW LINES ONLY NECESSARY IF YOU SELECTED MODE 6 IN GLOBAL SETTINGS  
    If you plan to use just one port for all pins then just edit "LCD_PORT" 
    otherwise you must specify the port of each lcd signal. the port(s) must be able
    to function as output.   It can be any combination!  
    PUT YOUR LCD PORT LETTER HERE USING CAPITAL LETTER (A,B,C,D...etc)
*/
#define LCD_CHARS_PER_LINE       20        /* visible chars per lcd line */
#define LCD_LINES                4         /* visible lines */

#define LCD_MULTIPLEX_ENABLE     0         /* 1= the DDR's used are saved and restored */
#define LCD_READ_REQUIRED        0         /* 0=use delay, 1=read busy flag & data (7 pins needed) */

/*
   This enables the backup functions that backup and restore the lcd display.
   LCD_LINES X LCD_CHARS_PER_LINE bytes are needed (That means 80 bytes for a 4 x 20 lcd display).
   0=lcd screen backup location is eeprom, 1= lcd screen backup location is ram. 
   If you need to backup less, use the lcd_getc function.
   When using the lcd_getc() function the Address Counter (AC) is auto incremented or decremented
   according to the ENTRY MODE selected during initialization, just like the lcd_putc() function works, 
   so when doing a sequential read, there is no need to reposition the cursor each time a read is performed.
   The backup & restore settings are valid only if LCD_READ_REQUIRED == 1 
*/
#define LCD_BACKUP_REQUIRED      0                
#define LCD_BACKUP_LOCATION      0                

#define LCD_PORT                 C   

#define LCD_DATA4_PORT           LCD_PORT         /* port for data 0 pin  */
#define LCD_D4_PIN               0                /* AVR port pin number */

#define LCD_DATA5_PORT           LCD_PORT         /* port for data 1 pin  */ 
#define LCD_D5_PIN               1                /* AVR port pin number */

#define LCD_DATA6_PORT           LCD_PORT         /* port for data 2 pin  */
#define LCD_D6_PIN               2                /* AVR port pin number */       

#define LCD_DATA7_PORT           LCD_PORT         /* port for data 3 pin  */
#define LCD_D7_PIN               3                /* AVR port pin number */

#define LCD_RS_SIGNAL_PORT       LCD_PORT         /* port for RS line */
#define LCD_RS_PIN               4                /* AVR port pin number */

#define LCD_E_SIGNAL_PORT        LCD_PORT         /* port for Enable line */
#define LCD_E_PIN                5                /* AVR port pin number */



/* YOU NEED TO EDIT  "LCD_RW_SIGNAL_PORT" AND "LCD_RW_PIN" ONLY IF "LCD_READ_REQUIRED == 1" */
#if LCD_READ_REQUIRED == 1
#define LCD_RW_SIGNAL_PORT       LCD_PORT         /* port for R/W line */
#define LCD_RW_PIN               6                /* AVR port pin number */
#endif

#endif  /* #if LCD_IO_MODE == 6 */
/***********************************************************************************************/
/*                            END OF 6 PIN CONFIGURATION BLOCK                                 */
/***********************************************************************************************/


/***********************************************************************************************/
#if LCD_IO_MODE == 7    /*    START OF MODE 7 CONFIGURATION BLOCK                              */
/***********************************************************************************************/
/* 
    CONFIGURATION OF BELOW LINES ONLY NECESSARY IF YOU SELECTED MODE 7 IN GLOBAL SETTINGS 
    If you plan to use just one port for all pins then just edit "LCD_PORT" 
    otherwise you must specify the port of each lcd signal. the port(s) must be able
    to function as output.   It can be any combination!  
    PUT YOUR LCD PORT LETTER HERE USING CAPITAL LETTER (A,B,C,D...etc)
*/
#define NUMBER_OF_LCD_UNITS      2         /* 2 or 3. if you set it to 1, mode 6 will be selected */

#define LCD_0_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_0_LINES              4         /* visible lines */

#define LCD_1_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_1_LINES              4         /* visible lines */

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_2_LINES              4         /* visible lines */
#endif



#define LCD_PORT                 C   

#define LCD_DATA4_PORT           LCD_PORT         /* port for data 0 pin  */
#define LCD_D4_PIN               0                /* AVR port pin number */

#define LCD_DATA5_PORT           LCD_PORT         /* port for data 1 pin  */ 
#define LCD_D5_PIN               1                /* AVR port pin number */

#define LCD_DATA6_PORT           LCD_PORT         /* port for data 2 pin  */
#define LCD_D6_PIN               2                /* AVR port pin number */       

#define LCD_DATA7_PORT           LCD_PORT         /* port for data 3 pin  */
#define LCD_D7_PIN               3                /* AVR port pin number */

#define LCD_RS_SIGNAL_PORT       LCD_PORT         /* port for RS line */
#define LCD_RS_PIN               4                /* AVR port pin number */

#define LCD_0_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_0_E_PIN              5                /* AVR port pin number */

#define LCD_1_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_1_E_PIN              6                /* AVR port pin number */

/* EDIT THE BELOW LINES IF YOU USE 3 LCD UNITS */
#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_2_E_PIN              7                /* AVR port pin number */
#endif

#endif  /* LCD_IO_MODE == 7 */

/*######################################################################################################*/
/*                             CONFIGURATION BLOCK ENDS HERE.                                           */
/*######################################################################################################*/

/* you shouldn't need to change anything below this line */

/********************************************************************************************************/
/*                             HD44780 DDRAM PARAMETERS                                                 */
/********************************************************************************************************/

#define LCD_LINE_LENGTH          0x40     /* internal line length */
#define LCD_START_LINE1          0x00     /* DDRAM address of first char of line 1 */
#define LCD_START_LINE2          0x40     /* DDRAM address of first char of line 2 */
#define LCD_START_LINE3          0x14     /* DDRAM address of first char of line 3 */
#define LCD_START_LINE4          0x54     /* DDRAM address of first char of line 4 */

/********************************************************************************************************/
/*                    INSTRUCTION REGISTER BIT POSITIONS AND COMBINATIONS                               */
/********************************************************************************************************/
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

/* function set: set interface data length and number of display lines */
#define LCD_FUNCTION_4BIT_1LINE  0x20   /* 4-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
#define LCD_FUNCTION_8BIT_1LINE  0x30   /* 8-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_8BIT_2LINES 0x38   /* 8-bit interface, dual line,   5x7 dots */

/* Lcd default mode used in this driver */
#define LCD_MODE_DEFAULT         ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )                                         

/********************************************************************************************************/
/*                     LCD COMMANDS (CAN BE USED WITH "lcd_command(cmd);")                              */
/********************************************************************************************************/
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
#define LCD_CLEAR_SCREEN         (1<<LCD_CLR)
#define LCD_RETURN_HOME          (1<<LCD_HOME)

/* move cursor/shift display */
#define LCD_MOVE_CURSOR_LEFT     0x10   /* move cursor left  (decrement)          */
#define LCD_MOVE_CURSOR_RIGHT    0x14   /* move cursor right (increment)          */
#define LCD_MOVE_DISP_LEFT       0x18   /* shift display left                     */
#define LCD_MOVE_DISP_RIGHT      0x1C   /* shift display right                    */



/********************************************************************************************************/
/*                             LCD USEFULL DEFINITIONS                                                  */
/********************************************************************************************************/

#ifndef LOCATION_IS_RAM
#define LOCATION_IS_RAM          1
#endif
#ifndef LOCATION_IS_EEPROM
#define LOCATION_IS_EEPROM       2
#endif 
#ifndef LOCATION_IS_FLASH
#define LOCATION_IS_FLASH        3
#endif

#if LCD_IO_MODE == 7 || LCD_IO_MODE == 3
/* lcd_select() argument values */
#define ALL                      0
#define LCD_0                    0 
#if NUMBER_OF_LCD_UNITS >= 2
#define LCD_1                    1
#endif
#if NUMBER_OF_LCD_UNITS >= 3
#define LCD_2                    2
#endif
#if NUMBER_OF_LCD_UNITS >= 4
#define LCD_3                    3
#endif
#if NUMBER_OF_LCD_UNITS >= 5
#define LCD_4                    4
#endif
#if NUMBER_OF_LCD_UNITS >= 6
#define LCD_5                    5
#endif
#if NUMBER_OF_LCD_UNITS >= 7
#define LCD_6                    6
#endif
#if NUMBER_OF_LCD_UNITS >= 8
#define LCD_7                    7
#endif

#endif /* #if LCD_IO_MODE == 7 */

/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/

/* 
   MANUAL LCD INITIALIZATION IS NOT NEEDED ANYMORE. IT IS DONE AUTOMATICALLY!
   An exception is when "LCD_SAVE_MORE_CODE_SPACE == 1". Then a call to "lcd_init()" is needed.
*/ 
/* HIGH level functions */
extern void          lcd_init(void);
#if NUMBER_OF_LCD_UNITS >= 2 
extern void          select_lcd(unsigned char lcd_unit);
#endif
extern void          lcd_command(unsigned char cmd);
extern void          lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y);
extern void          lcd_putc(unsigned char c);
extern void          lcd_puts(const unsigned char *s);
#if LCD_PUT_I_NEEDED == 1
extern void          lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars) ;
#endif
#if LCD_PUTS_P_NEEDED == 1
extern void          lcd_puts_p(const unsigned char *progmem_s);
#endif
#if LCD_PUTS_E_NEEDED == 1
extern void          lcd_puts_e(unsigned char *eeprom_s); 
#endif
#if LCD_PUTC_CGRAM_NEEDED == 1
extern void          lcd_putc_cgram(const unsigned char *user_char, unsigned char char_position);
#endif
#if LCD_CLRLINE_NEEDED == 1
extern void          lcd_clrline(unsigned char line);
#endif
#if LCD_GETXY_NEEDED == 1
/*
    The return value of the "lcd_getxy()" function is an integer,
    with the high byte containing the current line number (y) and the low byte 
    containing the char position in that line (x).
    If the lower byte has a value of 20 that means that you filled that line.
    This position result can only happen when no lcd reading is available.
    When lcd reading is available the maximum x == 19. 
*/
extern unsigned int  lcd_getxy(void);
#endif
#if  LCD_READ_REQUIRED == 1
extern unsigned char lcd_getc(void);
#if LCD_BACKUP_REQUIRED == 1
extern void          lcd_backup_scr(void);
extern void          lcd_restore_scr(void);
#endif
#endif


/*
   The lcd_puti is actually a macro of the lcd_put_i(x,y,z) function with z set to 0xFF.
   x= the signed 16 bit number, y= the decimal digits wanted and z the number of reserved lcd chars.
   x must be between -32767 and +32767 and y from 0 to 5 max. z bigger than 8 is ignored and
   the number gets as many lcd chars as it needs (325 will get 3 lcd chars and -1 will get 2 lcd chars).
   Maximum char count case is lcd_put_i(-32767,5,8);. The display should show -0,32767 (8 chars)
   The chars are now left alligned so if you reserve lets say 5 digits for a 3 digit number like 325
   then the display will show 325+space+space (Of course you can't see the spaces).

More examples:

   a) We give the command lcd_puti(var_name,0); and var=325
      The display will show 325 
   b) We give the command lcd_puti(var_name,2); and var=325
      The display will show 3,25 
   c) We give the command lcd_put_i(var_name,3,5); and var=325
      The display will show 0,325 
   d) We give the command lcd_put_i(var_name,4,4); and var=325
      the whole number is 0,0325 but since you reserved only 4 digits the display will show  0,03 
   e) We give the command lcd_put_i(var_name,1,6); and var=(-325)
      The display will show -32,5+space
   f) We give the command lcd_put_i(var_name,1,3); and var=(-325)
      the whole number is -32,5 but since you reserved only 3 digits the display will show -32  
 */

/********************************************************************************************************/
/*                                 FUNCTION RESEMBLING MACROS                                           */
/********************************************************************************************************/


#define lcd_clrscr()            lcd_command(LCD_CLEAR_SCREEN)
#define lcd_home()              lcd_command(LCD_RETURN_HOME)
#define lcd_puti(x,y)           lcd_put_i(x,y,0xFF)
#define lcd_fill_cgram(array)   lcd_putc_cgram((const unsigned char*)array, sizeof(array))

#if LCD_SAVE_MORE_CODE_SPACE == 1
#define lcd_goto_cgram(char)    lcd_command((1<<LCD_CGRAM)+(char*8))
#endif
/********************************************************************************************************/
/*                                 USEFULL MACROS                                                       */
/********************************************************************************************************/
/* macros for automatically storing string constant in program memory */
#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define lcd_puts_P(__s)         lcd_puts_p(P(__s))






#endif //LCD_IO_H
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


