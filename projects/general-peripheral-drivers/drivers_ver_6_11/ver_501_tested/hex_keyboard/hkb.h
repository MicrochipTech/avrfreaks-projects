/****************************************************************************
 Title  :   C  include file for the HEX KEYBOARD FUNCTIONS library (hkb.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      15/2/2003 7:28:28 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/*
The keyborad driver emulates now close enough the ERICSSON cellular phones keyboard with a few 
alteration due to limited resources.
However you can alter the keyboard behaviour by editing certain parameters within this .h file.

The keyboard driver functions get input from the keyboard and simultaneously display the input on the
LCD screen at the predefined position given as an argument to the keyboard function.
The input can be multiline or whatever length you give as an argument the the keyboard function.

During operation the keyboard port acts as both output and input so you must choose a bidirectional port.
After the driver finishes the command that was executing sets all keyb port pins as inputs, pullups off, 
in case you are multiplexing the keyboard pins with other devices. You can multiplex only 4 pins 
(either the row or the column pins, not mixed!)
That is because if you multiplex all lines when someone hits a key the correspondig row and
column lines will be shorted thus altering the values on the pins. 

I use the PACTEC SELF ADHESIVE 4X4 keyboard (Thats the only one i could easily find here).
You may use whatever keybord you want but you have to edit the definitions that define
the keyboard connections.

The keyboard driver can use any port-pin combination. Pins can be located in any 
bidirectional port. The driver automaticaly detects if more than one port is used and it
compiles different code as needed.
The actual keyboard port and pin assignments are defined in the keyboard key definition section.
The "KEYBOARD_PORT" definition is used as a general setting.

IF YOU USE AN EXTERNAL INTERRUPT WITHOUT THE USE OF I2C BUS THOSE ARE THE CONNECTIONS NEEDED.
               
                       C1   C2   C3   C4 
                    -----------------------         1N4148
                    |  1    2    3    4    | R1 -----|<------|      10K
                    |  5    6    7    8    | R2 -----|<------|----/\/\/\/\---VCC 5V
                    |  9    10   11   12   | R3 -----|<------|
                    |  13   14   15   16   | R4 -----|<------|
                    -----------------------                  |
                                                             |
                                                             |-----> To ext interrupt pin
                   DIODES AND RESISTOR NEEDED ONLY IF YOU
                   USE AN EXTERNAL INTERRUPT.
                   THE AVR PINS ARE CONNECTED DIRECTLY TO THE ROW AND COLUMN 
                   TERMINALS OF THE HEX KEYBOARD.




    IF YOU SELECT TO USE THE I2C BUS (with a PCF8574) THE PORT DEFINITIONS IN THIS H FILE ARE INACTIVE.
    ALSO THE PIN NUMBERS REPRESENT THE PCF8574 PINS AND NOT! THE AVR PINS.
    IF YOU USE MORE THAN ONE HEX KEYBOARD IT IS IMPORTANT THAT YOU DO NOT DEFINE MORE HEX KEYBOARDS 
    THAN THOSE THAT ARE ACTUALLY PRESENT AS THE KEYBOARDS ARE AUTOMATICALLY SCANNED AND YOU WILL UPSET
    OTHER I2C DEVICES THAT MIGHT HAVE THE SAME ADDRESS (LCD's etc.)     
                  

               The PCF8534 PINOUT IS GIVEN BELOW   
                                                   VCC    VCC  VCC
                                                    |      |    |
                        -------------            1,2 Kohm  |  1,2 Kohm   
GND or VCC    ADD1  1  |             |  16  VCC     |      |    |
                       |             |              |   10 Kohm |
GND or VCC    ADD2  2  |             |  15  SDA  ---|------|----X-- TO AVR SDA
                       |             |              |      |
GND or VCC    ADD3  3  |             |  14  SCL  ---X------|------- TO AVR SCL
                       |             |                     |
     COL1 ->   P0   4  |             |  13  INT  ----------X------- TO EXT INT
                       | PCF8534 AP  |
     COL2 ->   P1   5  |             |  12  P7 -> ROW4
                       |             |
     COL3 ->   P2   6  |             |  11  P6 -> ROW3
                       |             |
     COL4 ->   P3   7  |             |  10  P5 -> ROW2
                       |             |           
               GND  8  |             |  9   P4 -> ROW1
                        -------------

OF COURSE YOU CAN ASSIGN ANY PCF8574 PIN TO ANY KEYBOARD LINE.
ALSO REMEMBER THAT ONLY A PAIR OF PULL UP RESISTORS ARE NEEDED ON THE I2C BUS.
SO IF YOU USE MULTIPLE DEVICES ON THE I2C BUS DO NOT CONNECT MORE THAN 
1 RESISTOR TO THE SCL SIGNAL AND MORE THAN 1 RESISTOR TO THE SDA SIGNAL.


If you enable the use of an ext interrupt and you configure this driver for multiple
keyboards with the use of the I2C bus the interrupt signals that come from each PCF8574
device must be or'ed with the use of diodes. The pull up resistor can be an external one
or the AVR's internal but you must enable it your self by writting 1 to the specific DDR register.
I prefer an external one of 10 Kohm since this will pull up the pin as soon as Vcc is applyied.
Actually the diodes might not be nessesary as the PCF8574 INT pin is an open drain output.

                                   Vcc
                                    |
                                  pullup
                                  resistor
                                  10 Kohm
                      1N4148        |
   pcf8574 #1 ----------|<|---|     |
                              |-----X-----> to AVR's external interrupt 
   pcf8574 #2 ----------|<|---|
 


*/
 

