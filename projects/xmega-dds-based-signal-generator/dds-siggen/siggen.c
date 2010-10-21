////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Signal Generator
//
// This is the AVR code for the DDS Signal Generator. This board contains an Atmal XMega CPU,
// an Analog Devices AD9851 DDS (Direct Digital Synthesis) chip, which is the part that actually
// generates the signals, a 4x4 (maximum size) matrix keypad, and a HD44780-compatible LCD display.
//
// The DDS chip is fed by a 24 MHz oscillator (the AVR is driven by its own internal oscillator).
// Inside the DDS chip is a x6 PLL which we enable. There is also a "phase" register, which we 
// program to set the final output frequency. The output frequency of the DDS chip is:
//
// freq = (system_clock * phase) / 2^32
//
// where system_clock is, in our case, 24 MHz x 6 = 144 MHz
//
// Given that the user specifies the desired frequency, we need to calculate the value to write into
// the phase register. This is:
//
// phase = (freq * 2^32) / 144 MHz
//
//       = freq * 29.826
//
//
//
// Note the AVR is running at 2 MHz (its default internal oscillator). Yes it could easily run faster, 
// but there's no need, and running slow means there's no problem when talking to the DDS and LCD.
// In this application the AVR has almost nothing to do - it mostly just sits around waiting for
// keypresses to happen.
//
// We scan the keypad and convert keypresses to their corresponding ASCII character. We collect these
// keypresses to build up the ASCII frequency that the user wants. Eg: "125000" Hz. We then convert
// that ASCII string frequency into a binary 32-bit number frequency and give it to the DDS.
// That's it, in a nutshell anyway.
//
// LCD routines are handled by the accompanying lcd.c file, and the associated header lcd.h.
// Those routines are tuned to this hardware, so if you want to support a different LCD, or
// use those LCD routines on different hardware, make sure to edit that file.
//
// The LCD routines use simple delays, quite long at times (eg 10 mS) when writing to the display.
// It's crude, but very simple. It doesn't matter in this application because the AVR has nothing
// else to do anyway, so CPU utilisation is not a factor. In other applications it might not be
// acceptable for the CPU to stop and wait a while, in which case a much more complex system
// of handling delays would be required. Thankfully we don't need to worry about that.
//
// The keypad scanning routines take advantage of the fact that the Xmega has pull-downs on its
// I/O pins. So we pull-down our input pins, then set high our output pins to look for a keypress.
// This is backwards to many routines you might find, where the inputs may be pulled high
// and the outputs driven low.
// It has the big advantage than when no button is pressed, our code returns zero. If you port
// this key scanning code to other hardware, make sure the input pins are pulled down. Then
// change the keycode2char() function to adapt your keymapping to the appropriate ASCII characters
// (or whatever you want). Some code has been left commented out at the bottom of the main loop
// which can be used to display the scanned keycode on the LCD display, which makes it easy to
// determine what keycode each key produces when its pressed and scanned. 
//
// User error checking is limited because there's not a lot the user can do wrong. We check that
// the frequency entered is valid, ie, doesn't exceed the maximum allowed frequency. We also check
// while the user is entering digits that the number of digits isn't too long - if the user tries
// to enter too many digits we cut him off and start again. That's about it for user input
// error checking.
//
// The code assumes a 2-line LCD display, 2 x 24 characters in this case. The top line always
// displays an invite to enter a frequency, and shows the numbers as the user presses keys on
// the keypad. Eg: Enter Freq:
// The second line starts off showing the currently output frequency, but switches to displaying
// the "* to cancel # to enter" message if the user starts entering a new frequency.
// A smaller LCD can be used (eg 2 x 16) - you just need to shorten the LCD messages to be displayed
// in the code below.
//
// 
// (c) Frank Van Hooft 2010
//
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Side Notes:
//
// In AVRGCC, an INT is 16 bits. So be careful of this when you're trying to do a printf()
// of a 32-bit value. Using %d will only print a 16-bit number. To print a 32-bit number
// use %1.ld  instead (that's number-one then period then lower-case-letter-L).
//
// The _P versions of printf and sscanf are heavily used here. These are the same as the regular
// version, but they accept their string from flash memory instead of from RAM. Hence the PSTR("xx")
// in the middle of them, which places that string in flash.
//
////////////////////////////////////////////////////////////////////////////////////////////////////




#include <stdio.h>
#include <avr/io.h> 
#include "avr_compiler.h"
#include "lcd.h"



