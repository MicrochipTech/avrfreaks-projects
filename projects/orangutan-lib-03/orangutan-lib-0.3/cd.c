// The New LCD Code - Here's how it is:
// 
// The CrystalFontz LCDs used on the Orangutan and the Orangutan-X2
// don't use the Hitachi 47780 LCD controllers, they use an
// equivalent controller, the SunPlus SPLC780C.  After reading
// through Ben's X2 LCD code and re-re-(re-re-re) reading the
// datasheet on the SunPlus SPLC780C, I realize a number of mistakes
// I made in the original Orangutan LCD code.  Many of these were
// borne out of using other people's code rather than the datasheet,
// and several of them were of my own creation.  Here's my chance
// to set the record straight:
// 
// In operating an LCD there are two areas of concern:  The first is
// the timing for read and write operations, which is actually fairly
// critical.  The second is in the initialization process, which is
// equally critical.  Fortunately (or unfortunately) the SunPlus
// controllers are fairly forgiving, so there can be errors in both
// of these areas, but the LCD will still work after a fashion.
// Nonetheless, it's better to start with a good setup and know it is
// working correctly.
// 
// Read and write timing from the SPLC780C datasheet:
// 
// Write							Read
// 
// Set RS & R/W					Set RS & R/W
// Wait >= 40ns					Wait >= 40ns
// Set E High					Set E High
// Wait >= 150ns				Wait >= 120ns
// [Data must be valid by now]	[Data is now valid for read]
// Wait >= 80ns					Wait >= 210ns
// Set E Low					Set E Low
// Wait >= 10ns					RS & R/W can change
// [Data, RS, & R/W can change]
//
// In both cases, E cannot be brought high, then low, then high again
// in less than 500ns.
//
// Even though all that timing information is in nanoseconds, we need
// to be concerned about it.  At 20MHz, each AVR instruction executes
// in 50ns.  There are delays in there that must be longer than 50ns,
// so we must make sure we wait an appropriate amount of time.  On the
// Orangutan's 8MHz clock things are a little looser, but we're trying
// to write code that will execute on both.
//
// Initialization:
// 
// 4-Bit							8-Bit
// 
// Wait >= 15ms					Wait >= 15ms
// Send 0x3						Send 0x30
// Wait >= 4.1ms				Wait >= 4.1ms
// Send 0x3						Send 0x30
// Wait >= 100us				Wait >= 100us
// Send 0x3						Send 0x30
// Wait >= 100us				Wait >= 100us
// Send 0x2
// Wait >= 100us						
// Send 0x2 0x8					Send 0x38
// [Busy Flag is now valid]		[Busy Flag is now valid]
// Check BF						Check BF
// Send 0x0 0x8					Send 0x08
// Check BF						Check BF
// Send 0x0 0x1					Send 0x01
// Check BF						Check BF
// Send 0x0 0x6					Send 0x06
// Check BF						Check BF
// Send 0x0 0xC					Send 0x0C
//
// A point to consider:  Prior to a write, the busy flag (BF)
// must be clear.  During the time BF is not valid, apparently
// it stays clear.  So the BF check can apparently operate
// even when BF is not valid.  It simply will not cause any
// delays in execution.  I do not know if this is universally
// true, but it appears to be true for the two LCDs that ship
// with the Orangutan and the Orangutan-X2.
// 
// Another point:  On 4-bit interfaces, reading the busy flag
// is a little tricky.  The same command that returns the busy
// flag also returns the cursor's current memory address.  This 
// requires 8-bits of I/O, even if we're not interested in the 
// cursor's address.  So to check for the busy flag we have to
// do two 4-bit reads, complete with all the timing mentioned
// above, even though we're discarding the contents of the second
// 4-bit read.
//
// 
// The other big difference between the Orangutan's interface and
// the Orangutan-X2's interface is the choice in I/O pins on the
// AVR end.  On the Orangutan-X2, an entire 8-bit I/O port is used
// in a 1:1 configuration (e.g. PD0 -> D0, PD1 -> D1, ... PD7 -> D7)
// which simplifies the code a lot.  On the Orangutan, the 4-bit
// interface is split between two I/O ports, PORTB and PORTD.  This
// complicates the code a lot, and makes it far more difficult to
// hold all the I/O settings in neat #defines in some header file.
// Basically the 4-bit code is tailored to the Orangutan, whereas
// the 8-bit code is much more generic.  If you are looking for good
// easy to read 4-bit code for a non-Orangutan project, this is
// probably not the best example to use.
//
// As far as possible, I'll try to go over how each of the two
// interface to their LCDs so that differences in the code will be
// understandable.  We'll begin with the Orangutan's LCD:
//
// The Orangutan drives the LCD in 4-bit mode with E, R/W, and
// RS control lines.  Here are the pin assignments:
// 
// AVR		LCD		Direction		Function
// ------	------	--------------	----------------------------
// PD4		E		Out				Dedicated to LCD Enable
// PD2		RS		Out				Dedicated to LCD ???
// PD3		R/W		Out				Dedicated to LCD Read/Write
// PD7		DB7		In/Out			Dedicated to LCD I/O
// PB5		DB6		Out				LCD I/O and Orangutan Button 2
// PB4		DB5		Out				LCD I/O and Orangutan Button 1
// PB3		DB4		Out				LCD I/O and Orangutan Button 0
// N/C		DB3
// N/C		DB2
// N/C		DB1
// N/C		DB0
// 
// As you can see, three of the four data lines are also tasked to 
// the three push buttons on the Orangutan.  What this means is that 
// we need to keep track of this and that we can't use the buttons 
// when the LCD is being written to.  We have no guarantee what state
// those lines are in, so we need to return them to whatever state 
// they were in once we're done using them.
//
// The Orangutan-X2 drives the LCD in 8-bit mode with E, R/W, and
// RS control lines.  Here are the pin assignments:
//
// AVR		LCD		Direction		Function
// ------	------	--------------	----------------------------
// PB3		E		Out				Dedicated to LCD Enable
// PB0		RS		Out				Dedicated to LCD ???
// PB1		R/W		Out				Dedicated to LCD Read/Write
// PC7		DB7		In/Out			LCD I/O and Orangutan-X2 LED4
// PC6		DB6		Out				LCD I/O and Orangutan-X2 Button 2
// PC5		DB5		Out				LCD I/O and Orangutan-X2 LED3
// PC4		DB4		Out				LCD I/O and Orangutan-X2 Button 1
// PC3		DB3		Out				LCD I/O and Orangutan-X2 LED2
// PC2		DB2		Out				LCD I/O and Orangutan-X2 LED1
// PC1		DB1		Out				LCD I/O and Orangutan-X2 Button 0
// PC0		DB0		Out				LCD I/O and Orangutan-X2 LED0
//
// Even more so than the Orangutan, the LCD interface on the X2
// multi-tasks its I/O lines, so keeping track of their state is
// equally important.
//

