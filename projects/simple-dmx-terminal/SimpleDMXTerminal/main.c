//-------------------------------------------------------------------------
// Titel : DMX Control-SW for mySmartControl M168
//-------------------------------------------------------------------------
// Function : 	DMX Monitor program to 
//				- get commands from PC over the USB/Serial port
// 				- build DMX packages 
//				- send DMX packages to the DMX interface
//				- automatically every given time interval (default ~1sec)
//				- manually by command
//
// Hardware:	Program was developed and tested on a mySmartControl-M168 board
//				(http://shop.myavr.com/) connected and powered via USB to my PC
//				To drive DMX signals a small adapter is used with a 75176 driver 
//				and few resistors (see DMXControlAdapter.pdf).
//				For DMX output PortC is used: 	Bit 0 : DMX Data
//												Bit 1 : Enable Driver
//				
//				USB PC: Using virtual Comport driver on COM3, 9600 Baud, 8,n,1
//
// Software		GCC Toolset with AVR Studio 4.18
//				Interrupts for USART Receive und Timer used
//
//				Written by Gerhard Lehnerts, 2009
//				Gerhard.Lehnerts@t-online.de
//				
//-------------------------------------------------------------------------
// Processor : ATmega168
// Clock frequency : 20 MHz
// 
// Datum : 29.12.2009
// Version : 1.0
//-------------------------------------------------------------------------
// created AVR Studio 4 und gcc Toolset
//-------------------------------------------------------------------------

// #define 	F_CPU 	20000000 // handled by toolset
// 

#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include 	<avr/pgmspace.h>
#include 	<avr\io.h>
#include	<avr\interrupt.h>
#include	<util/crc16.h>

#include	"fifo.h"
#include	"DMX512.h"

#define 	BAUD 	9600

// Prescaler clk/1024 = 20Mhz / 1024 = 19531,25 Hz = 0x4C46
// For 1 second count from 0xFFFF-0x4C46 to 0xFFFF
uint16_t		TimerCount = 0xFFFF-0x4C46;

#define TRUE 	1
#define FALSE 	0

short DMXTransmitEnable = FALSE;	// No auto transmit
short DMXVerboseEnable = FALSE;		// Verbose off
short DEBUGEnable = FALSE;			// No debug information

// Prototypes
void uartPutChar(char);
void uartPutString(char *);
void init(void);
void intro(void);
int  main(void);

// Macros
#define uartPutLF()	(uartPutChar('\n'));

// Help Screen stored in Program Memory

char VersionString[] PROGMEM = "\nSimple DMX Terminal (SDMXT) V1.00\nWritten by Gerhard Lehnerts, Germany\n";

char Help_0[] PROGMEM = "\nCommands:";
char Help_1[] PROGMEM = "\n vs : Show version";
char Help_2[] PROGMEM = "\n dmxdu : DUmp channel buffer";
char Help_3[] PROGMEM = "\n dmxscv <ch> <value> : Set Channel Value";
char Help_4[] PROGMEM = "\n dmxtr (<repeat>) : Tranmit via DMX-Interface, <repeat> times";
char Help_5[] PROGMEM = "\n dmxauto (<timer value>) : Send DMX-package automatically every ~1 s or every (<timer value>)";
char Help_6[] PROGMEM = "\n dmxstop : Stop auto send DMX-packages";
char Help_7[] PROGMEM = "\n dmxsave (<set nr>) : Save channel data set to EEPROM";
char Help_8[] PROGMEM = "\n dmxread (<set nr>) : Read channel data set from EEPROM";
char Help_9[] PROGMEM = "\n dmxclr : Set all channel data to 0";

PGM_P string_table[] PROGMEM = 
{
    Help_0, Help_1, Help_2, Help_3, Help_4, Help_5,	Help_6,	Help_7,	Help_8,
	Help_9,

	0 // End marker. Don't forget!
};

/*----------------------------------------------------------------------
 *	Receive Buffer for USB/USART
 */

