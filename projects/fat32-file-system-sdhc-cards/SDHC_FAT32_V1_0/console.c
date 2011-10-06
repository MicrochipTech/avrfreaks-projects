/*
Console module, 
*/

// fail waiting for input after this much time
// TIMER_TICK must be defined (in seconds) as interval between calls to console_timer()
#define INPUT_PROMPT_TIME	60

#define BOOTLOADER_ADDRESS 	0x1E000

#include <includes.h>

#define TEXTOUT_P(arg1)		fifo_write_string_P(console_vars.output_ptr, arg1)
#define TEXTOUT(arg1)		fifo_write_string(console_vars.output_ptr, arg1)
#define TEXTOUT_CHAR(arg1)	fifo_write_char(console_vars.output_ptr, arg1)
#define TEXTOUT_INT(arg1)	fifo_write_string(console_vars.output_ptr, itoa(arg1, text, 10))
#define TEXTOUT_LONG(arg1)	fifo_write_string(console_vars.output_ptr, ltoa(arg1, text, 10))

enum 	input_result_enum{INPUT_RESULT_OK, INPUT_RESULT_TIMEOUT, INPUT_RESULT_ABORTED, INPUT_RESULT_NIL};

// **** Private Prototypes ****

static void bootload(void);

static enum input_result_enum getint(int *target, int low, int high);
static enum input_result_enum getlong(long* target, long low, long high);
static enum input_result_enum getline(char* buffer, int maxlen);
static enum input_result_enum getyn(char* target);
static enum input_result_enum getkey(char* target, char low, char high);

// **** Public variables ****

	struct console_struct console_vars;

// **** Private variables ****

	static volatile int time;
	static char text[100];
	char	copy_buffer[512];

void console_timer(void)
{
	if(time)
		time--;
}

void console_init(void)
{
	//initialise console input fifo, uses malloc to create input buffer
	fifo_init(&console_vars.input, CONSOLE_INPUT_FIFO_SIZE, NULL, NULL, NULL);
}