// Maximum frequency we allow the DDS to generate. If the requested frequency is greater
// than this, we limit to this frequency or simply consider it an invalid entry.
#define MAX_DDS_FREQ  40000000

// Used for the UART transmitter.
// Set these to USARTE0 & PORTE for our board.
// Bit3 is the transmit pin.
// This code doesn't receive anything over the serial port - it only transmits.
// The UART is really only used for debug - it has no real purpose here.
#define UARTPORT  USARTE0
#define UARTPPORT PORTE

// This defines which port the matrix keypad is on.
#define KEYPORT PORTD

// These are the Xmega port bit definitions for the DDS chip, ie, which port
// and which port pins the DDS chip is connected to. There are 4 DDS chip signals,
// and it is assumed they are all on the same port.
#define DDSPORT  PORTC
#define DDSRESET PIN6_bm			/* reset pin, output high to reset the DDS */
#define DDSWCLK  PIN2_bm			/* W_CLK pin, "word load clock" output */
#define DDSFQ	 PIN3_bm			/* FQ_UD pin, "frequency update" output, rising edge active */
#define DDSD7	 PIN7_bm			/* D7 pin, data bit 7 output, serial data pin */

// Maximum number of characters allowed in the frequency string
#define MAXFREQLEN	8



// Function Prototypes
static int uart_putchar(char c, FILE *stream);
static void uart_init (void);
static void dds_init (void);
static void keypad_init (void);
static void write_DDS_freq (uint32_t freq);
static uint8_t scan_keypad (void);
static uint8_t keycode2char (uint8_t keycode);
static void display_error (void);




// Global Variables
static FILE mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);	// used by printf()
static char MsgTemp[25];						// temporary string array, long enough for an LCD line plus null
static char FreqStr[MAXFREQLEN+1];				// string array to hold entered frequency, +1 to allow null on end



int main (void)
{
	// uint8_t  keycode;							// raw binary code from keyscan routine
	uint8_t  keychar;								// ASCII value for keypress (0-9, *, #)
	uint8_t  freqstrcnt;							// count number of received characters in the frequency string
	uint8_t  ok_flag;
	uint32_t current_freq;							// currently generating frequency
	uint32_t new_freq;


	// initialise the DDS chip & associated Xmega port bits
	dds_init();

	// Initialise the UART, used for printf()
	uart_init();
    stdout = &mystdout;

	// Print a message out the serial port
    printf_P(PSTR("\n\nDDS SIGNAL GENERATOR BOARD\n\n\n"));

	// Initialise the LCD Display
	lcd_init();

	// Initialise the keypad
	keypad_init();

	// Initial Current Frequency = 0
	current_freq = 0;
	write_DDS_freq (current_freq);


	// Sit in an endless loop. This is our "main loop" and we never exit it.
	while (1) {

		// number of received characters in the frequency string is currently zero
		freqstrcnt = 0;

		// clear display
		lcd_clear_and_home();
		
		// display "Enter Freq:" on first line
		lcd_line_one();
		lcd_write_string_p(PSTR("Enter Freq:\0"));		// message from flash space

		// display "Currently: xxx Hz" on second line
		lcd_line_two();
		sprintf_P(MsgTemp, PSTR("Currently: %1.ld Hz"), current_freq);
		lcd_write_string_0(MsgTemp);

		// sit here waiting for a number key (0-9) to be pressed - other keys ignored
		keychar = 0;
		while ( (keychar>'9') || (keychar<'0') )
			keychar = keycode2char(scan_keypad());			// get keypress, convert to ASCII character

		// we got a keypress!! the number 0 - 9. Now we start collecting them
		// display "* to cancel  # to enter" on second line
		lcd_line_two();
		lcd_write_string_p(PSTR("* to cancel   # to enter\0"));

		// goto position 13 on the first line (start of where we display the entered frequency)
		lcd_goto (0, 12);

		// set OK flag
		ok_flag = 1;

		// while ok_flag says all is OK, we loop collecting keypresses, building the frequency string
		while (ok_flag) {

			// display key (the user entered number digit 0 - 9) on display
			// the LCD automatically increments the cursor so we don't have to do anymore LCD goto's
			lcd_write_data (keychar);

			// place key in FreqStr frequency string and increment freqstrcnt frequency string counter
			FreqStr[freqstrcnt++] = keychar;

			// simple debounce - wait for keypress release, then wait 50ms, then again
			// wait for keypress release
			while (scan_keypad() != 0);				// wait until keypad returns zero, ie, no keypress
			_delay_ms(50);
			while (scan_keypad() != 0);

			// wait for a keypress
			keychar = 0;
			while (!keychar)
				keychar = keycode2char(scan_keypad());			// get keypress, convert to ASCII character
			
			// if key == "#" then user means "I'm done" so we must interpret the frequency string and
			// respond to it (check it for validity then use it if it's OK) We will clear the OK flag to 
			// signal we want to break out of the while loop; we'll need to go back to waiting
			// for that very first keypress again (ie back to the beginning of the while(1) loop).
			if (keychar == '#') {
				
				// add null to end of FreqStr frequency string
				FreqStr[freqstrcnt] = 0;
				
				// convert frequency string to integer
				new_freq = atol (FreqStr);

				// if requested frequency less than maximum allowed frequency, then it's good,
				// write it into current_frequency, give it to the DDS
				if (new_freq <= MAX_DDS_FREQ) {
					current_freq = new_freq;
					write_DDS_freq (current_freq);
				}
				
				// otherwise display error message because entered frequency is too high
				else
					display_error();

				// regardless, we clear OK flag - we're finished processing this frequency string
				ok_flag = 0;
			}

			// otherwise if key == "*" then clear OK flag  (as user wants to cancel)
			else if (keychar == '*')
				ok_flag = 0;
	
			// else if freqstrcnt = MAXFRREQLEN  then display error message and clear OK flag
			// (as our frequency string is now too long) regardless of what digit was pressed
			else if (freqstrcnt >= MAXFREQLEN) {
				display_error();
				ok_flag = 0;
			}

			// otherwise the keypress was just another number, so we leave ok_flag set, so the character
			// will be written on the screen and put into the frequency string once we get around to the
			// top of this while() loop again

		}	// bottom of while (ok_flag) 



		// These few lines scan the keypad & display the resulting codes on the LCD display.
		// Put this in an endless loop to show what code each keypress produces, for development purposes
		//  keycode = scan_keypad();						// get current keypress code
		//  keychar = keycode2char(keycode);				// convert keypress code into ASCII character
		//  lcd_line_one();
		//  sprintf(MsgTemp, "Keypad code: 0x%02x %1.1s", keycode, (char*)&keychar);	// could use sprintf_P instead
		//  lcd_write_string_0(MsgTemp);


	}		// bottom of the main while(1) loop
}			// bottom of main