#ifndef HEX_KEYB_H
#define HEX_KEYB_H

/*######################################################################################################*/
/*                              START OF CONFIGURATION BLOCK                                            */
/*######################################################################################################*/


#ifndef F_CPU
#define F_CPU                         3686400    /* CPU CLOCK FREQUENCY                                  */
#endif
#define HEX_KEYBOARD_TYPE             0          /* 0=your own type, 1=PACTEC, 2=VELLEMAN                */
#define HEX_KEYB_WILL_USE_INTERRUPT   0          /* 0=NO external interrupt, 1=Use an external interrupt */  
#define HEX_KEYB_INTERRUPT            0          /* The external interrupt number.                       */
#define CURSOR_ADVANCE_TIME           20         /* In tenth's of a second. Valid if autoadvance is on.  */
#define HEX_KEYB_WILL_USE_I2C         0          /* 1= use the I2C bus, 0=use an AVR port.               */
#define NUMBER_OF_HEX_KEYBOARDS       1          /* Total keyb's used. Valid only if you use the I2C bus.*/

/* The I2C bus address. Select addresses that do not interfere with other devices (lcd's etc.) 
   Valid only if "HEX_KEYB_WILL_USE_I2C == 1". In this case port definitions that follow are inactive.
   If you select to have 1 keyboard then only "HEX_KEYB_0_I2C_ADDRESS" is used, likewise if you select
   to have 2 keyboards then "HEX_KEYB_0_I2C_ADDRESS" and "HEX_KEYB_1_I2C_ADDRESS" are used and so on.
*/
#define HEX_KEYB_0_I2C_ADDRESS        0x70       /* The first keyboard i2c address   */
#define HEX_KEYB_1_I2C_ADDRESS        0x72       /* The second keyboard i2c address  */
#define HEX_KEYB_2_I2C_ADDRESS        0x74       /* The third keyboard i2c address   */
#define HEX_KEYB_3_I2C_ADDRESS        0x76       /* The fourth keyboard i2c address  */
#define HEX_KEYB_4_I2C_ADDRESS        0x78       /* The fifth keyboard i2c address   */
#define HEX_KEYB_5_I2C_ADDRESS        0x7A       /* The sixth keyboard i2c address   */
#define HEX_KEYB_6_I2C_ADDRESS        0x7C       /* The seventh keyboard i2c address */
#define HEX_KEYB_7_I2C_ADDRESS        0x7E       /* The eigth keyboard i2c address   */

/* Ways to save code by disabling unused code blocks */
#define ACTIVATE_ARROWS               1          /* arrow keys <-- and --> ON or OFF                 */
#define ACTIVATE_AUTO_ADVANCE         1          /* Cursor auto advance ON or OFF                    */
#define HKEYB_SCAN_FUNCTION_ONLY      0          /* 1= only "scan_keyb(void)" is available           */
#define HKEYB_GETS_NEEDED             1          /* 0=dont compile the hkb_gets() function           */
#define HKEYB_GETI_NEEDED             1          /* 0=dont compile the hkb_geti() function           */
#define HKEYB_GETC_NEEDED             1          /* 0=dont compile the hkb_getc() function           */

/*######################################################################################################*/
/*                                   KEYBOARD KEY DEFINITIONS                                           */
/*       IF YOU SELECTED THE KEYBOARD TO USE THE I2C BUS THEN THE PORT DEFINITIONS ARE INACTIVE         */
/*######################################################################################################*/