// All device-specific dependencies are stored in this file.  You
// must edit it first before using any of the library routines.

#include "device.h"

// Only include this code if we have an Orangutan device that has an
// LCD attached (set in "device.h").

#ifdef _ENABLE_LCD_

#include <avr/io.h>
#include <util/delay.h>

// Include pgmspace.h so PROGMEM functions work (specifically for
// lcd_string_P() )

#include <avr/pgmspace.h>

// Include our own .h so we get out function prototypes

#include "lcd.h"



// This section of code is specific to the 4-bit Orangutan interface.
// Doing it this way is a little messy for reading, but it means a
// single lcd.c/h file can service both the Orangutan's and the
// Orangutan-X2's LCDs:



#ifdef ORANGUTAN

// Memory locations for the start of each line
// The actual memory locations are 0x00, and 0x40, but since
// D7 needs to be high in order to set a new memory location, we can go
// ahead and make the seventh bit of our memory location bytes to 1,
// which makes the numbers 0x80 and 0xC0:

const uint8_t line_mem[] = {0x80, 0xC0};

// Proto for lcd_nibble()
void lcd_nibble(uint8_t nibble);


// Initialize the LCD for a 4-bit interface

void lcd_init(void)
{
	// Set up the DDR for the LCD control lines
	LCD_CTRL_DDR |= (1 << LCD_RW) | (1 << LCD_RS) | (1 << LCD_E);

	// Wait >15ms
	_delay_ms(10);
	_delay_ms(10);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >4.1ms
	_delay_ms(5);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >120us
	_delay_ms(1);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >120us
	_delay_ms(1);
	// Send 0x2 (last four bits ignored)  Sets 4-bit mode
	lcd_cmd(0x20);
	// Wait >120us
	_delay_ms(1);
	// Send 0x28 = 4-bit, 2-line, 5x8 dots per char
	lcd_cmd(0x28);

	// Busy Flag is now valid, so hard-coded delays are no longer
	// required.

	// Send 0x08 = Display off, cursor off, blinking off
	lcd_cmd(0x08);

	// Send 0x01 = Clear display
	lcd_cmd(0x01);

	// Send 0x06 = Set entry mode: cursor shifts right, don't scroll
	lcd_cmd(0x06);

	// Send 0x0C = Display on, cursor off, blinking off
	lcd_cmd(0x0C);
}