// Display Error Message
// This displays an error message and then waits 4 seconds to allow the user to read it.
// The only error message is to state the maximum allowed frequency.
static void display_error (void)
{
	lcd_clear_and_home();
	lcd_line_one();
	lcd_write_string_p(PSTR("ERROR: Max Freq is\0"));
	lcd_line_two();
	sprintf_P(MsgTemp, PSTR("%1.ld Hz"), MAX_DDS_FREQ);
	lcd_write_string_0(MsgTemp);
	_delay_ms(4000);			// wait 4 sec to allow user time to read it
}





// Write_DDS_freq
// This routine writes a 40-bit serial word to the DDS. The calling function passes
// in the 32-bit frequency value, in Hz, that it desires. This routine converts
// that frequency in Hz into a corresponding value for the DDS chip register, then
// writes the full 40-bit register serially into the DDS.
static void write_DDS_freq (uint32_t freq)
{
	uint64_t temp64;
	uint32_t temp32;
	uint8_t  temp8, i;

	// Check to see if the frequency requested is beyond our capability, and
	// set it to our maximum frequency if it's too high.
	if (freq > MAX_DDS_FREQ)
		freq = MAX_DDS_FREQ;

	// Convert the requested frequency to the number required by the DDS chip
	// using the math: freq * 29.826, which is the same as
	// (freq * 30542) >> 10
	// The (freq * 30542) portion can exceed a 32-bit number in the case of larger
	// frequency values, so we need to do this little bit of math as 64-bits,
	// then bring the final result back to a 32-bit number. The final copying to a 
	// 32-bit number probably isn't necessary, but it makes me feel better.
	temp64 = (uint64_t)freq * 30542;
	temp64 = temp64 >> 10;
	temp32 = (uint32_t)temp64;

	// Now in temp32 we have the 32-bit frequency value that needs to be written to
	// the DDS. In temp8 we'll put the remaining 8-bit value that needs to be given to
	// the DDS. See figure 20 and table III in the AD9851 datasheet. Between temp32
	// and temp8 we'll have the 40 bits that must be written to the DDS.
	temp8 = 0x01;							// enable x6 PLL

	// Now we serially write these 40 bits to the DDS. The sequence of events is:
	// - start with FQ and WCLK both low
	// - place a data bit on D7
	// - set WCLK high, then low
	// - place the next data bit on D7
	// etc
	// - finish with setting FQ high, then low
	// We shift out temp32 LSB first, followed by temp8 also LSB first

	DDSPORT.OUTCLR = DDSWCLK | DDSFQ;						// ensure W_CLK & FQ_UD both low

	for (i=32; i; i--) {
		if (temp32 & 0x1)
			DDSPORT.OUTSET = DDSD7;							// D7 high if temp32 LSB high
		else				
			DDSPORT.OUTCLR = DDSD7;							// else D7 low because temp32 LSB is low
		
		DDSPORT.OUTSET = DDSWCLK;							// set W_CLK high
		DDSPORT.OUTCLR = DDSWCLK;							// set W_CLK low

		temp32 = temp32 >> 1;								// move next bit into temp32 LSB
	}

	for (i=8; i; i--) {
		if (temp8 & 0x1)
			DDSPORT.OUTSET = DDSD7;							// D7 high if temp8 LSB high
		else				
			DDSPORT.OUTCLR = DDSD7;							// else D7 low because temp8 LSB is low
		
		DDSPORT.OUTSET = DDSWCLK;							// set W_CLK high
		DDSPORT.OUTCLR = DDSWCLK;							// set W_CLK low

		temp8 = temp8 >> 1;									// move next bit into temp8 LSB
	}

	DDSPORT.OUTSET = DDSFQ;									// set FQ_UD high
	DDSPORT.OUTCLR = DDSFQ;									// set FQ_UD low, to latch our 40 bits into the DDS

	DDSPORT.OUTCLR = DDSD7;									// just to be neat, leave D7 low when we're finished
}