void console_main(void)
{
	enum input_result_enum success;
	int option;
	char tempchar;
	long templong=0, templong2=0, templong3=0;

	struct fat_file_struct	fat_file_info, fat_file_info2;
	unsigned long fat_directory=0;

	sd_debug_fifo_ptr=console_vars.output_ptr;
	fat_debug_fifo_ptr=console_vars.output_ptr;
	fat_dir_fifo_ptr=console_vars.output_ptr;

	do
	{
		if(fat_directory)
			fat_directory_list(fat_directory);
		
		TEXTOUT_P(PSTR("\
\r\n\
****** Menu ******\r\n\
1. Upload new firmware\r\n\
2. Init SD card (do this first)\r\n\
3. Init FAT32 (do this second)\r\n\
4. Directory\r\n\
5. Change directory\r\n\
6. Create file\r\n\
7. Create sub-directory\r\n\
8. Delete sub-directory\r\n\
9. Delete file\r\n\
10. Append to file\r\n\
11. debug FAT table list\r\n\
12. debug DIR table list\r\n\
13. copy file\r\n\
14. show file as text\r\n\
15. play raw sound file (15.686kHz 8-bit unsigned)\r\n\
0. exit\r\n\
"));

		success=getint(&option, -1, 50);
		//if input failed, exit menu, or refresh menu
		if(success != INPUT_RESULT_OK)
			option=0;
		
		switch(option)
		{
			case 1:
				bootload();
				break;
			case 2:
				if(sd_initcard())
					TEXTOUT_P(PSTR("OK.\r\n"));
				else
					TEXTOUT_P(PSTR("Failed.\r\n"));
				break;
			case 3:
				if(fat_init())
				{
					TEXTOUT_P(PSTR("OK.\r\n"));
					fat_directory=2ul;
				}
				else
					TEXTOUT_P(PSTR("Failed.\r\n"));
				break;			
			case 4:
				fat_directory_list(fat_directory);
				break;
			case 5:
				TEXTOUT_P(PSTR("Enter name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
					fat_directory_change(&fat_directory, text);
				break;
			case 6:
				TEXTOUT_P(PSTR("Enter file size >"));
				if(getlong(&templong, 0, 0x7FFFFFFFul) == INPUT_RESULT_OK)
				{
					TEXTOUT_P(PSTR("Enter file name >"));
					if(getline(text, 100)==INPUT_RESULT_OK)
					{
						fat_file_info.cluster_directory=fat_directory;
						fat_file_create(&fat_file_info, fat_directory, text, templong);
					};
				};
				break;
				
			case 7:
				TEXTOUT_P(PSTR("Enter sub directory name >"));
				if(getline(text, 100) == INPUT_RESULT_OK)
					fat_directory_create(fat_directory, text);
				break;
				
			case 8:
				TEXTOUT_P(PSTR("Enter sub directory name >"));
				if(getline(text, 100) == INPUT_RESULT_OK)
					fat_directory_delete(fat_directory, text);
				break;
				
			case 9:			
				TEXTOUT_P(PSTR("Enter file name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
				{
					if(fat_file_locate(&fat_file_info, fat_directory, text))
						fat_file_delete(&fat_file_info);
				};
				break;
				
			case 10:
				TEXTOUT_P(PSTR("Enter file name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
				{
					if(fat_file_locate(&fat_file_info, fat_directory, text))
					{
						TEXTOUT_P(PSTR("Enter some text to append >"));
						if(getline(text, 100)==INPUT_RESULT_OK)
						{
							fat_file_append(&fat_file_info, text, strlen(text)+1);
						};
					};
				};
				break;
				
			case 11:
				do
				{
					TEXTOUT_P(PSTR("Enter start cluster (esc to finish)"));
					success=getlong(&templong, 2, 0x7FFFFFFFul);
					if(success==INPUT_RESULT_OK)
						fat_debug_fat(templong);
				}while(success==INPUT_RESULT_OK);
				break;
			case 12:
				TEXTOUT_P(PSTR("Enter directory base cluster "));
				success=getlong(&templong, 0, 0x7FFFFFFFul);
				do
				{	
					TEXTOUT_P(PSTR("Enter start index (esc to finish)"));
					success=getlong(&templong2, 0, 65535);
					if(success==INPUT_RESULT_OK)
						fat_debug_dir(templong, (unsigned int)templong2);
				}while(success==INPUT_RESULT_OK);
				break;
			case 13:

				TEXTOUT_P(PSTR("Enter file name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
				{
					if(fat_file_locate(&fat_file_info, fat_directory, text))
					{
						templong=fat_file_info.size;
						templong2=0;
						
						TEXTOUT_P(PSTR("Enter target file name >"));
						if(getline(text, 100)==INPUT_RESULT_OK)
						{
							if(fat_file_create(&fat_file_info2, fat_directory, text, templong));
							{
								while(templong)
								{
									if(fat_file_info.chain.physical != templong3)
									{
										templong3=fat_file_info.chain.physical;
										TEXTOUT_P(PSTR("."));
									};
									
									if(templong < 512)
									{
										fat_file_read(&fat_file_info, copy_buffer, templong2, templong);
										fat_file_write(&fat_file_info2, copy_buffer, templong2, templong);
										templong=0;
									}
									else
									{
										fat_file_read(&fat_file_info, copy_buffer, templong2, 512);
										fat_file_write(&fat_file_info2, copy_buffer, templong2, 512);
										templong -= 512;
										templong2+= 512;
									};
								};
							};
						};
					};
				};
				
				break; 
				
			case 14:
				TEXTOUT_P(PSTR("Enter file name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
				{
					if(fat_file_locate(&fat_file_info, fat_directory, text))
					{
						templong=fat_file_info.size;
						templong2=0;
						
						while(templong)
						{							
							fat_file_read(&fat_file_info, &tempchar, templong2, 1);
							//filter non-displayable chars
							tempchar &=0x7F;
							if( (tempchar < 0x20) && (tempchar !=0x0A) && (tempchar !=0x0D) )
								tempchar='_';
								
							TEXTOUT_CHAR(tempchar);							
							templong--;
							templong2++;
						};
					};
				};
				TEXTOUT_P(PSTR("\r\n\r\n"));
				break;
		
			case 15:
				
				TEXTOUT_P(PSTR("Enter file name >"));
				if(getline(text, 100)==INPUT_RESULT_OK)
				{
					if(fat_file_locate(&fat_file_info, fat_directory, text))
					{
						templong=fat_file_info.size;
						templong2=0;
						
						while(templong)
						{
							if(fat_file_info.chain.physical != templong3)
							{
								templong3=fat_file_info.chain.physical;
								TEXTOUT_P(PSTR("Cluster - "));
								TEXTOUT_LONG(templong3);
								TEXTOUT_P(PSTR("\r\n"));
							};
							
							//wait for frame
							audio_swapflag=FALSE;
							while(audio_swapflag==FALSE)
								main_fly();
							if(templong < AUDIO_BUFFER_SIZE)
							{
								fat_file_read(&fat_file_info, audio_buffer, templong2, templong);
								memset(&audio_buffer[templong], 0x80, AUDIO_BUFFER_SIZE-templong);
								templong=0;
							}
							else
							{
								fat_file_read(&fat_file_info, audio_buffer, templong2, AUDIO_BUFFER_SIZE);
								templong -= AUDIO_BUFFER_SIZE;
								templong2+= AUDIO_BUFFER_SIZE;
							};
						};
						
						//silence both frames
						audio_swapflag=FALSE;
						while(audio_swapflag==FALSE)
							main_fly();
						memset(audio_buffer, 0x80, AUDIO_BUFFER_SIZE);
						audio_swapflag=FALSE;
						while(audio_swapflag==FALSE)
							main_fly();
						memset(audio_buffer, 0x80, AUDIO_BUFFER_SIZE);
					};
				};
				break;
		};
	}while( option || success==INPUT_RESULT_NIL );
	TEXTOUT_P(PSTR("Exit\r\n"));
}

static void bootload(void)
{
	void (*bootloader)(void)=  (void(*)(void))BOOTLOADER_ADDRESS;

	if( console_vars.input_device == CONSOLE_DEVICE_OTHER )
	{
		TEXTOUT_P(PSTR("Cannot use bootloader from this I/O device (hit space)\r\n"));
		while(fifo_read_char(&console_vars.input)!=' ');
	}
	else
	{
		//disable all interrupts (as we will switch vector table)
		cli();
		EIMSK =0;											//external int's
	
		TIMSK0=0;											//timer int's
		TIMSK1=0;											//timer int's
		TIMSK2=0;											//timer int's
		TIMSK3=0;											//timer int's
		TIMSK4=0;											//timer int's
		TIMSK5=0;											//timer int's	
		
		SPCR &=~_BV(SPIE);									//spi int
		
		UCSR0B &=~(_BV(RXCIE0)+_BV(TXCIE0)+_BV(UDRIE0));	//uart0 ints
		UCSR1B &=~(_BV(RXCIE1)+_BV(TXCIE1)+_BV(UDRIE1));	//uart1 ints

		TWCR &=~_BV(TWIE);									//two wire int
		ACSR &=~_BV(ACIE);									//analog comparator
		ADCSRA &=~_BV(ADIE);								//a2d
		
		//Bootloader will detect which uart is left enabled.
		if( console_vars.input_device == CONSOLE_DEVICE_UART0 )
			UCSR1B &= ~_BV(RXEN1);	//disable uart1
		if( console_vars.input_device == CONSOLE_DEVICE_UART1 )
			UCSR0B &= ~_BV(RXEN0);	//disable uart0	
		(*bootloader)();
	};
}

// get an integer from the user and write it to *target, returns OK/ABORTED/TIMEOUT
// aborts if ESC is pressed.
// note that any input that atoi cannot recognise as a number, will result in 0.
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

// get an integer from the user and write it to *target, returns OK/ABORTED/TIMEOUT
// aborts if ESC is pressed.
// note that any input that atol cannot recognise as a number, will result in 0.
static enum input_result_enum getlong(long* target, long low, long high)
{
	enum input_result_enum success;
	char finished=FALSE;
	long value;

	do
	{
		TEXTOUT_P(PSTR(">"));
		success=getline(text, 9);
		if(success==INPUT_RESULT_OK)
		{
			if(strlen(text))
			{
				value=atol(text);
				//out of range
				if( value < low || high < value )
				{
					TEXTOUT_P(PSTR("Out of range! Enter number between "));
					TEXTOUT(ltoa(low, text, 10));
					TEXTOUT_P(PSTR(" and "));
					TEXTOUT(ltoa(high, text, 10));
					TEXTOUT_P(PSTR("\r\n"));
				}	
				//ok
				else
				{
					finished=TRUE;
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
// a line input length of 0 is a valid entry (returns OK)
// maxlen includes terminator, must be non0
// attempting to enter text longer than maxlen-1 will simply return a truncated string once CR is received
// if echo is enabled chars will not be echo'd beyond maxlen-1
static enum input_result_enum getline(char* buffer, int maxlen)
{
	enum input_result_enum success;
	int index=0;	//indexes terminator in buffer
	char tempchar;
	char finished=FALSE;

	buffer[index]=0;
	do
	{		
		//wait for input or timeout
		time=(INPUT_PROMPT_TIME/TIMER_TICK);
		
		while(time && console_vars.input.empty==TRUE)
			main_fly();
		
		if(console_vars.input.empty==TRUE)
		{
			buffer[0]=0;
			success=INPUT_RESULT_TIMEOUT;
			finished=TRUE;
		}
		else
		{
			tempchar=fifo_read_char(&console_vars.input);
			
			//abort (esc)
			if(tempchar==0x1B)
			{
				buffer[0]=0;
				success=INPUT_RESULT_ABORTED;
				finished=TRUE;
			}
			
			//backspace
			else if(tempchar==0x08 && index)
			{
				if(console_vars.echo)
					TEXTOUT_P(PSTR("\x08 \x08"));
				buffer[--index]=0;
			}
			
			//text
			else if(0x1F < tempchar && tempchar < 0x7F)
			{
				if(index<maxlen-1)
				{
					if(console_vars.echo)
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

	if(console_vars.echo)
	{
		TEXTOUT_P(PSTR("\r\n"));
	};
	return success;
}

// get's a y or n response, writes non-0 to target if yes
// returns OK/ABORT/TIMEOUT
// abort only occurs if ESC is hit, in which case target is not modified
static enum input_result_enum getyn(char* target)
{
	enum input_result_enum success;
	char tempchar;
	char finished=FALSE;

	do
	{
		//wait for input or timeout
		time=INPUT_PROMPT_TIME/TIMER_TICK;
		while(time && console_vars.input.empty==TRUE)
			main_fly();
		
		if(console_vars.input.empty==TRUE)
		{
			success=INPUT_RESULT_TIMEOUT;
			finished=TRUE;
		}
		else
		{
			tempchar=fifo_read_char(&console_vars.input);
			if(tempchar=='y' || tempchar=='Y')
			{
				if(console_vars.echo)
					TEXTOUT_CHAR(tempchar);
				*target=TRUE;
				finished=TRUE;
				success=INPUT_RESULT_OK;
			}
			else if(tempchar=='n' || tempchar=='N')
			{
				if(console_vars.echo)
					TEXTOUT_CHAR(tempchar);
				*target=FALSE;
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
		while(time && console_vars.input.empty==TRUE)
			main_fly();
		
		if(console_vars.input.empty==TRUE)
		{
			success=INPUT_RESULT_TIMEOUT;
			finished=TRUE;
		}
		else
		{
			tempchar=fifo_read_char(&console_vars.input);
			if(low <= tempchar && tempchar <= high)
			{
				if(console_vars.echo)
					TEXTOUT_CHAR(tempchar);
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