// Wait for the busy flag to clear on a 4-bit interface
// This is necessarily more complicated than the 8-bit interface
// because E must be strobed twice to get the full eight bits
// back from the LCD, even though we're only interested in one
// of them.

void lcd_busy_wait(void)
{
	uint8_t temp_ddr, data;

	// Save our DDR information
	temp_ddr = DDRD;

	// Set up the data DDR for input
	DDRD &= ~(LCD_PORTD_MASK);

	// Set up RS and RW to read the state of the LCD's busy flag
	LCD_CTRL_PORT = (LCD_CTRL_PORT & ~(1 << LCD_RS)) | (1 << LCD_RW);

	do
	{
		// Bring E high
		LCD_CTRL_PORT |= (1 << LCD_E);

		// Wait at least 120ns (1us is excessive)
		_delay_us(1);

		// Get the data back from the LCD
		data = PIND & LCD_PORTD_MASK;

		// That excessive delay means our cycle time on E cannot be
		// shorter than 1000ns (500ns being the spec), so no further
		// delays are required

		// Bring E low
		LCD_CTRL_PORT &= ~(1 << LCD_E);

		// Wait a small bit
		_delay_us(1);

		// Strobe out the 4 bits we don't care about:

		// Bring E high
		LCD_CTRL_PORT |= (1 << LCD_E);

		// Wait at least 120ns (1us is excessive)
		_delay_us(1);

		// Bring E low
		LCD_CTRL_PORT &= ~(1 << LCD_E);
	}
	while (data & (1 << LCD_BF));

	// To reach here our busy flag must be zero, meaning the LCD is free

	// Restore our DDR information
	DDRD = temp_ddr;
}

// Send four bits out the 4-bit interface.  This assumes the busy flag
// is clear, that our DDRs are all set, etc.  Basically all it does is
// line up the bits and shove them out the appropriate I/O lines.

void lcd_nibble(uint8_t nibble)
{
	// Shift our nibble so bits 0, 1, and 2 line up with PB3, PB4,
	// and PB5:
	nibble <<= LCD_PORTB_SHIFT;

	// Clear those bits out of PORTB so we can write into them:
	PORTB &= ~LCD_PORTB_MASK;

	// And load PORTB with those three bits:
	PORTB |= (nibble & LCD_PORTB_MASK);

	// Now shift our nibble so bit 3 lines up with PD7:
	nibble <<= LCD_PORTD_SHIFT;

	// Clear that bit out of PORTD so we can write into it:
	PORTD &= ~LCD_PORTD_MASK;

	// And load PORTD with that last bit:
	PORTD |= (nibble & LCD_PORTD_MASK);

	// Big thanks to Jim Remington for posting his LCD code to
	// the Pololu Forums.  There was a 30us delay in here in
	// order to get the LCD to function.  Turns out the init
	// routine was setting something funky.  Changing the init
	// routine to something closer to what Jim had negated the
	// need for the long delay.

//	_delay_us(1);

	// At this point the four data lines are set, so the Enable pin 
	// is strobed to let the LCD latch them.

	// Bring E high
	LCD_CTRL_PORT |= (1 << LCD_E);
	
	// Wait => 450ns (1ms is excessive)
	_delay_us(1);

	// Bring E low
	LCD_CTRL_PORT &= ~(1 << LCD_E);

	_delay_us(1);

	// Dropping out of the routine will take at least 10ns, the time
	// given by the datasheet for the LCD controller to read the
	// nibble on the falling edge of E

	// Our nibble has now been sent to the LCD.
}