/********************************************************************************************************/
/* CONFIGURATION OF THE BELOW LINES ARE ONLY NESSESARY IF YOU SELECTED THE KEYBOARD TYPE TO BE 0        */
/*                             START OF KEYBOARD TYPE 0 CONFIGURATION                                   */
/********************************************************************************************************/

#if HEX_KEYBOARD_TYPE == 0     /* YOUR OWN TYPE KEYBOARD ID SETTINGS (0) */

#define HEX_KEYBOARD_PORT  B   /* THE KEYBOARD PORT (A,B,C,D,etc.). Only nessesary when not in I2C bus. */ 

/* PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  */
/*    R or C      bit position                            */  
#define COL1_PORT       HEX_KEYBOARD_PORT
#define COL1_PIN        0
#define COL2_PORT       HEX_KEYBOARD_PORT
#define COL2_PIN        1     
#define COL3_PORT       HEX_KEYBOARD_PORT
#define COL3_PIN        2  
#define COL4_PORT       HEX_KEYBOARD_PORT
#define COL4_PIN        3
#define ROW1_PORT       HEX_KEYBOARD_PORT
#define ROW1_PIN        4
#define ROW2_PORT       HEX_KEYBOARD_PORT
#define ROW2_PIN        5
#define ROW3_PORT       HEX_KEYBOARD_PORT
#define ROW3_PIN        6
#define ROW4_PORT       HEX_KEYBOARD_PORT
#define ROW4_PIN        7   

/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            'A'
#define KEY5            '4'
#define KEY6            '5'
#define KEY7            '6'
#define KEY8            'B'
#define KEY9            '7'
#define KEY10           '8'
#define KEY11           '9'
#define KEY12           'C'
#define KEY13           '*'
#define KEY14           '0'
#define KEY15           '#'
#define KEY16           'D'                      

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'D'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       '*'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      '#'             /* DEFINE MOVE RIGHT arrow key for kgets()           */
#define HKB_CHAR_KEY_1  '1'             /* The first key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_2  '2'             /* The second key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_3  '3'             /* The third key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_4  '4'             /* The fourth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_5  '5'             /* The fifth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_6  '6'             /* The sixth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_7  '7'             /* The seventh key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_8  '8'             /* The eighth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_9  '9'             /* The ninth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_10 '0'             /* The tenth key that hkb_gets() will use for entering chars */

#endif

/********************************************************************************************************/
/*                           END OF KEYBOARD TYPE 0 CONFIGURATION                                       */
/********************************************************************************************************/

/********************************************************************************************************/
/* CONFIGURATION OF THE BELOW LINES ARE ONLY NESSESARY IF YOU SELECTED THE KEYBOARD TYPE TO BE 1        */
/*                             START OF KEYBOARD TYPE 1 CONFIGURATION                                   */
/********************************************************************************************************/

#if HEX_KEYBOARD_TYPE == 1

#define HEX_KEYBOARD_PORT  B   /* THE KEYBOARD PORT (A,B,C,D,etc.). Only nessesary when not in I2C bus. */
/*
                       PACTEC SELF ADHESIVE 4X4 KEYBOARD
                |----------------------------------------------------|
                |   KEYB PINS  R3  R4 C3  C4  C1  C2   R2  R1        |
                |   PORT BITS  7   6   5   4   3   2    1   0        |    
                |----------------------------------------------------|        

   PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  
         R or C          bit position    
*/  
#define COL1_PORT       HEX_KEYBOARD_PORT
#define COL1_PIN        3
#define COL2_PORT       HEX_KEYBOARD_PORT
#define COL2_PIN        2     
#define COL3_PORT       HEX_KEYBOARD_PORT
#define COL3_PIN        5  
#define COL4_PORT       HEX_KEYBOARD_PORT
#define COL4_PIN        4
#define ROW1_PORT       HEX_KEYBOARD_PORT
#define ROW1_PIN        0
#define ROW2_PORT       HEX_KEYBOARD_PORT
#define ROW2_PIN        1
#define ROW3_PORT       HEX_KEYBOARD_PORT
#define ROW3_PIN        7
#define ROW4_PORT       HEX_KEYBOARD_PORT
#define ROW4_PIN        6   
/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            '4'
#define KEY5            '5'
#define KEY6            '6'
#define KEY7            '7'
#define KEY8            '8'
#define KEY9            '9'
#define KEY10           '0'
#define KEY11           'A'
#define KEY12           'B'
#define KEY13           'C'
#define KEY14           'D'
#define KEY15           'E'
#define KEY16           'F'                

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'F'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       'D'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      'E'             /* DEFINE MOVE RIGHT arrow key for kgets()           */
#define HKB_CHAR_KEY_1  '1'             /* The first key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_2  '2'             /* The second key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_3  '3'             /* The third key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_4  '4'             /* The fourth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_5  '5'             /* The fifth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_6  '6'             /* The sixth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_7  '7'             /* The seventh key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_8  '8'             /* The eighth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_9  '9'             /* The ninth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_10 '0'             /* The tenth key that hkb_gets() will use for entering chars */

