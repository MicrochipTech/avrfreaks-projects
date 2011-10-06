/*
Test program for FAT32 module.
Built for Atmega1281 @ 8MHz 3V

Connections are:

SD card:
	CLK_BIT		PB1		(hardware spi port)
	MISO_BIT	PB3		(hardware spi port)
	MOSI_BIT	PB2		(hardware spi port)
	SS_BIT		PD7

Serial port:
	TX output	PD3		(hardware uart1)
	TX output	PD2		(hardware uart1)
	
Audio output:
	Audio PWM	PB7		(hardware timer0 PWM)
	
The audio output will need a low pass filter to attenuate the 15kHz, and an amplifier to drive a speaker.


*/

#include <includes.h>

// Buffer sizes for writing to uarts, must be at least 1
#define UART0_FIFO_TX_SIZE	1
#define UART1_FIFO_TX_SIZE	512

#define DEBUG_P(arg1)		fifo_write_string_P(debug_fifo_ptr, arg1)
#define DEBUG(arg1)			fifo_write_string(debug_fifo_ptr, arg1)
#define DEBUG_CHAR(arg1)	fifo_write_char(debug_fifo_ptr, arg1)

// **** Local prototypes ****

void main_fly(void);

// **** Global variables ****

	struct fifo_control *debug_fifo_ptr=NULL;	//debug output	
	char debug_text[30];						//scratch ram used for building debug strings


// Watchdog fix for 1281
void get_mcusr(void) \
      __attribute__((naked)) \
      __attribute__((section(".init3")));
void get_mcusr(void)
{
	MCUSR = 0;
    wdt_disable();
}

/*
Any code which can be executed on-the-fly (without pauses or delays), should be inserted into main_fly().
main_fly() is then called any time the main application is waiting for something,
this may include waiting for space in the uart transmit buffer, waiting for user input, or any kind of delay.
If debug output is required from main_fly(), then the uart must be configured NOT to call main_fly when waiting.
*/
void main_fly(void)
{
}

	
// Main application routine.
int main(void)
{
	//initialise IO
	sd_init();
	audio_init();

	DDRC |=_BV(PC0)+_BV(PC1)+_BV(PC2);
	PORTC &= ~(_BV(PC0)+_BV(PC1)+_BV(PC2));

	#define BAUD_TOL	5
	#define	BAUD		19200
	#include <util/setbaud.h>
	uart1_init(UBRR_VALUE, USE_2X, UART1_FIFO_TX_SIZE, NULL);
	
	//execute on-the-fly processes while waiting for uart to transmit, DO NOT DO THIS IF main_fly() ALSO OUTPUTS TO UART!!
	uart1_fifo_tx.wait_fptr = main_fly;

	//connect debug info to UART 1
	debug_fifo_ptr=&uart1_fifo_tx;

	//enable global interrupts	
	sei();

	//setup console
	console_init();
	console_vars.echo=TRUE;									//echo on
	console_vars.input_device = CONSOLE_DEVICE_UART1;		//tell console it's i/o device type
	console_vars.output_device = CONSOLE_DEVICE_UART1;

	//connect uart to console
	console_vars.output_ptr=&uart1_fifo_tx;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uart1_fifo_rx_ptr = &console_vars.input;
	};

	//power on message
	DEBUG_P(PSTR("\r\nDevice name "));
	DEBUG_P(PSTR(VERSION_STRING));
	DEBUG_P(PSTR("\r\nHardware revision: "));
	DEBUG(hardware_name(debug_text));
	DEBUG_P(PSTR("\r\n(hit space for main menu)\r\n"));

	while(1)
	{
		main_fly();
		
		//execute console if space hit.
		if(console_vars.input.empty==FALSE)
		{
			if(fifo_read_char(&console_vars.input)==' ')
				console_main();
		};
	};

	return 0;
}


//**************************************************************************************
// Subroutines
//**************************************************************************************

//return numeric representation of hardware revision
int hardware_id(void)
{
	// Insert code here to determine the hardware revision by reading port lines.
	// This is important to allow your firmware to be compatible with different revisions or models of hardware.
	return 0;
}

//write name of hardware revision to text buffer
char* hardware_name(char* textbuf)
{
	switch(hardware_id())
	{
		case 0:
			strcpy_P(textbuf, PSTR("V1.0"));
			break;
		default:		
			strcpy_P(textbuf, PSTR("UNKNOWN!"));
			break;
	};
	return textbuf;
}
