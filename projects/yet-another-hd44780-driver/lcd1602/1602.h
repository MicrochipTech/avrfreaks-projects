/*
 * lcd1602.h
 *
 * Created: 2012-12-24 20:32:27
 *  Author: George Robinson
 */ 

// this wee convention prevents definitions from being made twice
#ifndef LCD1602_H_
#define LCD1602_H_

/* Interface selection:
**		Define LCD_4BIT for 4-bit interface, will use DB4-DB7 as defined below 
**			(DB0-DB3 will be ignored and untouched, use your MCU ports for other more useful things!)
**		Define LCD_8BIT for 8-bit interface, will use DB0-DB7 as defined below
**		If you're having trouble getting stuff to work, start with the full 8-bit interface and work backwards from there...
**		Other than confusion, there's no compelling reason not to use the 4-bit interface.
**		Compiler should complain if you define both / don't define either, but best not. :)
*/
//#define LCD_4BIT
#define LCD_8BIT

/* General data port assignment:
**		Provided as convenience, these definitions will be referenced in each bit's definition.
**		Makes sure that the PORT register is consistent with the DDR register (e.g. PORTD and DDRD)
*/
#define DATA_PORT PORTD
#define DATA_DDR DDRD

/* DBx assignments
**		May use the general data ports above, or specify each individually
**		Although fully-configurable ports make the program hungrier,
**		this program will drop easily into many hardware configurations.
*/
// DB0, pin 7
#define DB0_PORT DATA_PORT
#define DB0_DDR DATA_DDR
#define DB0_BIT 0

// DB1, pin 8
#define DB1_PORT DATA_PORT
#define DB1_DDR DATA_DDR
#define DB1_BIT 1

// DB2, pin 9
#define DB2_PORT DATA_PORT
#define DB2_DDR DATA_DDR
#define DB2_BIT 2

// DB3, pin 10
#define DB3_PORT DATA_PORT
#define DB3_DDR DATA_DDR
#define DB3_BIT 3

// DB4, pin 11
#define DB4_PORT DATA_PORT
#define DB4_DDR DATA_DDR
#define DB4_BIT 4

// DB5, pin 12
#define DB5_PORT DATA_PORT
#define DB5_DDR DATA_DDR
#define DB5_BIT 5

// DB6, pin 13
#define DB6_PORT DATA_PORT
#define DB6_DDR DATA_DDR
#define DB6_BIT 6

// DB7, pin 14
#define DB7_PORT DATA_PORT
#define DB7_DDR DATA_DDR
#define DB7_BIT 7

// Enable signal (strobe) port assignment (pin 6 on most LCD modules)
#define E_PORT PORTB
#define E_DDR DDRB
#define E_BIT 0

// Register select signal (RS) port assignment (pin 4 on most LCD modules)
#define RS_PORT PORTB
#define RS_DDR DDRB
#define RS_BIT 1

// *** FIXED(ISH) DEFINITIONS ***

/* Constants for first character of each line
**		By default, set up for standard 16x2 displays. Probably also ok for most 2-line displays.
**		May need to be altered + extended for other displays (i.e. 4-line)
**		These constants are not referenced in lcd1602.c, but feel free to use in your code to make it nice and readable!
*/
#define LCD_LINE1 0x00
#define LCD_LINE2 0x40

/* Command constants
**		These are necessarily consistent between different chips.
**		I've arranged them by function, followed by variables you can OR (|) with them
*/
#define LCD_FUNC_CLEAR 0x01 // clears the display, and returns cursor and DD addr to 0

#define LCD_FUNC_HOME 0x02 // returns the cursor and DD address to 0, without changing display data

#define LCD_FUNC_ENTRYMODE 0x04 // set up the entry mode
#define LCD_VAR_AUTOINC 0x02 // auto-increment DD addr with each character written
#define LCD_VAR_AUTOSHIFT 0x01 // auto-shift display (see FUNC_SHIFT below) with each character written

#define LCD_FUNC_ONOFF 0x08 // turns display and elements on or off
#define LCD_VAR_DISPON 0x04 // display
#define LCD_VAR_CURSON 0x02 // cursor - little underline shown at DD addr
#define LCD_VAR_BLINKON 0x01 // blink - whole character blinks on and off at DD addr

#define LCD_FUNC_SHIFT 0x10 // move the cursor or shift the display
#define LCD_VAR_SHIFT 0x08 // shift display, rather than move cursor
#define LCD_VAR_RIGHT 0x04 // shift to right, rather than left
	// e.g. lcdFunc(LCD_FUNC_SHIFT | LCD_VAR_RIGHT); will move the cursor (and DD addr) one character to the right
	// e.g. lcdFunc(LCD_FUNC_SHIFT | LCD_VAR_SHIFT); will shift the display (both lines!) to the left by one character

#define LCD_FUNC_FUNCTIONSET 0x20 // init command, used for reset and initial config (you probably don't need to use it)
#define LCD_VAR_8BIT 0x10 // select 8-bit input mode, rather than 4-bit
#define LCD_VAR_2LINE 0x08 // set 2-line mode, rather than 1-line
#define LCD_VAR_10DOT 0x04 // set 10-dot-high characters, rather than the more usual 7-dot
	// note these variables all pertain to the physical construction of the device, so will be constant for a particular unit
	
