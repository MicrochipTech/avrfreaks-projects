/*

	Console module.
	
*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//	Called while waiting for input
	#define WAIT_LOOP			main_fly

// 	fail waiting for input after this much time
// 	TIMER_TICK must be defined (in seconds) as interval between calls to console_timer()
	#define INPUT_PROMPT_TIME	60

//	Bootloader address for Atmega64
	#define BOOTLOADER_ADDRESS 	0x0F000

	#define TEXTOUT_P(arg1)		uart1_tx_string_P(arg1)
	#define TEXTOUT(arg1)		uart1_tx_string(arg1)

	#define TEXTOUT_CHAR(arg1)	uart1_tx_char(arg1)
	#define TEXTOUT_INT(arg1)	uart1_tx_string(itoa(arg1, text, 10))
	#define TEXTOUT_FLOAT(arg1)	do { sprintf(text, "%5.2f", arg1); uart1_tx_string(text); } while(0)

//********************************************************************************************************
// Local defines
//********************************************************************************************************

/*
	All input functions and sub-menus return this enum.
	
	OK 		- Input was entered as expected.
	TIMEOUT	- User did not press a key for INPUT_PROMPT_TIME seconds
	ABORTED - User hit ESC
	NIL		- User hit Enter without typing any input (usually used to refresh menu)
	
*/
	enum input_result_enum{INPUT_RESULT_OK, INPUT_RESULT_TIMEOUT, INPUT_RESULT_ABORTED, INPUT_RESULT_NIL};

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static unsigned int		time;
	static char 			text[10];
	static volatile char	input_char;

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	//Invoke the bootloader
	static void bootload(void);
	
	//Functions for getting input, if successful, input is written to target pointer.
	static enum input_result_enum getint(int *target, int low, int high);
	static enum input_result_enum getline(char* buffer, int maxlen);
	static enum input_result_enum getkey(char* target, char low, char high);

	//show supply voltages while adjusting VR1
	static enum input_result_enum console_adjust_vled(void);
	
	//edit light threshholds
	static enum input_result_enum console_adjust_light(void);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

//Call to pass input characters to the console
void console_rx(char text_char)
{
	input_char=text_char;
}

//Call every timer tick to provide input timeout.
void console_tick(void)
{
	if(time)
		time--;
}

//Not required in this application
void console_init(void)
{
}

//Execute hardware test sequence for new devices/
void console_test(void)
{
	char tempchar;

	TEXTOUT_P(PSTR("\
\r\n\
Insert code here to test your hardware.\r\n\
Try to force the tester to pay attention.\r\n\
\r\n\
You should implement a password to skip the test, and a hidden menu option to re-test.\r\n\
\r\n\
The device should NOT operate until the test is completed.\r\n\
This prevents devices from accidently going into the field un-tested.\r\n\
(press any key)\r\n\
\r\n\
"));


	getkey(&tempchar, 0x0D, 0x7F);
	settings_ram.test=FALSE;
	settings_save();
	TEXTOUT_P(PSTR("TEST COMPLETE!\r\n"));
}

//Template for a submenu
/*
static enum input_result_enum menu_template(void)
{
	enum input_result_enum input_result;
	int option;
	char finished=FALSE;
	
	do
	{
		TEXTOUT_P(PSTR("\
\r\n\
****** Menu name ******\r\n\
\r\n\
More menu text\r\n\
\r\n\
1. Blah blah\r\n\
2. Blah blah\r\n\
0. exit\r\n\
"));
		
		input_result = getint(&option, 0, 2);

		if(input_result == INPUT_RESULT_OK)
		{
			switch(option)
			{
				case 1:
					//input result = menu_option1();
					break;
				
				case 2:
					//input result = menu_option2();
					break;
				
				case 0:
					finished=TRUE;
					break;			
			};
		};
		
		if(input_result == INPUT_RESULT_ABORTED)
			finished=TRUE;
		else if(input_result == INPUT_RESULT_TIMEOUT)
			finished=TRUE;

	}while( !finished);
	
	return input_result;
}
*/