// Scan Keypad
// Pretty simple. This function scans the matrix keypad, and returns an 8-bit code corresponding 
// to the key that's been pressed. You'll want to call this function pretty regularly to make
// sure you don't miss a keypress. This function performs no debouncing - it simply tells you if
// a key is being pressed at this moment in time. If no key is pressed, this function returns zero.
// The keypad is assumed to be a 4x4 matrix. The keypad_init function already set the upper 4
// port bits to be outputs, and the lower 4 bits to be inputs, with their pull-downs enabled.
// So normally the 4 input bits would read low. Unless a key is pressed, shorting an input pin
// to one of the output pins, then if that output pin was high, that input pin would read high.
// To scan, this function simply sets one of the output lines high, then checks to see if any
// of the 4 input lines are high. Then sets the next output line high & checks the inputs, etc,
// until all 4 of the outputs have had their turn being high. (Sounds kinda funny hey?) If no
// inputs are found high it returns a zero. If an input is found high it returns the port value
// immediately (so the calling function gets a unique code of output & input bit combination)
static uint8_t scan_keypad (void)
{
	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;		// all output pins start off low
	KEYPORT.OUTSET = PIN4_bm;									// only output pin 4 is high
	_delay_us(5);												// allow time for this to propagate
	if (KEYPORT.IN & 0x0f)										// if any of the input pins are high
		return KEYPORT.IN;										// then immediately return this value
		
	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;
	KEYPORT.OUTSET = PIN5_bm;									// pin 5
	_delay_us(5);												
	if (KEYPORT.IN & 0x0f)								
		return KEYPORT.IN;								

	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;
	KEYPORT.OUTSET = PIN6_bm;									// pin 6
	_delay_us(5);										
	if (KEYPORT.IN & 0x0f)								
		return KEYPORT.IN;								

	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;
	KEYPORT.OUTSET = PIN7_bm;									// and pin 7					
	_delay_us(5);					
	if (KEYPORT.IN & 0x0f)								
		return KEYPORT.IN;								

	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;
	return 0;
}




// Keycode to Character
// This routine converts a keycode to a character. It returns the ASCII numbers 0 - 9,
// and the ASCII characters for * and #.  For any unknown character it returns binary zero.
// Pretty simple function - just a long switch statement, matching each keycode to an return value.
static uint8_t keycode2char (uint8_t keycode)
{
	switch (keycode) {
		case 0x28: return '0';
		case 0x11: return '1';
		case 0x21: return '2';
		case 0x41: return '3';
		case 0x12: return '4';
		case 0x22: return '5';
		case 0x42: return '6';
		case 0x14: return '7';
		case 0x24: return '8';
		case 0x44: return '9';
		case 0x18: return '*';
		case 0x48: return '#';
		default:   return 0;
	}
}



