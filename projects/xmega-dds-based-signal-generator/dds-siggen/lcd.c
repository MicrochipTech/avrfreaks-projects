// -------------------------------------------------------------------------------------
// lcd.c file for a 16*2 char LCD interfaced with the ATMEL ATMega32 MCU
// By - Nandan Banerjee
// Updated by - Carlos E. Marciales
// Date - 08/02/2010
//
// Updated by - Frank Van Hooft, Sept 2010 to support Xmega,
// and to add a new function and a .h file
//
// This code only supports 8-bit mode, and it only writes to the LCD.
// It uses simple delays for timing (no reading of the busy flag).
//
// Compiled using AVR-GCC (WinAVR)
// IDE - AVRStudio 4.18
 
// 8-bit communication mode for the HD44780
// Be sure to set the Frequency field in project properties
// To modify this file, edit the #defines below, and also make any
// necessary changes to lcd_init at the bottom of this file.
 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
 
// This implementation uses Port A for the Data port, all 8 bits of it
#define DATA_PORT PORTA
 
 
// This implementation uses Port B for the Control port
#define LCD_RS PIN3_bm		/* RS on pin PB3 */
#define LCD_E  PIN1_bm		/* E on pin PB1 */
#define COMM_PORT PORTB
 
 
// This function clears the RS line to write a command
void lcd_set_write_instruction() {
	COMM_PORT.OUTCLR = LCD_RS;		// set RS line low 
	_delay_us(50);
}
 
 
// This function sets the RS line to write data
void lcd_set_write_data() {
	COMM_PORT.OUTSET = LCD_RS;		// set RS line high
	_delay_us(50);
}
 
 
 
// This function writes a byte to the LCD
void lcd_write_byte (char c) {
	DATA_PORT.OUT = c;			// Place data on Data Port
	COMM_PORT.OUTSET = LCD_E;		// set E line high
	_delay_us(1);
	COMM_PORT.OUTCLR = LCD_E;		// set E line low to latch data into LCD
	_delay_ms(10);
}
 
 
// This function clears LCD and sets address to beginning of first line
void lcd_clear_and_home() {
	lcd_set_write_instruction();
	lcd_write_byte(0x01);
	_delay_ms(50);
	lcd_write_byte(0x02);
	_delay_ms(50);
}
 
 
// This function sets address to beginning of first line
	void lcd_home() {
	lcd_set_write_instruction();
	lcd_write_byte(0x02);
	_delay_ms(50);
}
 
 
// This function moves cursor to a given line and position
//  line is either 0 (first line) or 1 (second line)
//  pos is the character position from 0 to 15.
void lcd_goto(uint8_t line, uint8_t pos)
{
	uint8_t position = 0;
 
	lcd_set_write_instruction();
	switch(line)
	{
		case 0: position = 0;
		break;
		case 1: position = 0x40;
		break;
	}
		lcd_write_byte(0x80 | (position + pos));
	}
 
 
// This function moves the cursor to 1st character of 1st line
void lcd_line_one() { lcd_goto(0, 0); }
 
 
// This function moves the cursor to 1st character if 2nd line
void lcd_line_two() { lcd_goto(1, 0); }
 
 
// This function writes a character to the LCD
void lcd_write_data(char c) {
	lcd_set_write_data();
	lcd_write_byte(c);
}
 
 
// This function writes a string (in SRAM) of given length to the LCD
void lcd_write_string(char *x, uint8_t len ) {
	while (--len > 0)
		lcd_write_data(*x++);
}
 
 
// This function writes a null-terminated string (in SRAM) to the LCD
void lcd_write_string_0(char *x) {
	while (*x)
		lcd_write_data(*x++);
}
 
 
// Same as above, but the string is located in program memory,
//  so "lpm" instructions are needed to fetch it, and a \0
//  must be defined at the end of the string to terminate it.
void lcd_write_string_p(const char *s)
{
	char c;
 
	for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
		lcd_write_data(c);
}
 
 
// This function initializes the LCD plus any AVR ports etc
void lcd_init() {
 
	// Initialise the AVR ports and any other hardware bits
	DATA_PORT.OUT = 0x00;							// initial data lines all low
	DATA_PORT.DIRSET = 0xff;						// set the 8-bit data port to all outputs
	COMM_PORT.OUTCLR = LCD_E | LCD_RS;			// all LCD control lines low
	COMM_PORT.DIRSET = LCD_E | LCD_RS;  			// set the LCD control line pins to outputs
 
	// Now do the actual LCD initialisations
 
	// startup delay - make it long to allow time for power to settle
	// (you want wish to remove this line)
	_delay_ms(500);
 
	// LCD display initialisation by instruction magic sequence
	lcd_write_byte(0x38);			// function set
	_delay_us(50);
	lcd_write_byte(0x0c);			// display on/off control
	_delay_us(50);
	lcd_write_byte(0x01);			// display clear
	_delay_ms(5);
	lcd_write_byte(0x06);			// entry mode set
	_delay_ms(5);
 
	lcd_write_byte(0x14); 			// Cursor shift
 
	lcd_clear_and_home(); 			// LCD cleared and cursor is brought to 
         						// the beginning of 1st line
}
