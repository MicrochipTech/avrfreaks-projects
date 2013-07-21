/*
	Cuboid 8x8x8 LED display controller.
	
	Modules are:
	
	a2d.c		Measure voltages, LED current, and ambient light level.
	
	button.c	Read buttons.
	
	console.c	Provide menu via serial port, used for configuration and/or uploading a new .hex file.
	
	led.c		Drives the LEDs. Controls shift registers, plane FETs, frame rate, etc..
				Also has functions for scrolling and drawing lines.
	
	main.c		Main application, handles timer ticks, fly processing, initialisation etc...

	pattern.c	Uses functions in led.c to create patterns.

	power.c		Controls LED supply voltage.

	sequence.c	Calls functions in pattern.c in a particular sequence, also has menu for sequence selection.

	settings.c	Handles non-volatile settings, loads defaults from progmem, saves ram shadow to eeprom.

	text.c		Functions for drawing text in the 8x8x8 bitmap.

	uart.c		Basic uart functions.


	Operation:
	
	Sequence selection:
	
		There are 4 sequences to choose from numbered 0-3. Press SELECT to choose a sequence.
		The sequence number 0-3 or A (for Auto) will be displayed on the front.
		The left side will display N or D showing if a sequence is intended for Day or Night.
		In Auto mode, the light will be measured between patterns,
		If the light measurement is above the day threshold, the last selected day sequence will play.
		If the light measurement is below the night threshold, the last selected night sequence will play.
		The selected sequence, and preferences for day/night selection are saved in eeprom.
		
		Example, lets say you have selected AUTO, and sequence 1 plays at night, and sequence 2 plays during the day.
		If you wish to change this so that sequence 0 plays at night;
			Press SELECT to choose sequence 0, wait for it to start playing.
			Press SELECT to choose AUTO (A). Auto mode will now play sequence 0 at night.
	
	Pattern selection/rotation:
	
		To stop a sequence from rotating, press SKIP. Rotation is disabled, the current pattern will continue indefinatly.
		When rotation is disabled, pressing SKIP will manually rotate through patterns.
		Pressing and holding SKIP for 1sec will re-enable rotation.
		Rotation being enabled or disabled, is saved seperatly for each sequence.
	
	To modify brightness:
	
		Use the brightness +/- buttons to change brightness.
		The brightness setting is saved seperatly for each sequence.
		
		

 *	todo: check hardware POR
	
	Cuboid firmware
*/
 
	#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define TEXTOUT_P(arg1)		uart1_tx_string_P(arg1)
	#define TEXTOUT(arg1)		uart1_tx_string(arg1)
	

//********************************************************************************************************
// Local defines
//********************************************************************************************************
	
//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static unsigned int	time;
	static char 		console_request = FALSE;		//Flag console request

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	// Attached to uart1_rx_ptr to detect a space character
	static void main_rx(char x);

//********************************************************************************************************
// Public functions
//********************************************************************************************************


// Call this while waiting, checks for a timer tick and calls module timer functions.
// Be very careful not to call anything within main_fly that also calls main_fly
void main_fly(void)
{
	static char recursive=FALSE;
	if(recursive)
	{
		TEXTOUT_P(PSTR("\r\nERROR! main_fly() calls itself!!\r\n"));
		while(1);
	};
	recursive=TRUE;
	
	a2d_fly();
	if(TIFR & _BV(TOV2))
	{
		TIFR=_BV(TOV2);
		if(time)
			time--;
		
		console_tick();
		pattern_tick();
		button_tick();
		sequence_tick();
		a2d_tick();
	};
	
	recursive=FALSE;
}

// Main application routine.
int main(void)
{
	//Timer2 Fosc/1024
	TCCR2 = 5;	//FOSC/1024
	
	//initialise modules
	settings_init();
	uart1_init();
	led_init();
	power_init();
	button_init();
	a2d_init();
	sequence_init();

	//enable global interrupts	
	sei();

	//execute test sequence
	if(settings_ram.test)	
	{
		//attach uart RX to console
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			uart1_rx_ptr = &console_rx;
		};
		console_test();
	};

	//power on message
	TEXTOUT_P(PSTR("\r\nCuboid "));
	TEXTOUT_P(PSTR(VERSION_STRING));
	TEXTOUT_P(PSTR("\r\n"));

	//Power on test
	power_test();

	while(1)
	{
		//attach uart RX to main_rx
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			uart1_rx_ptr = &main_rx;
		};
		
		//run display sequence
		sequence_run();
		
		//select new sequence if sequence button was pressed
		sequence_menu();
		
		//enter console if space was received on UART
		if(console_request)
		{			
			console_request=FALSE;
			
			//attach uart RX to console
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				uart1_rx_ptr = &console_rx;
			};
			
			//execute console
			console_main();
		};
	};

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

//If serial port receives a space, exit display sequence and request console
static void main_rx(char x)
{
	if(x==' ')
	{
		sequence_exit();
		console_request=TRUE;
	};
}