static fifo rx_fifo;

/*----------------------------------------------------------------------
 *	Initilize
 */

void init()
{
	// *** Init RX-Fifo
	fifo_init(&rx_fifo);

	// *** Init UART
	
	UCSR0B |= _BV(TXEN0);  	// Enable TX
	UCSR0B |= _BV(RXEN0); 	// Enable RX
	UBRR0L=(uint8_t)(F_CPU/(BAUD*16L))-1; 		// Define baud rate
	UBRR0H=(uint8_t)((F_CPU/(BAUD*16L))-1)>>8; 	// dto.
	
	// UART Interrupt RXReady enable
	UCSR0B |= _BV(RXCIE0); 	// Set RXCIE0

	// *** Init Timer

	// TCCR1A - Timer/Counter1 Control Register A
	// COM1A1 = COM1A0 = COM1B1 = COM1B0 = 0 : Normal port operation,
	//                                         OC1A/OC1B disconnected.  
	// WGM11 = WGM10 = 0 : Mode normal, Top 0xFFFF
	// counting direction is always up (incrementing)
	TCCR1A = 0b00000000;

	// TCCR1B - Timer/Counter1 Control Register B
	// ICNC1 = ICES1 = 0 : n/a
	// WGM13 = WGM12 = 0 : s. WGM11 and WGM10
	// CS12 = 1; CS11 = 0; CS10 = 1 : Prescaler clk/1024 = 20Mhz / 1024
	// = 19531,25 Hz = 0x4C46

	TCCR1B = _BV(CS12) | _BV(CS10);

	// TCCR1C - Timer/Counter1 Control Register C
	// FOC1A = FOC1B = 0 : n/a
	TCCR1C = 0;

	// TCNT1H and TCNT1L Timer/Counter1
	// Count from 0xFFFF-0x4C46 to 0xFFFF
	TCNT1 = TimerCount;

	// OCR1A, OCR1B : Output Compare Register
	// n/a
	// ICR1 : Input Capture Register
	// n/a

	// TIMSK1 Timer/Counter1 Interrupt Mask Register
	// ICIE1 = OCIE1B = OCIE1A = 0
	// TOIE1 = 1 : Enable Timer Overflow Interrupt
	TIMSK1 = _BV(TOIE1);

	// Enable Interrupts
	sei();

	// Init DMX Interface
	DMX_Init();
}

/*----------------------------------------------------------------------
 *
 *	ISR(USART_RX_vect,ISR_BLOCK)
 *
 *	Interrupt Service Routine "USART Receiver Buffer Ready" 
 * 	Writes received character to the receive fifo
 */

ISR(USART_RX_vect,ISR_BLOCK) { 	fifo_push(&rx_fifo,UDR0); }

/*----------------------------------------------------------------------
 *	ISR(TIMER1_OVF_vect,ISR_NOBLOCK)
 *
 *	Interrupt Service Routine Timer/Counter 1 overrun
 * 	Reload timer/counter. If enabled, call DMX_Transmit
 */

ISR(TIMER1_OVF_vect,ISR_NOBLOCK)
{
	// Reload Count Value
 	TCNT1 = TimerCount;
	if(DMXTransmitEnable)
	{
		DMX_Transmit(1);
		if(DMXVerboseEnable) uartPutChar('-');
	} else if(DMXVerboseEnable) uartPutChar('.');
}

/*----------------------------------------------------------------------
 *	void uartPutChar(char data)
 *
 *	Send character <char data> via UART
 */

void uartPutChar(char data)
{
	// Wait until UDR empty
	while (!(UCSR0A&_BV(UDRE0)));
	//sende
	UDR0=data;
}

/*----------------------------------------------------------------------
 *
 *	void uartPutString(char *buffer)
 *	void uartPutStringP(PGM_P p)
 *
 * 	Send string via UART
 *
 * 	IN 	char *buffer : String, NULL terminated
 *		PGM_P *buffer : String in program memory, NULL terminated
 */
 