// Send either data or a command on a 4-bit interface

void lcd_send(uint8_t data, uint8_t rs)
{
	uint8_t temp_ddrb, temp_portb,
			temp_ddrd, temp_portd;

	// Wait until the busy flag clears
	lcd_busy_wait();

	// Save our DDR and port information
	temp_ddrb = DDRB;
	temp_portb = PORTB;
	temp_ddrd = DDRD;
	temp_portd = PORTD;

	// Clear RW and RS
	LCD_CTRL_PORT &= ~((1 << LCD_RW) | (1 << LCD_RS));

	// Set RS according to what this routine was told to do
	LCD_CTRL_PORT |= (rs << LCD_RS);

	// Set the data pins as outputs
	DDRB |= LCD_PORTB_MASK;
	DDRD |= LCD_PORTD_MASK;

	// Send the high 4 bits
	lcd_nibble(data >> 4);

	// Send the low 4 bits
	lcd_nibble(data & 0x0F);

	// Restore our DDR and port information
	PORTD = temp_portd;
	DDRD = temp_ddrd;
	PORTB = temp_portb;
	DDRB = temp_ddrb;
}

#endif // ORANGUTAN


// This section of code is specific to the 8-bit Orangutan-X2 interface.
// Again, doing it this way is a little messy for reading, but it means a
// single lcd.c/h file can service both the Orangutan's and the
// Orangutan-X2's LCDs:


#ifdef ORANGUTAN_X2

// Memory locations for the start of each line
// The actual memory locations are 0x00, 0x40, 0x14, and 0x54, but since
// D7 needs to be high in order to set a new memory location, we can go
// ahead and make the seventh bit of our memory location bytes to 1,
// which makes the numbers 0x80, 0xC0, 0x94, and 0xD4:

const uint8_t line_mem[] = {0x80, 0xC0, 0x94, 0xD4};

// Initialize the LCD for an 8-bit interface

void lcd_init(void)
{
	// Set up the DDR for the LCD control lines
	LCD_CTRL_DDR |= (1 << LCD_RW) | (1 << LCD_RS) | (1 << LCD_E);

	// Wait >15ms
	_delay_ms(10);
	_delay_ms(10);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >4.1ms
	_delay_ms(5);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >120us
	_delay_ms(1);
	// Send 0x3 (last four bits ignored)
	lcd_cmd(0x30);
	// Wait >120us
	_delay_ms(1);
	// Send 0x38 = 8-bit, 2-line, 5x8 dots per char
	lcd_cmd(0x38);

	// Busy Flag is now valid, so hard-coded delays are no longer
	// required.

	// Send 0x08 = Display off, cursor off, blinking off
	lcd_cmd(0x08);
	// Send 0x01 = Clear display
	lcd_cmd(0x01);
	// Send 0x06 = Set entry mode: cursor shifts right, don't scroll
	lcd_cmd(0x06);
	// Send 0x0C = Display on, cursor off, blinking off
	lcd_cmd(0x0C);
}

// Wait for the busy flag to clear on an 8-bit interface

void lcd_busy_wait(void)
{
	uint8_t temp_ddr, data;

	// Save our DDR information
	temp_ddr = LCD_DATA_DDR;

	// Set up the data DDR for input
	LCD_DATA_DDR = 0x00;

	// Set up RS and RW to read the state of the LCD's busy flag
	LCD_CTRL_PORT = (LCD_CTRL_PORT & !(1 << LCD_RS)) | (1 << LCD_RW);

	do
	{
		// Bring E high
		LCD_CTRL_PORT |= (1 << LCD_E);

		// Wait at least 120ns (1us is excessive)
		_delay_us(1);

		// Get the data back from the LCD
		data = LCD_DATA_PIN;

		// That excessive delay means our cycle time on E cannot be
		// shorter than 1000ns (500ns being the spec), so no further
		// delays are required

		// Bring E low
		LCD_CTRL_PORT &= ~(1 << LCD_E);
	}
	while (data & (1 << LCD_BF));

	// To reach here our busy flag must be zero, meaning the LCD is free

	// Restore our DDR information
	LCD_DATA_DDR = temp_ddr;
}