//Top level menu of console
void console_main(void)
{
	enum input_result_enum input_result;
	int option;
	char finished=FALSE;
	
	do
	{
		TEXTOUT_P(PSTR("\
\r\n\
****** Main Menu ******\r\n\
\r\n\
1. Upload new firmware\r\n\
2. Default all settings\r\n\
3. Power supply test\r\n\
4. Adjust maximum LED supply\r\n\
5. Adjust day/night threshold\r\n\
6. Reset\r\n\
0. exit\r\n\
"));
		input_result = getint(&option, -1, 6);
		
		if(input_result == INPUT_RESULT_OK)
		{
			switch(option)
			{
				case -1:
					settings_ram.test=0xFF;
					settings_save();
					TEXTOUT_P(PSTR("RESET to start hardware test\r\n"));
					getkey(NULL, 0x0D, 0x7F);
					break;
				case 1:
					bootload();
					break;
				case 2:
					settings_default();
					break;			
				case 3:
					power_test();
					break;
				case 4:
					console_adjust_vled();
					break;
				case 5:
					console_adjust_light();
					break;
				case 6:
					led_blank();
					wdt_enable(WDTO_15MS);
					while(1);
					break;
			};
		};
		
		if(input_result == INPUT_RESULT_ABORTED)
			finished=TRUE;
		else if(input_result == INPUT_RESULT_TIMEOUT)
			finished=TRUE;
		
	}while(!finished);
	TEXTOUT_P(PSTR("Exit\r\n\r\n"));
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

//Show supply voltages while adjusting VR1
static enum input_result_enum console_adjust_vled(void)
{
	char finished=FALSE;
	enum input_result_enum input_result;
	double mA;
	
	led_framerate(60);
	power_vled_set(255);
	memset(led_frame_bitmap, 0xFF, 64 );
	led_plane_intensity_all(255);
	led_waitframe(30);

	do
	{
		mA = a2d_led_i();
		TEXTOUT_P(PSTR("\r\nCurrently:"));
		TEXTOUT_P(PSTR("\r\nSupply input         - "));
		TEXTOUT_FLOAT(a2d_supply_v());
		TEXTOUT_P(PSTR("v\r\nMaximum LED supply   - "));
		TEXTOUT_FLOAT(a2d_led_v());
		TEXTOUT_P(PSTR("v\r\nMaximum LED current  - "));
		TEXTOUT_FLOAT(mA);
		TEXTOUT_P(PSTR("mA\r\nLED forward drop     - "));
		TEXTOUT_FLOAT(a2d_led_fv());
		TEXTOUT_P(PSTR("v\r\n"));
		if(mA > 25)
			TEXTOUT_P(PSTR("Reccomend inserting JP1 for > 25mA\r\n"));
		if(mA > 40)
			TEXTOUT_P(PSTR("mA over 40mA not reccomended\r\n"));
		
		TEXTOUT_P(PSTR("Adjust VR1 and press enter to refresh, or ESC when done\r\n"));
		
		input_result = getint(NULL, 0, 0);
		if(input_result == INPUT_RESULT_ABORTED || input_result == INPUT_RESULT_TIMEOUT)
			finished = TRUE;
		
	}while(!finished);

	led_blank();
	
	return input_result;
};
	

//edit light threshholds
static enum input_result_enum console_adjust_light(void)
{
	enum input_result_enum input_result;
	int option;
	char finished=FALSE;
	
	do
	{
		
		TEXTOUT_P(PSTR("\r\n****************************\r\nCurrent light level = "));
		TEXTOUT_INT(a2d_light());
		TEXTOUT_P(PSTR("\r\nNight threshold < "));
		TEXTOUT_INT(settings_ram.threshold_night);
		TEXTOUT_P(PSTR("\r\nDay threshold   > "));
		TEXTOUT_INT(settings_ram.threshold_day);
		TEXTOUT_P(PSTR("\r\n\r\n1. Edit night threshold\r\n"));
		TEXTOUT_P(PSTR("2. Edit day threshold\r\n"));
		TEXTOUT_P(PSTR("(Enter-refresh, ESC-exit)\r\n"));
		input_result = getint(&option, 0, 2);
		
		if(input_result == INPUT_RESULT_OK)
		{
			switch(option)
			{
				case 1:
					TEXTOUT_P(PSTR("New night threshold "));
					input_result = getint((int*)&(settings_ram.threshold_night), 0, settings_ram.threshold_day);
					break;
				
				case 2:
					TEXTOUT_P(PSTR("New day threshold "));
					input_result = getint((int*)&(settings_ram.threshold_day), settings_ram.threshold_night, 1022);
					break;
			};
			settings_save();
		};
		
		if(input_result == INPUT_RESULT_ABORTED)
			finished=TRUE;
		else if(input_result == INPUT_RESULT_TIMEOUT)
			finished=TRUE;

	}while( !finished);
	
	return input_result;
}

static void bootload(void)
{
	void (*bootloader)(void)=  (void(*)(void))BOOTLOADER_ADDRESS;

	(*bootloader)();
}

// get an integer from the user and write it to *target, returns OK/ABORTED/TIMEOUT
// aborts if ESC is pressed.
// uses atoi, so any input that atoi cannot recognise as a number, will result in a valid entry of 0.
static enum input_result_enum getint(int* target, int low, int high)
{
	enum input_result_enum success;
	char finished=FALSE;
	int value;

	do
	{
		TEXTOUT_P(PSTR(">"));
		success=getline(text, 9);
		if(success==INPUT_RESULT_OK)
		{
			if(strlen(text))
			{
				value=atoi(text);
				//out of range
				if( value < low || high < value )
				{
					TEXTOUT_P(PSTR("Out of range! Enter number between "));
					TEXTOUT(itoa(low, text, 10));
					TEXTOUT_P(PSTR(" and "));
					TEXTOUT(itoa(high, text, 10));
					TEXTOUT_P(PSTR("\r\n"));
				}	
				//ok
				else
				{
					finished=TRUE;
					if(target)
						*target=value;
				};
			}
			else
			{
				success=INPUT_RESULT_NIL;
				finished=TRUE;
			};
		}
		else
			finished=TRUE;
	}while(!finished);

	return success;
}

// get's a line from input, terminates at CR, ignores outside 0x20-0x7E, writes it to *buffer
// returns OK/ABORT/TIMEOUT, does NOT return INPUT_RESULT_NIL, as an empty line is a valid entry.
// abort only occurs if ESC is hit, in which case buffer is terminated string of length 0
// maxlen includes terminator, must be non0
// attempting to enter text longer than maxlen-1 will simply return a truncated string once CR is received
// chars will not be echo'd beyond maxlen-1
static enum input_result_enum getline(char* buffer, int maxlen)
{
	enum input_result_enum success;
	int index=0;	//indexes terminator in buffer
	char tempchar;
	char finished=FALSE;

	if(buffer)
		buffer[index]=0;
	do
	{		
		//wait for input or timeout
		time = INPUT_PROMPT_TIME/TIMER_TICK;
		
		while(time && !input_char)
			WAIT_LOOP();
		
		if(!input_char)
		{
			if(buffer)
				buffer[0]=0;
			success=INPUT_RESULT_TIMEOUT;
			finished=TRUE;
		}
		else
		{
			tempchar = input_char;
			input_char = FALSE;
			
			//abort (esc)
			if(tempchar==0x1B)
			{
				if(buffer)
					buffer[0]=0;
				success=INPUT_RESULT_ABORTED;
				finished=TRUE;
			}
			
			//backspace
			else if(tempchar==0x08 && index && buffer)
			{
				TEXTOUT_P(PSTR("\x08 \x08"));
				buffer[--index]=0;
			}
			
			//text
			else if(0x1F < tempchar && tempchar < 0x7F && buffer)
			{
				if(index<maxlen-1)
				{
					TEXTOUT_CHAR(tempchar);
					buffer[index++]=tempchar;
					buffer[index]=0;
				};
			}
			
			//CR
			else if(tempchar=='\r')
			{
				finished=TRUE;
				success=INPUT_RESULT_OK;
			};
		};
	} while(!finished);

	TEXTOUT_P(PSTR("\r\n"));
	return success;
}

// get's a single character within range
// returns OK/ABORT/TIMEOUT
// abort only occurs if ESC is hit, in which case target is not modified
static enum input_result_enum getkey(char* target, char low, char high)
{
	enum input_result_enum success;
	char tempchar;
	char finished=FALSE;
	
	do
	{
		//wait for input or timeout
		time=INPUT_PROMPT_TIME/TIMER_TICK;
		while(time && !input_char)
			WAIT_LOOP();
		
		if(!input_char)
		{
			success=INPUT_RESULT_TIMEOUT;
			finished=TRUE;
		}
		else
		{
			tempchar=input_char;
			input_char=FALSE;
			
			if(low <= tempchar && tempchar <= high)
			{
				TEXTOUT_CHAR(tempchar);
				if(target)
					*target=tempchar;
				finished=TRUE;
				success=INPUT_RESULT_OK;
			}
			else if(tempchar==0x1B)
			{
				finished=TRUE;
				success=INPUT_RESULT_ABORTED;
			};
		};
		
	}while(!finished);

	TEXTOUT_P(PSTR("\r\n"));

	return success;
}