void uartPutString(char *buffer)
{
	while(*buffer != 0)
		uartPutChar(*buffer++);
}

void uartPutStringP(PGM_P p)
{
	register char c;
	while((c = pgm_read_byte(p++)) != 0)
		uartPutChar(c);
}

/********************* DMX - Commands *********************************/

/*----------------------------------------------------------------------
 *
 *	void cmd_DMXHelp(void)
 *
 *	Show help screen
 */

void cmd_DMXHelp(void)
{
	PGM_P p; 
	char c;
	char buf[50];

	// Read Help-Info from program memory and show it
	// End of Stringtable is marked by a NULL-pointer
	int i = 0;
    while((p = (PGM_P)pgm_read_word(&(string_table[i++]))) != 0)
		while((c = pgm_read_byte(p++)))
			uartPutChar(c);

	uartPutLF();

	// Show global values
	sprintf(buf,"\nAutoTransmit=%d Timer=%u  Verbose=%d Debug=%d\n",
		DMXTransmitEnable,TimerCount,DMXVerboseEnable,DEBUGEnable);
	uartPutString(buf);

	// Show DMX inteface confiuration
	sprintf(buf,"NoC=%d NoS=%d ActS=%d", DMX_GetChannelCount(), DMX_GetNrSets(), DMX_GetActivChSetNr());
	uartPutString(buf);
}

/*----------------------------------------------------------------------
 *	
 *	int cmd_DMXAUTO(char *argv[], int argc)
 *
 *	Command DMX-Auto Transmit
 *
 *	IN:		1 Argument optional: New Timer value
 *	OUT:	0 ok, -1 on error
 */

int cmd_DMXAUTO(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR dmxauto (<time>) :";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";
	static char ErrMsg_3[] PROGMEM = " Bad Args\n";

	register unsigned int n;
	char buf[40];

	if((argc != 2) & (argc != 1)) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}

	DMXTransmitEnable = TRUE;

	if(argc == 2)
	{
		n = atoi(argv[1]);
		if((n > 0xFFFF) | (n < 10)) {
			uartPutStringP(ErrMsg_1);
			uartPutStringP(ErrMsg_3);
			return -1;
		}
		TimerCount = n;
		TCNT1 = TimerCount;	
	}

	// Show current timer value
	sprintf(buf,"\nTimer = %u ",TimerCount);
	uartPutString(buf);
	return 0;
}

/*----------------------------------------------------------------------
 *	
 *	int cmd_DMXTR(char *argv[], int argc)
 *
 *	Command DMX-Transmit
 *
 *	IN:		1 Argument optional: Repeat count for re-transmit 
 *		
 *	OUT:	0 ok, -1 on error
 */

int cmd_DMXTR(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR dmxtr (<repeat>) :";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";
	static char ErrMsg_3[] PROGMEM = " Bad Args\n";

	register int n;

	if((argc != 2) & (argc != 1)) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}

	if(argc == 1)
		DMX_Transmit(1);
	else {
		n = atoi(argv[1]);
		if((n > 10000) | (n < 0)) {
			uartPutStringP(ErrMsg_1);
			uartPutStringP(ErrMsg_3);
			return -1;
		}
		DMX_Transmit(n);
	}

	return 0;
}

/*----------------------------------------------------------------------
 *	
 *	int cmd_DMX_Save(char *argv[], int argc)
 *
 *	Command Save Set to EEPROM
 *
 *	IN:		1 Argument optional: Set-Nummber
 *			If not providet, activ set will be used 
 *
 *	OUT:	0 ok, -1 on error
 */

int cmd_DMX_Save(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR dmxsave (<set_nr>) :";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";
	static char ErrMsg_3[] PROGMEM = " Bad Args\n";

	register int n;

	if((argc != 2) & (argc != 1)) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}

	if(argc == 1) 
		DMX_SaveToEEPROM();
	else {
		n = atoi(argv[1]);
		if((n > 511) | (n < 0)) {
			uartPutStringP(ErrMsg_1);
			uartPutStringP(ErrMsg_3);
			return -1;
		}
		DMX_SetChSet(n);
		DMX_SaveToEEPROM();
	}

	return 0;
}