#define LCD_FUNC_SETCGADDR 0x40 // move data input pointer to a specific character graphics (CG) address
#define LCD_MASK_CGADDR 0x3F // & this with your address to prevent overrun problems!
	// used for writing custom characters to CG ram
	
#define LCD_FUNC_SETDDADDR 0x80 // move data input pointer to a specific display data (DD) address
#define LCD_MASK_DDADDR 0x7F // & this with your address to prevent overrun problems!
	// used to move cursor position - e.g. by the provided lcdSetPos(uint8_t pos); function

/* Timing constants
**		These are used in the strobing functions to make the timing work. Refer to chip/module datasheet for specific values.
**		We make use of the <util/delay.h> library to make in-line delays
**		I'd warn against being to tricky with timers for these delays (e.g. to keep goind
**			with other background work) as the interrupt statefulness gets complicated
**		Note the units for each (variously ms (millisecond, 1/1000 second) and us (microsecond, 1/1000000 second))
**		1us = 1 CPU cycle at 1 MHz - so even very small times should work well for f >= 1 MHz
*/
#define LCD_TIMEOUT_FUNCTION 1800 // us, time to wait after submitting a control function (e.g. display clear, set cursor, etc.)
#define LCD_TIMEOUT_DATA 50 // us, time to wait after submitting a character - from the datasheet, this is much quicker than most functions
#define LCD_TIMEOUT_SETUP 1 // us, time to wait for data to settle before strobing E (minimum setup time O(ns))
#define LCD_TIMEOUT_HOLD 1 // us, time to hold E low (minimum hold time O(ns))
#define LCD_TIMEOUT_POWERUP 40 // ms, time to wait before reset commands are set - if you implement this delay yourself, feel free to make this 0
#define LCD_TIMEOUT_RESETS 5

/* Convenience functions for switching bits
**		There should be no need to edit these here, they reference the definitions above.
**		Be careful not to accidentally play with these in your user code.
*/
// data ports 0-3 (not used in 4 bit mode)
#define DB0_HI() DB0_PORT|=(1<<DB0_BIT)
#define DB0_LO() DB0_PORT&=~(1<<DB0_BIT)
#define DB1_HI() DB1_PORT|=(1<<DB1_BIT)
#define DB1_LO() DB1_PORT&=~(1<<DB1_BIT)
#define DB2_HI() DB2_PORT|=(1<<DB2_BIT)
#define DB2_LO() DB2_PORT&=~(1<<DB2_BIT)
#define DB3_HI() DB3_PORT|=(1<<DB3_BIT)
#define DB3_LO() DB3_PORT&=~(1<<DB3_BIT)
 // data ports 4-7 (used in both 8/4 bit modes)
#define DB4_HI() DB4_PORT|=(1<<DB4_BIT)
#define DB4_LO() DB4_PORT&=~(1<<DB4_BIT)
#define DB5_HI() DB5_PORT|=(1<<DB5_BIT)
#define DB5_LO() DB5_PORT&=~(1<<DB5_BIT)
#define DB6_HI() DB6_PORT|=(1<<DB6_BIT)
#define DB6_LO() DB6_PORT&=~(1<<DB6_BIT)
#define DB7_HI() DB7_PORT|=(1<<DB7_BIT)
#define DB7_LO() DB7_PORT&=~(1<<DB7_BIT)
// E
#define E_HI() E_PORT|=(1<<E_BIT)
#define E_LO() E_PORT&=~(1<<E_BIT)
// RS
#define RS_HI() RS_PORT|=(1<<RS_BIT)
#define RS_LO() RS_PORT&=~(1<<RS_BIT)

// *** INCLUSIONS ***

#include <avr/io.h>
#include <util/delay.h>

// *** FUNCTIONS ***
// Anything not done here can usually be done easily with a quick call to lcdFunc(...);

/* void lcdInit(void)
**		Sets up AVR IO to control LCD, as per above definitions
**		Resets and configures LCD for chosen mode of operation
**		Clears screen and turns the display on ready for text!
**		Should be called once, after power-up
**		If you are having trouble making this work, please check your wiring and i/o definitions above very carefully,
**			and _then_ start playing with the init routine in lcd1602.c...
*/
void lcdInit();

/* void lcdFunc(func)
**		Sends a command to the LCD - use for setup and fiddling with complex bits
*/
void lcdFunc(uint8_t func);

/* void lcdData(uint8_t data)
**		Nearly identical to lcdFunc, but sets the RS signal for a data transmission
**		Use this for both writing display data and writing character data
**		Will write to whatever address is currently set (could be either DD or CG)
**		Auto-incrementing should work with both DD and CG data
*/
void lcdData(uint8_t data);

/* void lcdSetPos(int pos)
**    Sets the position of the write cursor. Note constants:
**		LCD_LINE1
**		LCD_LINE2
*/
void lcdSetPos(uint8_t pos);

/* void lcdWrite(uint8_t* test, uint8_t length)
**	    Writes [length] characters of the string [text] to the screen.
**      No overflow protection - some say be careful, but the worst case is you
**			write a bunch of garbage across the screen, maybe even across lines.
**			You can't harm the LCD, and neither will reading random registers tend to harm the AVR.
*/
void lcdWrite(uint8_t* text, uint8_t length);

#endif /* LCD1602_H_ */