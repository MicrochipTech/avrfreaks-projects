/*
 * lcd1602.c
 *
 * Created: 2012-12-24 20:31:57
 *  Author: George Robinson
 *	
 *  Don't pay too much attention to this file, all the configurable stuff is in lcd1602.h
 *  On the other hand, this is extensively commented for your reading pleasure.
 */ 

#include "lcd1602.h"
//#include <avr/interrupt.h>	// Uncomment this if you are using interrupts in your code, and for reliability
								// want to disable interrupts around the pair of 4-bit instructions (see function lcdSendByte below)

#ifdef LCD_4BIT

/* 4-bit interface internal data transmit functions
**		Note the #ifdef - this code only gets complied if you've asked for a 4-bit interface
**			otherwise you just get the boring 8-bit version.
*/
void lcdSendNibble(uint8_t nibble) // 'nibble' is the technical term for 4-bits (half a 'byte')
{
	// here, we bash out the 4 least significant bits of the byte provided
	// i.e. 0b####EFGH
	
	// E_HI(); // totally unnecessary, but if you're an ass-out-of-u-and-me kind of person (see footnote ##), then go for it.
	
	// set/clear DB4 output (LSB, H)
	if (nibble & 0x01)
		DB4_HI();
	else
		DB4_LO();
	
	// set/clear DB5 output (G)
	if (nibble & 0x02)
		DB5_HI();
	else
		DB5_LO();

	// set/clear DB6 output (F)
	if (nibble & 0x04)
		DB6_HI();
	else
		DB6_LO();
	
	// set/clear DB7 output (MSB, E)
	if (nibble & 0x08)
		DB7_HI();
	else
		DB7_LO();
			
	_delay_us(LCD_TIMEOUT_SETUP); // data-hold time before strobing E
	E_LO();			// strobe E low (this is where the data flows into the LCD controller)
	_delay_us(LCD_TIMEOUT_HOLD); // hold E low
	E_HI();			// bring E back high (see footnote ##)
	
	// Note that I'm leaving the data as it was here - there's no reason to zero it between instructions
	// as long as it's valid when I need it to be!
}

/* FOOTNOTE ## Assumptions made when strobing E
**		'Strobe' means to flick on or off briefly, used to clock or synchronise some process
**		I make the assumption that E is high in the function above
**		This is valid as long as:
**			(1) E is high at the start of proceedings
**			(2) Every time E is pulled low, it is then directly returned high
**		If you're uncomfortable with this logic, please uncomment the useless "E_HI();" in the strobe functions... :D
*/

/* 4-bit method of sending an 8-bit instruction (which we use when things are underway)
**		High nibble sent first, then low nibble
**		We have to be careful that this is always completed, as if we miss one nibble the
**			whole thing will go out of whack. I would recommend being cautious about interrupts
**			when sending LCD commands, as this may cause such an 'interruption'.
**			E.g. An easy approach would be to start most functions with cli();, then end with sei();, such that they are guaranteed to complete
**			I would add it in the functions here, but it really depends on whether you are using interrupts - if not, an unexpected sei(); might make things go funny for you.
*/
void lcdSendByte(uint8_t b)
{
	// cli(); // clear interrupts before sending the pair of instructions, so nothing gets between them...
	lcdSendNibble(b >> 4); // use bitshift to send the high-nibble first (0bABCDEFGH >> 4 = 0b0000ABCD);
	lcdSendNibble(b & 0x0F); // followed by a masking of the low-nibble (although the lcdSendNibble function ignores the high bits anyway)
	// sei(); // reenable interrupts, if you cleared them earlier!
}

#endif /* LCD_4BIT */ // it's convention to put a comment with the #if definition after an #endif