/*----------------------------------------------------------------------
 *	
 *	int cmd_DMX_Read(char *argv[], int argc)
 *
 *	Command Read Set from EEPROM
 +
 *	IN:		1 Argument optional: Set-Nummber
 *			If not providet, activ set will be used 
 *
 *	OUT:	0 ok, -1 on error
 */

int cmd_DMX_Read(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR dmxread (<set_nr>) :";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";
	static char ErrMsg_3[] PROGMEM = " Bad Args\n";
	static char ErrMsg_4[] PROGMEM = " Bad Set Nr\n";

	register int n;

	if((argc != 2) & (argc != 1)) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}

	if(argc == 1)
		DMX_ReadFromEEPROM();
	else {
		n = atoi(argv[1]);
		if((n > 511) | (n < 0)) {
			uartPutStringP(ErrMsg_1);
			uartPutStringP(ErrMsg_3);
			return -1;
		}
		if(DMX_SetChSet(n)) {
			uartPutStringP(ErrMsg_1);
			uartPutStringP(ErrMsg_4);
			return -1;
		}
		DMX_ReadFromEEPROM();
	}

	return 0;
}


/*----------------------------------------------------------------------
 * 	
 *	int cmd_DMXDU(char *argv[], int argc)
 *
 *	Command DMX-Channel Dump
 *	List all channel data of the activ set
 *
 *	IN:		Arguments: non
 * 
 *	OUT:	0 ok, -1 on error
 */

int cmd_DMXDU(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR dmxdu : ";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";

	char buf[40];

	if(argc != 1) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}

	// Show activ set nr
	sprintf(buf,"Set Nr=%d\n",DMX_GetActivChSetNr()); uartPutString(buf);

	for(int i = 0; i < DMX_GetChannelCount() ; i++)
	{
		uartPutString("Ch Nr ");
		sprintf(buf,"%03u Value: %03u  Changed: ", i, DMX_GetChannelData(i));
		uartPutString(buf);
		if(DMX_IsChannelChanged(i))
			uartPutString("Yes\n");
		else
			uartPutString("No\n");
	}

	return 0;
}

/*----------------------------------------------------------------------
 * 	
 *	int cmd_DMXSCV(char *argv[], int argc)
 *
 *	Command Set DMX-Channel Value
 *	Write new value for channel data to the channel buffer
 *
 *	IN:		Arguments: Channel nr, new value
 *
 *	OUT:	0 ok, -1 on error
 */
 
int cmd_DMXSCV(char *argv[], int argc)
{
	static char ErrMsg_1[] PROGMEM = "ERROR DMXSCV <nr> <value>:";
	static char ErrMsg_2[] PROGMEM = " Nr of Args\n";
	static char ErrMsg_3[] PROGMEM = " Bad Args\n";
	static char ErrMsg_4[] PROGMEM = " Invalid Channel Nr\n";

	register int n,v;
	if(argc != 3) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_2);
		return -1;
	}
	n = atoi(argv[1]); v = atoi(argv[2]);
	if((n >= 512) | (n < 0) | (v > 255) | (v < 0) ) {
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_3);
		return -1;
	}

	if(DMX_SetChannelValue(n ,(unsigned char) v) != 0)
	{
		uartPutStringP(ErrMsg_1);
		uartPutStringP(ErrMsg_4);
		return -1;
	}

	return 0;
}

/*----------------------------------------------------------------------
 *
 *	uint16_t getcmd(char *cmd_buf)
 *
 *	Read command line from receive fifo
 *
 *	IN:	*cmd_buf	Pointer to cmd line buffer
 *	
 *	Remark: Cmd line is allways terminated by NULL 
 */

#define LONGEST_COMMAND 20