#endif                    
/********************************************************************************************************/
/*                           END OF KEYBOARD TYPE 1 CONFIGURATION                                       */
/********************************************************************************************************/

/********************************************************************************************************/
/* CONFIGURATION OF THE BELOW LINES ARE ONLY NESSESARY IF YOU SELECTED THE KEYBOARD TYPE TO BE 1        */
/*                             START OF KEYBOARD TYPE 2 CONFIGURATION                                   */
/********************************************************************************************************/
#if HEX_KEYBOARD_TYPE == 2

#define HEX_KEYBOARD_PORT  B   /* THE KEYBOARD PORT (A,B,C,D,etc.). Only nessesary when not in I2C bus. */
/*
                       VELLEMAN PLASTIC 4X4 KEYBOARD
                |----------------------------------------------------|
                |   KEYB PINS  R4  R3 R2  R1  C4  C3   C2  C1        |
                |   PORT BITS  7   6   5   4   3   2    1   0        |    
                |----------------------------------------------------|        


   PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  
         R or C          bit position    
*/  
#define COL1_PORT       HEX_KEYBOARD_PORT
#define COL1_PIN        0
#define COL2_PORT       HEX_KEYBOARD_PORT
#define COL2_PIN        1     
#define COL3_PORT       HEX_KEYBOARD_PORT
#define COL3_PIN        2  
#define COL4_PORT       HEX_KEYBOARD_PORT
#define COL4_PIN        3
#define ROW1_PORT       HEX_KEYBOARD_PORT
#define ROW1_PIN        4
#define ROW2_PORT       HEX_KEYBOARD_PORT
#define ROW2_PIN        5
#define ROW3_PORT       HEX_KEYBOARD_PORT
#define ROW3_PIN        6
#define ROW4_PORT       HEX_KEYBOARD_PORT
#define ROW4_PIN        7   

/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            'A'
#define KEY5            '4'
#define KEY6            '5'
#define KEY7            '6'
#define KEY8            'B'
#define KEY9            '7'
#define KEY10           '8'
#define KEY11           '9'
#define KEY12           'C'
#define KEY13           '*'
#define KEY14           '0'
#define KEY15           '#'
#define KEY16           'D'                      

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'D'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       '*'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      '#'             /* DEFINE MOVE RIGHT arrow key for kgets()           */
#define HKB_CHAR_KEY_1  '1'             /* The first key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_2  '2'             /* The second key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_3  '3'             /* The third key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_4  '4'             /* The fourth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_5  '5'             /* The fifth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_6  '6'             /* The sixth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_7  '7'             /* The seventh key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_8  '8'             /* The eighth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_9  '9'             /* The ninth key that hkb_gets() will use for entering chars */
#define HKB_CHAR_KEY_10 '0'             /* The tenth key that hkb_gets() will use for entering chars */

#endif
/********************************************************************************************************/
/*                           END OF KEYBOARD TYPE 2 CONFIGURATION                                       */
/********************************************************************************************************/

/********************************************************************************************************/
/*                           END OF KEYBOARD KEY CONFIGURATION                                          */
/********************************************************************************************************/

/* Lcd driver information (you only need to change it if you decide to use your own lcd driver)      */
/* THE BELOW DEFINITIONS ARE NEEDED BY BOTH THE TEST PROGRAM AND THE DRIVER! */
/* name your lcd driver header file below */
#if HKEYB_SCAN_FUNCTION_ONLY == 0

#include                       "lcd_io.h"         
#ifndef LCD_CHARS_PER_LINE
/* Define your lcd characters per line size below                       */
#define LCD_CHARS_PER_LINE     20
#endif
#ifndef LCD_LINES
/* Define your lcd lines number below                                   */
#define LCD_LINES              4
#endif
/* name your lcd driver goto_xy function below                          */
#define LCD_GOTOXY(x,y)        lcd_gotoxy(x,y)    
/* name your lcd function to make the cursor visible and blinking below */
#define LCD_CURSOR_ON_BLINK()  lcd_command(LCD_DISP_ON_BLINK)
/* name your lcd function to turn off the cursor  below                 */
#define LCD_CURSOR_OFF()       lcd_command(LCD_DISP_ON)
/* name your lcd put char function below                                */
#define LCD_PUTC(x)            lcd_putc(x)        