#ifdef LCD_8BIT
/*
** 8-bit sending function
**		Much simpler, a touch quicker, but on balance a total waste of copper and I/O!
*/
void lcdSendByte(uint8_t b)
{
	// E_HI(); // totally unnecessary, but if you're an ass-out-of-u-and-me kind of person (see footnote ##), then go for it.
	
	// Here we bash out all 8 bits on the pin's you lovingly configured in the h-file
	// Not as efficient as carefully arranging your IO along a full PORTD, and writing once, but this lets you work around all the weird and useful pins
	// i.e. 0bABCDEFGH

	// set/clear DB0 output (LSB, H)
	if (b & 0x01)
		DB0_HI();
	else
		DB0_LO();

	// set/clear DB1 output (G)
	if (b & 0x02)
		DB1_HI();
	else
		DB1_LO();
		
	// set/clear DB2 output (F)
	if (b & 0x04)
		DB2_HI();
	else
		DB2_LO();

	// set/clear DB3 output (E)
	if (b & 0x08)
		DB3_HI();
	else
		DB3_LO();
				
	// set/clear DB4 output (D)
	if (b & 0x10)
		DB4_HI();
	else
		DB4_LO();
	
	// set/clear DB5 output (C)
	if (b & 0x20)
		DB5_HI();
	else
		DB5_LO();

	// set/clear DB6 output (B)
	if (b & 0x40)
		DB6_HI();
	else
		DB6_LO();
	
	// set/clear DB7 output (MSB, A)
	if (b & 0x80)
		DB7_HI();
	else
		DB7_LO();
		
	_delay_us(LCD_TIMEOUT_SETUP); // set up time
	E_LO();			// strobe E low
	_delay_us(LCD_TIMEOUT_HOLD); // hold E low
	E_HI();			// bring E back high
	
}
#endif

void lcdFunc(uint8_t func)
{
	RS_LO();
	lcdSendByte(func);
	_delay_us(LCD_TIMEOUT_FUNCTION); // worst case function wait time
}

void lcdData(uint8_t data)
{
	RS_HI();
	lcdSendByte(data);
	_delay_us(LCD_TIMEOUT_DATA); // data write wait time
}

void lcdInit()
{
	// initialise IO according to your lovingly-crafted config in lcd1602.h
	
	// before we start, make sure RS and E are where we want them
	RS_LO();
	E_HI();
	
	RS_DDR |= (1<<RS_BIT); // set RS pin as output
	E_DDR |= (1<<E_BIT); // set E pin as output
	#ifdef LCD_8BIT
		// set DB0-DB3 pins as outputs
		DB0_DDR |= (1<<DB0_BIT);
		DB1_DDR |= (1<<DB1_BIT);
		DB2_DDR |= (1<<DB2_BIT);
		DB3_DDR |= (1<<DB3_BIT);
	#endif
	// set DB4-DB7 pins as outputs
	DB4_DDR |= (1<<DB4_BIT);
	DB5_DDR |= (1<<DB5_BIT);
	DB6_DDR |= (1<<DB6_BIT);
	DB7_DDR |= (1<<DB7_BIT);	
	
	// power on delay
	_delay_ms(LCD_TIMEOUT_POWERUP);
	
	#ifdef LCD_8BIT
		// function set - should only need one in 8-bit mode, nice and simple
		lcdSendByte(LCD_FUNC_FUNCTIONSET | LCD_VAR_8BIT | LCD_VAR_2LINE);
	#endif
	
	#ifdef LCD_4BIT
		/* KS0070 seems to require this exact quantity of reset commands in 4-bit mode
		** note that in most cases, I assume our power supplies are good enough for the
		** auto power-on-init to work prior to this "init-by-instruction" (refer to datasheets)
		*/
		lcdSendNibble((LCD_FUNC_FUNCTIONSET | LCD_VAR_8BIT) >> 4); // sending a nibble on its own is like sending an 8-wire instruction, while ignoring DB0-3. this one is a regular 8-bit function set
		_delay_ms(LCD_TIMEOUT_RESETS);
		lcdSendNibble((LCD_FUNC_FUNCTIONSET | LCD_VAR_8BIT) >> 4); // 8-bit function set
		_delay_ms(LCD_TIMEOUT_RESETS);
		lcdSendNibble(LCD_FUNC_FUNCTIONSET >> 4); // function set, asking for 4-bit, but sent as an 8-bit
		_delay_ms(LCD_TIMEOUT_RESETS);
		lcdSendByte(LCD_FUNC_FUNCTIONSET | LCD_VAR_2LINE); // function set, including additional necessary bits, sent as a pair of 4-bit nibbles
	#endif
	
	_delay_ms(LCD_TIMEOUT_RESETS); // after init timeout
		
	// configure
	lcdFunc(LCD_FUNC_CLEAR); // display clear
	lcdFunc(LCD_FUNC_ENTRYMODE | LCD_VAR_AUTOINC); // entry mode set
}

void lcdSetPos(uint8_t pos)
{
	lcdFunc(LCD_FUNC_SETDDADDR | (pos & LCD_MASK_DDADDR));
}

void lcdWrite(uint8_t* text, uint8_t length)
{
	for (int n = 0; n < length; n++)
	{
		lcdData(text[n]);
	}
}