uint16_t getcmd(char *cmd_buf)
{
	char *p = cmd_buf;
	short count = 0;
	uint16_t c; //register int c;
	uint16_t crc = 0xFFFF;

	while(count < LONGEST_COMMAND)
	{
		cli();
		c = fifo_pop(&rx_fifo);
		sei();
		if( c == '\n' || c == '\r' || c == ';')	// CR, LF or ';' found -> Cmd end
		{
			break; 
		}
		else if(isprint(c))
		{
			*p++ = c; count++;
		}
	}
	*p = '\0'; // Terminte string with NULL

	// Calculate Checksum
	while(count--)
	{
		//crc = _crc_ccitt_update (crc, (uint8_t) *cmd_buf++);
		crc = _crc16_update (crc, (uint8_t) *cmd_buf++);
	}
	
	return crc;
}


/////////////////////////////////////////////////////////////////////////////
// Main-Funktion
/////////////////////////////////////////////////////////////////////////////

#define MAX_NR_ARGUMENTS 5

int main()
{
	static char *argv[MAX_NR_ARGUMENTS];
	static int 	argc;
	const char *delimiter = " \t,;";
	char buf[10];
	uint16_t crc;

	static char command[LONGEST_COMMAND+1];
	
	init(); 	// Init

	// Read saved data from EEPROM, Channel Set 0
	DMX_ReadFromEEPROM();

	// Enable auto transmition of DMX package
	DMXTransmitEnable = TRUE;

	while (TRUE)  	// Mainloop-Begin
	{
		// Look for commands

		uartPutString("\n> ");
		crc = getcmd(command);

		// Split Cmd line in token
		argc = 0;
		if((argv[0] = strtok(command,delimiter)) != 0)
			for(argc = 1; (argv[argc] = strtok(NULL,delimiter)) != NULL; argc++);

		if(!argc) continue; // Nothing usable

		// Return CRC
		sprintf(buf,"%06u\n",crc);
		uartPutString(buf);

		// DEBUG Mode
		if(DEBUGEnable)
		{
			for(int i = 0; i < argc; i++) {
				uartPutString(argv[i]); uartPutChar(',');
			}
			uartPutLF();
		}

		// Command dispatcher
		if(!strcmp(argv[0],"vs")) uartPutStringP(VersionString);
		else if(!strcmp(argv[0],"dmxscv"))		{ cmd_DMXSCV(argv, argc);		}
		else if(!strcmp(argv[0],"dmxdu")) 		{ cmd_DMXDU(argv, argc);		}
		else if(!strcmp(argv[0],"dmxtr")) 		{ cmd_DMXTR(argv, argc);		}
		else if(!strcmp(argv[0],"dmxclr")) 		{ DMX_ClrChannels();			}
		else if(!strcmp(argv[0],"help"))  		{ cmd_DMXHelp();				}
		else if(!strcmp(argv[0],"dmxauto"))  	{ cmd_DMXAUTO(argv, argc);		}
		else if(!strcmp(argv[0],"dmxstop"))  	{ DMXTransmitEnable = FALSE;	}
		else if(!strcmp(argv[0],"dmxsave")) 	{ cmd_DMX_Save(argv, argc);		}
		else if(!strcmp(argv[0],"dmxread")) 	{ cmd_DMX_Read(argv, argc);		}
		else if(!strcmp(argv[0],"dmxvon")) 		{ DMXVerboseEnable = TRUE;		} // Hidden
		else if(!strcmp(argv[0],"dmxvoff")) 	{ DMXVerboseEnable = FALSE;		} // Hidden
		else if(!strcmp(argv[0],"debugon")) 	{ DEBUGEnable = TRUE;			} // Hidden
		else if(!strcmp(argv[0],"debugoff")) 	{ DEBUGEnable = FALSE;			} // Hidden
		else if(!strcmp(argv[0],"dmx")) 		{ uartPutString("dmx alive\n");	} // Hidden
		else {
			uartPutString("???");
		}
	}
	// Mainloop-End
	return(0);
}
//---------------------------------------------------------------------------
// That's all