/*********************************************************************************************************/
/* THE BELOW DEFINITIONS ARE NEEDED ONLY BY THE TEST PROGRAM AND NOT BY THE KEYBOARD DRIVER! */
/* EDIT THEM AS NEEDED BY YOUR ACTUAL APPLICATION OR USE THE ACTUAL COMMANDS!                */
/*********************************************************************************************************/

/* name your lcd initialization function below                          */
#define LCD_INIT()             lcd_init()
/* name your lcd clear screen function below                            */
#define LCD_CLRSCR()           lcd_clrscr()
/* name your lcd put string function below                              */
#define LCD_PUTS(x)            lcd_puts(x)
/* name your lcd put integer function below                             */
#define LCD_PUTI(x,y)          lcd_puti(x,y)
/* name your lcd put progmem string function below                      */
#define LCD_PUTS_P(x)          lcd_puts_P(x)
/* name your lcd put eeprom string function below                       */
#define LCD_PUTS_E(x)          lcd_puts_e(x)

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/*######################################################################################################*/
/*                                END OF CONFIGURATION BLOCK                                            */
/*######################################################################################################*/

#if HEX_KEYB_WILL_USE_I2C == 0
#undef  NUMBER_OF_HEX_KEYBOARDS 
#define NUMBER_OF_HEX_KEYBOARDS   1 
#endif

#if HKEYB_SCAN_FUNCTION_ONLY == 0

/* FUNCTION PROTOTYPES */

/* Sets the initial port and pin states. Usefull only in interrupt mode. */
#if HEX_KEYB_WILL_USE_INTERRUPT == 1
/* 
    Initialize the keyboard port and pins and turn on or off the external interrupt.
    The first argument passed is a pointer to a "void function_name(unsigned char key_code)"
    function type and the second is turning the keyboard interrupt on or off.
    1=ON, 0=OFF. 
*/
extern void          hkb_init(void (*execute_this_function)(unsigned char key_code),
                              unsigned char on_off);
#endif

#if NUMBER_OF_HEX_KEYBOARDS >=  2
extern unsigned char hkb_select(unsigned char hkb_unit_number);
#endif

/*
   waits untill it gets a string + enter of n chars displayed at column lcd_x and row lcd_y.    
   Returns the number of chars entered. Escape returns 0 so you can test if the data are valid.
*/                                          
#if HKEYB_GETS_NEEDED == 1
extern void          hkb_gets( unsigned char *data, unsigned char max_chars,
                               unsigned char lcd_x, unsigned char lcd_y      );
#endif
/* 
   waits untill it gets a number of n digits displayed at column lcd_x and row lcd_y 
   If digits is set to 0 the function enters password mode where the user can input  
   a 4 digit number but the display will show **** (stars) instead of the actual number
*/
#if HKEYB_GETI_NEEDED == 1
extern unsigned char hkb_geti(int *data, unsigned char digits, unsigned char lcd_x, unsigned char lcd_y);
#endif
                             
/* waits untill it gets a character */
#if HKEYB_GETC_NEEDED == 1
extern unsigned char hkb_getc(void);                            
#endif

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/* 
   The low level keyboard scanning function that scans the keyboard.            
   returns the scan codes defined earlier in the file or 0 if no key is pressed 
   NOTE: IT DOES NOT WAIT FOR A KEY NOR USES THE LCD DRIVER 
*/
extern unsigned char hkb_scan(void);

#if HKEYB_SCAN_FUNCTION_ONLY == 0 

/* MACRO DEFINITION  */
#define kgetS(w,x,y,z)  kgets(w,(sizeof(w)-1),y,z)

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

#if HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >=  2

#define hkb_get_id()   hkb_select(0xFF)

/* Hex keyboard selection keywords. Valid only if you use the I2C bus and more than one keyboard. */
#define HKB_0        0   
#define HKB_1        1
#define HKB_2        2
#define HKB_3        3
#define HKB_4        4
#define HKB_5        5
#define HKB_6        6
#define HKB_7        7

#endif   /* #if HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >=  2 */

#endif /* HEX_KEYB_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

