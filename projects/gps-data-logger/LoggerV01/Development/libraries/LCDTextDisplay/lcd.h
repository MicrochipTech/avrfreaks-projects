// LCD.H
#ifndef LCD_Header
#define LCD_Header
#include <inttypes.h>

#define LCD_LINES           2     // visible lines 
#define LCD_LINE_LENGTH  0x40     // internal line length 
#define LCD_START_LINE1  0x00     // DDRAM address of first char of line 1 
#define LCD_START_LINE2  0x40     // DDRAM address of first char of line 2 


// 7 bits are used
// bits 0..3 are the data bits. They are toggeled from input to output
//								as necessary (ie they are read/write)
// bits 4..6 are control bits.  Always output

//one if the LCS_PORT_IS_X must be defined in the makefile, this decides on the 
//port compiled into the object
//to define thisin the make file use:   -Wp,-DLCDPORTISA

#if defined (LCDPORTISA)
#define LCD_PORT         PORTA    // port for all LCD io
#define LCD_PORT_DDR     DDRA
#define LCD_PORT_PIN     PINA
#elif defined (LCD_PORT_IS_C)
#define LCD_PORT         PORTC     // port for all LCD io
#define LCD_PORT_DDR     DDRC
#define LCD_PORT_PIN     PINC
#endif


#define LCD_RS_BIT       4
#define LCD_RW_BIT       5
#define LCD_E_BIT        6

// lower bits change from input to output as needed

#define LCD_PORT_FOR_OUTPUT 0xFF  //DD for sending data to LCD
#define LCD_PORT_FOR_INPUT  0xF0  //DD for reading data from LCD

// define the commands for the LCD

#define _DB0                 0x01
#define _DB1                 0x02
#define _DB2                 0x04
#define _DB3                 0x08
#define _DB4                 0x10
#define _DB5                 0x20
#define _DB6                 0x40
#define _DB7                 0x80


#define RS0                 0
#define RS1                 1

#define LCD_BUSY            _DB7		//busy is a bit within status response

#define LCD_CLR             _DB0
#define LCD_HOME            _DB1
#define LCD_DDRAM           _DB7


// set entry mode: display shift on/off, dec/inc cursor move direction 
#define LCD_ENTRY_DEC            (_DB2)              // display shift off, dec cursor move dir
#define LCD_ENTRY_DEC_SHIFT      (_DB2 | _DB0)        // display shift on,  dec cursor move dir
#define LCD_ENTRY_INC            (_DB2 | _DB1)        // display shift off, inc cursor move dir
#define LCD_ENTRY_INC_SHIFT      (_DB2 | _DB1 | _DB0)  // display shift on,  inc cursor move dir

// display on/off, cursor on/off, blinking char at cursor position
#define LCD_DISP_OFF             (_DB3)                     // display off                            
#define LCD_DISP_ON              (_DB3 | _DB2)               // display on, cursor off
#define LCD_DISP_ON_BLINK        (_DB3 | _DB2 | _DB0)         // display on, cursor off, blink char
#define LCD_DISP_ON_CURSOR       (_DB3 | _DB2 | _DB1)         // display on, cursor on
#define LCD_DISP_ON_CURSOR_BLINK (_DB3 | _DB2 | _DB1 | _DB0)   // display on, cursor on, blink char

// move cursor/shift display 
#define LCD_MOVE_CURSOR_LEFT     (_DB4)             // move cursor left  (decrement)
#define LCD_MOVE_CURSOR_RIGHT    (_DB4 | _DB2)       // move cursor right (increment)
#define LCD_MOVE_DISP_LEFT       (_DB4 | _DB3)       // shift display left   
#define LCD_MOVE_DISP_RIGHT      (_DB4 | _DB3 | _DB2) // shift display right  


// function set: set interface data length and number of display lines
#define LCD_FUNCTION_INIT   	   (_DB5 )   // 4-bit interface, dual line,   5x8 dots
#define LCD_FUNCTION_INIT_2LINES   (_DB5 | _DB3)   // 4-bit interface, dual line,   5x8 dots

 
// function prototypes 

extern void LCDCommand(uint8_t cmd);
extern void LCDGotoXY(uint8_t x, uint8_t y);
extern void LCDClrscr(void);
extern void LCDHome(void);
extern void LCDPutc(char c);
extern void LCDPuts(const char *s);
extern void LCDPutsN(const char *s, uint8_t n);

extern void LCDPuts_P(const char *progmem_s);
extern void LCDInit(uint8_t dispAttr);



#endif //LCD_Header