// Send either data or a command on an 8-bit interface

void lcd_send(uint8_t data, uint8_t rs)
{
	uint8_t temp_ddr, temp_port;

	// Wait until the busy flag clears
	lcd_busy_wait();

	// Save our DDR and port information
	temp_ddr = LCD_DATA_DDR;
	temp_port = LCD_DATA_PORT;

	// Clear RW and RS
	LCD_CTRL_PORT &= ~((1 << LCD_RW) | (1 << LCD_RS));

	// Set RS according to what this routine was told to do
	LCD_CTRL_PORT |= (rs << LCD_RS);

	// Set data pins as outputs
	LCD_DATA_DDR = 0xFF;

	// Load our data
	LCD_DATA_PORT = data;

	// Bring E high
	LCD_CTRL_PORT |= (1 << LCD_E);

	// Enable line must remain high for a total of 230ns
	// Delaying for 1000ns (1ms) is overkill.
	_delay_us(1);

	// Bring E low
	LCD_CTRL_PORT &= ~(1 << LCD_E);
	
	// Restore our DDR and port information
	LCD_DATA_PORT = temp_port;
	LCD_DATA_DDR = temp_ddr;
}

#endif // ORANGUTAN_X2



// Past here the LCD routines are identical for both the Orangutan
// and the Orangutan-X2, so there's no need to split them up in
// terms of 4-bit or 8-bit versions.  (This also makes the higher-
// level LCD routines a lot easier to maintain on both platforms.)



// lcd_string sends a string to the LCD.  The string is printed from 
// wherever the cursor is, and will not span lines.  (This lets you 
// concatenate print statements.)

void lcd_string(const char *str)
{
	while (*str != 0)
		lcd_data(*str++);
}

// lcd_string_P copies a string from FLASH to RAM and calls
// lcd_string().  In thery this routine SHOULD be able to use
// the "pgm_read_byte" macro to fetch each byte from FLASH and
// pass it to lcd_data(), but for some reason that does not work.

void lcd_string_P(const char *str)
{
	char ucStr[16];
	strncpy_P(ucStr,str,15);
	lcd_string(ucStr);
}

// lcd_int prints an integer.  Again, this prints from wherever
// the cursor is, and will not span lines.  (This lets you 
// concatenate print statements.)

void lcd_int(uint16_t n)
{
	char st[4] = {0,0,0,0};

	// The 0x30 addition shifts the decimal number up
	// to the ASCII location of "0".

	// Hundreds place
	st[0] = (n / 100) + 0x30;
	n = n % 100;

	// Tens place
	st[1] = (n / 10) + 0x30;
	n = n % 10;

	// Ones place
	st[2] = n + 0x30;

	// Print it as a string
	lcd_string(st);
}

// lcd_gotoxy() is the replacement for lcd_moveto() (the latter of
// which will is deprecated, and will disappear after the 0.3
// release of Orangutan-lib.)

void lcd_gotoxy(uint8_t x, uint8_t y)
{
	// Make sure our X and Y are within bounds
	x = (x < LCD_MAX_X ? x : LCD_MAX_X);
	y = (y < LCD_MAX_Y ? y : LCD_MAX_Y);

	// Grab the location in the LCD's memory of the start of line y,
	// and add X to it to get the right character location.
	lcd_cmd(line_mem[y] + x);
}

// lcd_moverel shifts the cursor left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_moverel(uint8_t dir, uint8_t num)
{
	uint8_t cmd;

	cmd = dir ? LCD_CURSOR_R : LCD_CURSOR_L;
	while(num-- > 0)
		lcd_cmd(cmd);
}

// lcd_shift shifts the display left or right the given number of
// positions.  This is what you'd use for a scrolling display.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_shift(uint8_t dir, uint8_t num)
{
	uint8_t cmd;

	cmd = dir ? LCD_SHIFT_R : LCD_SHIFT_L;
	while(num-- > 0)
		lcd_cmd(cmd);
}

// The rest of the LCD commands are one-liners.  Send the appropriate 
// command to the LCD, and it takes care of the rest.  Since these 
// are all inline #define statements, they're handled in the lcd.h file.

#endif //_ENABLE_LCD_