// Prints a single character out the UART, implicitly used by printf()
static int uart_putchar (char c, FILE *stream)
{
	if (c == '\n')
        uart_putchar('\r', stream);

  	// Wait for the transmit buffer to be empty
  	while ( !( UARTPORT.STATUS & USART_DREIF_bm) );
    
  	// Put our character into the transmit buffer
  	UARTPORT.DATA = c; 

	return 0;
}




// Init DDS. 
// This routine initialises the AD9851 DDS, including initialising
// the XMega ports which are used to communicate with the DDS chip.
// This routine:
// - initialises the Xmega DDS port bits
// - resets the DDS
// - puts the DDS into serial mode
// - loads a value of 0 into its frequency control register & enable the x6 PLL
static void dds_init (void)
{
	volatile int8_t i;

	// Set the desired output values for the DDS pins
	DDSPORT.OUTSET = DDSRESET;								// want reset pin high to reset the DDS
	DDSPORT.OUTCLR = DDSWCLK | DDSFQ | DDSD7;				// want other DDS pins to be low
	DDSPORT.DIRSET = DDSRESET | DDSWCLK | DDSFQ | DDSD7;	// set all DDS Xmega pins to be outputs

	// OK, DDS is currently in reset & the Xmega DDS ports are configured as outputs.
	// Wait a moment to give the DDS a nice long reset pulse
	for (i=5; i; i--);

	// Take DDS out of reset, put DDS into serial data communication mode
	DDSPORT.OUTCLR = DDSRESET;								// DDS now out of reset

	// Enable serial mode. See Fig 17 in the AD9851 datasheet. We have pins D2-0 
	// wired appropriately on the PCB, so we just need to toggle WCLK & FQ.
	DDSPORT.OUTSET = DDSWCLK;								// set W_CLK high
	DDSPORT.OUTCLR = DDSWCLK;								// set W_CLK low
	DDSPORT.OUTSET = DDSFQ;									// set FQ_UD high
	DDSPORT.OUTCLR = DDSFQ;									// set FQ_UD low

	// At this point the DDS should be in serial mode. Write a 40 bit word, where
	// the frequency control bits are all zero, but the x6 PLL is enabled.
	write_DDS_freq (0);

	// we're finished initialising the DDS
}





// Init USART.  Transmit only (we're not receiving anything)
// We use USARTE0, transmit pin on PE3.
// Want 9600 baud. Have a 2 MHz clock. BSCALE = 0
// BSEL = ( 2000000 / (2^0 * 16*9600)) -1 = 12
// Fbaud = 2000000 / (2^0 * 16 * (12+1))  = 9615 bits/sec
static void uart_init (void)
{
	// Set the TxD pin high - set PORTE DIR register bit 3 to 1
	UARTPPORT.OUTSET = PIN3_bm;
	
	// Set the TxD pin as an output - set PORTE OUT register bit 3 to 1
	UARTPPORT.DIRSET = PIN3_bm;

	// Set baud rate & frame format
	UARTPORT.BAUDCTRLB = 0;			// BSCALE = 0 as well
	UARTPORT.BAUDCTRLA = 12;

	// Set mode of operation
	UARTPORT.CTRLA = 0;				// no interrupts please
	UARTPORT.CTRLC = 0x03;			// async, no parity, 8 bit data, 1 stop bit

	// Enable transmitter only
	UARTPORT.CTRLB = USART_TXEN_bm;
}




// Initialise keypad
// This function initialises the keypad. The keypad is assumed to be a 4x4 matrix.
// (Which means it'll also handle 4x3 keypads.)
// So this function sets the low 4 bits (bits 0-3) as inputs with their internal
// pull-down resistors enabled, and sets the upper 4 bits (bits 4-7) as outputs,
// and sets their output values low.
static void keypad_init (void)
{

	// make the low 4 bits be inputs
	KEYPORT.DIRCLR = PIN0_bm | PIN1_bm | PIN2_bm| PIN3_bm;		// direction set to input

	// For each of the 4 input pins, enable its internal pulldown
	KEYPORT.PIN0CTRL = 0x10;
	KEYPORT.PIN1CTRL = 0x10;
	KEYPORT.PIN2CTRL = 0x10;
	KEYPORT.PIN3CTRL = 0x10;

	// set the upper 4 bits as outputs & make their value "0"
	KEYPORT.DIRSET = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;		// direction set to output
	KEYPORT.OUTCLR = PIN4_bm | PIN5_bm | PIN6_bm| PIN7_bm;		// pin levels now low
}
