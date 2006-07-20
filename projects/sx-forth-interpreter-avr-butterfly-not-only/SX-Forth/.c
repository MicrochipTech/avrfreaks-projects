/*
								********************
								**** SX-Forth.C ****
								********************

	Valter Foresto
	Comm.SEC - Torino
	<valter.foresto@tematsrl.it>

	Description:   Kernel of SX System eXecutive and optional Forth INTERPRETER
	IDE:           AVR Studio 4.12, Service Pack 3, Build 488
	Compiler:		WinAVR (GCC) Build 22060421
	Target:			AVR Butterfly / ATmega169
	Tools:			JTAG-ICE MKII, AVRisp

	*** Release:   20060713
	Notes:	...

*/

/*------------------------------------------------------------------
								SX-Forth LICENCE
--------------------------------------------------------------------

Copyright (C) 1997...2006  Valter Foresto

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA or download the GNU General Public License from
"http://www.gnu.org/licenses/" web site.

Comm.SEC di Valter Foresto, hereby disclaims all copyright
interest in the program `SX and it's components' that build the
Forth System eXecutive written by Valter Foresto.

--------------------------------------------------------------------
Comm.SEC di Valter Foresto
Via Quittengo 37 - 10154 Torino
valter.foresto@tematsrl.it
Tel/Fax +39 011 232539
http://digilander.libero.it/Comm.SEC/
-------------------------------------------------------------------*/

//==============================================================================================================================
#include "Defines.h"
#define  SX_FORTH_MODULE


#define  FIRMWARE_IS_INTERRUPTS_DRIVEN    1  // Application Firmware Interrupts Driven ONLY
#define  STDIO_USED                       1
#define  RAM_USER_APPLICATION_BYTES      64

// System Timing: see 'RTC_init()' for proper settings (7.8125 ... 8000 msec based on 32.768kHz RTC)
#if STDIO_USED
	#define  MSEC_SYSTEM_TICK   8        // 7.8125 msec (128 ticks/sec)
#else
	#define  MSEC_SYSTEM_TICK   63       // 62.5 msec (16 ticks/sec)
#endif

#include "SX-Forth.h"
//==============================================================================================================================
//                                        Internal OSC, RTC and System Timing
//==============================================================================================================================

void OSCCAL_calibration( void )
	{
	s8    calibrate = 0;
	u16   temp;
	u8    tempL;

	CLKPR = (1<<CLKPCE);                   // set Clock Prescaler Change Enable
	CLKPR = (1<<CLKPS1) | (1<<CLKPS0);     // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
	TIMSK2 = 0;                            // disable OCIE2A and TOIE2
	ASSR = (1 << AS2);                     // select asynchronous operation of timer2 (32,768kHz)
	OCR2A = 200;                           // set timer2 compare value
	TIMSK0 = 0;                            // delete any interrupt sources
	TCCR1B = (1 << CS10);                  // start timer1 with no prescaling
	TCCR2A = (1 << CS20);                  // start timer2 with no prescaling
	while((ASSR & 0x01) | (ASSR & 0x04)) ; // wait for TCN2UB and TCR2UB to be cleared

	DelayMSEC(1000);                       // wait for external crystal to stabilise after a power-on

	while(calibrate == 0)
		{
		TIFR1 = 0xFF;                       // delete TIFR1 flags
		TIFR2 = 0xFF;                       // delete TIFR2 flags
		TCNT1H = 0;                         // clear timer1 counter
		TCNT1L = 0;
		TCNT2 = 0;                          // clear timer2 counter

		while ( !(TIFR2 & (1<<OCF2A)) ) ;   // wait for timer2 compareflag
		TCCR1B = 0;                         // stop timer1
		if ( (TIFR1 & (1<<TOV1)) )
			{
			temp = 0xFFFF;                   // if timer1 overflows, set the temp to 0xFFFF
			}
		else
			{                                // read out the timer1 counter value
			tempL = TCNT1L;
			temp = TCNT1H;
			temp = (temp << 8);
			temp += tempL;
			}

		if (temp > 6250)
			OSCCAL--;                        // the internRC oscillator runs to fast, decrease the OSCCAL
		else if (temp < 6120)
			OSCCAL++;                        // the internRC oscillator runs to slow, increase the OSCCAL
		else
			calibrate = 1;                   // the interRC is correct

		TCCR1B = (1 << CS10);               // start timer1
		}

	CLKPR = (1 << CLKPCE);                 // set Clock Prescaler Change Enable
	CLKPR = (1<<CLKPS1) | (1<<CLKPS0);     // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
	}

// Assume 1 MHz CPU Core Clock
void DelayMSEC(u16 millisec)
	{
	u8 i;

	 while (millisec--)
		{
		for (i=0; i<125; i++)
			asm volatile ("nop"::);
		}
	}

void RTC_init( void )
	{
	// SX Timer Interrupt Tick occurs any MSEC_SYSTEM_TICK, running from the RTC 32.768kHz crystal Asyncronous Timer 2
	DelayMSEC(100);                        // wait for external crystal to stabilise after a power-on

	ClearPortBit(TIMSK2, TOIE2);           // disable OCIE2A and TOIE2
	ASSR = (1 << AS2);                     // select asynchronous operation of Timer2
	TCNT2 = 0;                             // clear TCNT2A

	#if (MSEC_SYSTEM_TICK <= 8)
	 TCCR2A = (1<<CS20);                          // no precaler: 32.768 kHz       7.8125 msec each Timer2 overflow interrupt
	#elif (MSEC_SYSTEM_TICK <= 63)
	 TCCR2A = (1<<CS21);                          // precaler: 32.768 kHz /   8  = 62.5   msec each Timer2 overflow interrupt
	#elif (MSEC_SYSTEM_TICK <= 250)
	 TCCR2A = (1<<CS21) | (1<<CS20);              // precaler: 32.768 kHz /  32  = 250    msec each Timer2 overflow interrupt
	#elif (MSEC_SYSTEM_TICK <= 500)
	 TCCR2A = (1<<CS22);                          // precaler: 32.768 kHz /  64  = 500    msec each Timer2 overflow interrupt
	#elif (MSEC_SYSTEM_TICK <= 1000)
	 TCCR2A = (1<<CS22) | (1<<CS20);              // precaler: 32.768 kHz / 128  = 1       sec each Timer2 overflow interrupt
	#elif (MSEC_SYSTEM_TICK <= 2000)
	 TCCR2A = (1<<CS22) | (1<<CS21);              // precaler: 32.768 kHz / 256  = 2       sec each Timer2 overflow interrupt
	#else
	 TCCR2A = (1<<CS22) | (1<<CS21) | (1<<CS20);  // precaler: 32.768 kHz / 1024 = 8       sec each Timer2 overflow interrupt
	#endif

	while((ASSR & 0x01) | (ASSR & 0x04)) ; // wait for TCN2UB and TCR2UB to be cleared

	TIFR2 = 0xFF;                          // clear interrupt-flags
	SetPortBit(TIMSK2, TOIE2);             // enable Timer2 overflow interrupt
	}

//==============================================================================================================================
//                                              Internal EEPROM
//==============================================================================================================================

void EEPROM_Read(s8 *ramDest, s16 eepromAddr, s16 readLen)
	{
	// Wait for completion of previous EEPROM write
	while(EECR & (1<<EEWE)) ;

	#if (FIRMWARE_IS_INTERRUPTS_DRIVEN == 0)
	 cli();
	#endif

	while (readLen-- > 0)
		{
		// Set up the EEPROM address
		EEAR = eepromAddr++;
		// Start EEPROM read
		EECR |= (1<<EERE);

		// Store EEPROM readed data into RAM Destination Buffer
		*(ramDest++) = EEDR;
		}

	#if (FIRMWARE_IS_INTERRUPTS_DRIVEN == 0)
	 sei();
	#endif
	}

void EEPROM_Write(s16 eepromAddr, s8 *ramSource, s16 writeLen)
	{
	#if (FIRMWARE_IS_INTERRUPTS_DRIVEN == 0)
	 cli();
	#endif

	while (writeLen--)
		{
		// Wait for completion of previous or current EEPROM write
		while(EECR & (1<<EEWE)) ;

		// Set up the EEPROM address
		EEAR = eepromAddr++;
		EEDR = *(ramSource++);
		// Write logical one to EEMWE
		EECR |= (1<<EEMWE);
		// Start EEPROM write
		EECR |= (1<<EEWE);
		}

	#if (FIRMWARE_IS_INTERRUPTS_DRIVEN == 0)
	 sei();
	#endif
	}


//------------------------------------------------------------------------------------------------------------------------------
//                                        ERROR TRAP Function
//------------------------------------------------------------------------------------------------------------------------------

// 'errCode' defines the index in the user terminal messages table used if STDIO_USED is set to TRUE
#define  STACK_FULL                          0
#define  WORD_UNKNOWN                        1
#define  FUNCTION_UNKNOWN                    2
#define  MEMORY_UNKNOWN                      3
#define  INSUFFICIENT_PARAMETERS             4
#define  WRITE_DISABLED                      5
#define  STDIN_RING_BUFFER_FULL              6
#define  PARSER_RING_BUFFER_FULL             7

#if STDIO_USED
 const s8 _STACK_FULL_[] PROGMEM                      = "STACK_FULL";
 const s8 _WORD_UNKNOWN_[] PROGMEM                    = "WORD_UNKNOWN";
 const s8 _FUNCTION_UNKNOWN_[] PROGMEM                = "FUNCTION_UNKNOWN";
 const s8 _MEMORY_UNKNOWN_[] PROGMEM                  = "MEMORY_UNKNOWN";
 const s8 _INSUFFICIENT_PARAMETERS_[] PROGMEM         = "INSUFFICIENT_PARAMETERS";
 const s8 _WRITE_DISABLED_[] PROGMEM                  = "WRITE_DISABLED";
 const s8 _STDIN_RING_BUFFER_FULL_[] PROGMEM          = "STDIN_RING_BUFFER_FULL";
 const s8 _PARSER_RING_BUFFER_FULL_[] PROGMEM         = "PARSER_RING_BUFFER_FULL";


// user terminal errors messages table 'errMsg[]', 'errCode' is used as the index for message printout
 const s8 *errMsg[] PROGMEM = {
	_STACK_FULL_,
	_WORD_UNKNOWN_,
	_FUNCTION_UNKNOWN_,
	_MEMORY_UNKNOWN_,
	_INSUFFICIENT_PARAMETERS_,
	_WRITE_DISABLED_,
	_STDIN_RING_BUFFER_FULL_,
	_PARSER_RING_BUFFER_FULL_
	};
#endif

// result code for C and Forth functions / words
#ifndef CONTINUE
	#define CONTINUE  0
#endif
#ifndef READY
	#define READY     1
#endif
#ifndef EXIT
	#define EXIT      -1
#endif
SX_FORTH_VARIABLE s8 resCode;

// readables from the Forth terminal as '.ERR' and '.ERRCNT' ... cleared when readed
SX_FORTH_VARIABLE u8 err;
SX_FORTH_VARIABLE u8 errCnt;

void Error(u8 errCode)
	{
  #if STDIO_USED
	PGM_VOID_P  msgTable;
	PGM_P       msgPtr;
  #endif

	// always flush dataSTACK and returnSTACK
	dataTOS = 0;
  #if STDIO_USED
	returnTOS = 0;
  #endif
	// count errors and save the least 'errCode'
	err = errCode;
	++errCnt;

	// any call to 'void Error(s8 errCode)' set resCode to EXIT
	resCode = EXIT;

	// errors signalling if hardware dependent ... but we can have ALWAYS evidence of errors for making correct decisions !
  #if STDIO_USED
	// ... if user terminal available ... print 'errCode' to user
	USART_PutFlashString(PSTR(" -> "));
	msgTable = errMsg;
	errCode <<= 1;
	msgTable += errCode;
	msgPtr = (PGM_P)(pgm_read_word(msgTable));
	USART_PutFlashString(msgPtr);
	USART_Rx_Flush();
  #else
	// ... if user terminal is NOT available ... sound a Buzzer, blink a Leds, ...
	// define a user procedure for quit signalling ... Button pressed, ... , at least Reset the hardware
  #endif
	}

//------------------------------------------------------------------------------------------------------------------------------
//                                      C and Forth unified PUSH and POP Functions
//------------------------------------------------------------------------------------------------------------------------------

s16 Pop(void)
	{
	if (dataTOS >= 1)
		return(dataSTACK[--dataTOS]);
	else
		{
		Error(INSUFFICIENT_PARAMETERS);
		return(0);
		}
	}

void Push(s16 value)
	{
	if (dataTOS < (DATA_STACK_MAX_ENTRIES-1))
		dataSTACK[dataTOS++] = value;
	else
		Error(STACK_FULL);
	}


#if STDIO_USED
//==============================================================================================================================
//                                STDIO, SHELL, PARSER and Forth INTERPRETER Functions
//==============================================================================================================================

void USART_Init(u16 ubrrBaudrate) // UBRR used value
	{
	// Set baud rate
	UBRRH = (u16)(ubrrBaudrate >> 8);
	UBRRL = (u16)(ubrrBaudrate);
	// Enable 2x speed
	UCSRA = (1<<U2X);
	// Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);
	// Async. mode, 8N1
	UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
	}

void USART_Tx(s8 txChar)
	{
	// return after 'txChar' transmission
	while (!(UCSRA & (1<<UDRE))) ;
	UDR = txChar;
	}

s16 USART_Rx_Check(void)
	{
	// return -1 if not 'rxChar' ready, else 0x0000 ... 0x00FF 'rxChar'
	if ((UCSRA & (1<<RXC)))
		return ((s16)(UDR) & 0x00FF);
	else
		return (-1);
	}

void USART_Rx_Flush(void)
	{
	do {
		DelayMSEC(100);
		} while (USART_Rx_Check() >= 0);
	}

void USART_PutRamString(s8 *s)
	{
  	while (*s)
		USART_Tx(*s++);
	}

void USART_PutRamString_UpperCase(s8 *s)
	{
	// put string from RAM, convert to upper case alphabetic characters
	while (*s)
		{
		*s = toupper(*s);
		USART_Tx(*s++);
		}
	}

#if 0
 void USART_PutRamBuffer(s8 *b, u8 len)
	{
	while (len--)
		USART_Tx(*b++);
	}
#endif

void USART_PutFlashString(const s8 *s)
	{
	s8 c;

	do	{
		c = pgm_read_byte(s++);
		if (c == '\0')
			break;
		USART_Tx(c);
	 	} while(1);
	}

s16 xtoi (s8 *s)
	{
	s16 res = 0;
	s8  tmp;

	while (*s != '\0')
		{
		tmp = toupper(*s);
		if ((tmp >= '0') && (tmp <= '9'))
			tmp -= '0';
		else if ((tmp >= 'A') && (tmp <= 'F'))
			tmp = tmp - 'A' + 10;
		else
			break;
		res <<= 4;
		res |= tmp;
		++s;
		}
	return (res);
	}


// BUILTIN 'numberBase'
#define  DECIMAL     10
#define  EXADECIMAL  16 // default BASE
SX_FORTH_VARIABLE s16 numberBase;

// BUILTIN 'debug' and 'step'
SX_FORTH_VARIABLE s8 debug;   // default 0=FALSE
SX_FORTH_VARIABLE s8 step;

//** builtin interpreter I and J variables *********************************
SX_FORTH_VARIABLE s16 i_var, j_var;


// 'memoryType' used on memories access functions
#define  RAM      0
#define  EEPROM   1
#define  FLASH    2
SX_FORTH_VARIABLE u8 memoryType; // default RAM

// The Forth STDIN Channel is the STDIO Terminal for interaction with User and MEMORIES when it execute a Script
#define  TERMINAL_STDIN       0  // default
#define  RAM_STDIN            1
#define  EEPROM_STDIN         2
#define  FLASH_STDIN          3
SX_FORTH_VARIABLE u8    forth_Interpreter_Stdin_Channel;
SX_FORTH_VARIABLE s8    *forth_Interpreter_Stdin_MemoryAddress;   // default is NULL

// The Forth STDOUT Channel is usually the STDIO Terminal but the channel can be redirected to MEMORIES or other PORTs
#define  TERMINAL_STDOUT      0  // default
#define  RAM_STDOUT           1
#define  EEPROM_STDOUT        2
#define  PORT_STDOUT          3
SX_FORTH_VARIABLE u8    forth_Interpreter_Stdout_Channel;
SX_FORTH_VARIABLE s8    *forth_Interpreter_Stdout_MemoryAddress;  // default is NULL

// C and Forth Data STACK (Last IN, First OUT behaviour)
#define DATA_STACK_MAX_ENTRIES      16
SX_FORTH_VARIABLE u8    dataTOS;    // Top Of C and Forth DATA Stack
SX_FORTH_VARIABLE s16   dataSTACK[DATA_STACK_MAX_ENTRIES];

// Forth RETURN STACK (Last IN, First OUT behaviour)
#define RETURN_STACK_MAX_ENTRIES    4
SX_FORTH_VARIABLE u8    returnTOS;    // Top Of Forth RETURN Stack
SX_FORTH_VARIABLE u8    returnSTACK[RETURN_STACK_MAX_ENTRIES];

// common scratch variables for Forth and C
SX_FORTH_VARIABLE s16 s16Tmp0, s16Tmp1, s16Tmp2;
SX_FORTH_VARIABLE u8  u8Tmp0;

//------------------------------------------------------------------------------------------------------------------------------
//                                           Forth Return STACK and STATE
//------------------------------------------------------------------------------------------------------------------------------

// 'forth_StateFlags' flags
#define EXEC_ALL_WORDS                    0x00
#define SKIP_NOT_ALWAYS_EXECUTABLE_WORDS  0x01
#define DO_LOOP_BREAK                     0x02

s8 forth_StateFlags;                    // default EXEC_ALL_WORDS
s8 forth_Branching;
s8 forth_ElseEndifSkiping;

//** builtin interpreter 'variables' *****************************************

// BUILTIN 'numberBase'
const s8 BASE_DEC[] PROGMEM       = "DEC";
void F_DecimalBASE(void)
	{
	numberBase = DECIMAL;
	}

const s8 BASE_HEX[] PROGMEM       = "HEX";  // default
void F_HexadecimalBASE(void)
	{
	numberBase = EXADECIMAL;
	}

// BUILTIN 'debug' and 'step'
const s8 DBG_YES[] PROGMEM        = "DBG+";
void F_DBG_Yes(void)
	{
	debug = TRUE;
	}

const s8 DBG_NO[] PROGMEM         = "DBG-";   // default
void F_DBG_No(void)
	{
	debug = FALSE;
	step  = FALSE;
	}

// BUILTIN interpreter 'step'ing
const s8 STEP_YES[] PROGMEM       = "STEP+";
void F_Step_Yes(void)
	{
	debug = TRUE;
	step  = TRUE;
	}

const s8 STEP_NO[] PROGMEM        = "STEP-";   // default
void F_Step_No(void)
	{
	step = FALSE;
	}


// BUILTIN 'memoryType'
const s8 RAM_[] PROGMEM           = "RAM";    // default
void F_Ram(void)
	{
	memoryType = RAM;
	}

// BUILTIN 'ramUser'
#if RAM_USER_APPLICATION_BYTES
 s8 ramUser[RAM_USER_APPLICATION_BYTES];
 const s8 USER_RAM[] PROGMEM      = "RAM_USER"; // ( -- ramUserADDR )
 void F_UserRamADDR(void)
	{
	memoryType = RAM;
	dataSTACK[dataTOS++] = (s16)(ramUser);
	}
#endif

const s8 EEPROM_[] PROGMEM        = "EEPROM";
void F_Eeprom(void)
	{
	memoryType = EEPROM;
	}

const s8 FLASH_[] PROGMEM        = "FLASH";
void F_Flash(void)
	{
	memoryType = FLASH;
	}


//** builtin interpreter 'immediate words' executed on the fly ***************

// data stack operation
// NOTE: Store (!) and Fetch (@) words are executed based on the current 'memoryType' value (default to RAM)

const s8 STORE[] PROGMEM          = "!";
void F_StoreWord(void)
	{
	// (w addr -- )

	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (memoryType == RAM)
		*((s16 *)(s16Tmp0)) = s16Tmp1;
	else if (memoryType == EEPROM)
		EEPROM_Write(s16Tmp0, (s8 *)(&s16Tmp1), 2);
	else if (memoryType == FLASH)
		Error(WRITE_DISABLED);
	else Error(MEMORY_UNKNOWN);
	}

const s8 STORE_BYTE[] PROGMEM     = "!B";
void F_StoreByte(void)
	{
	// (b addr -- )
	s16Tmp0 = Pop();
	s16Tmp1 = Pop() & 0xFF;
	if (memoryType == RAM)
		*((s8 *)(s16Tmp0)) = (s8)(s16Tmp1);
	else if (memoryType == EEPROM)
		EEPROM_Write(s16Tmp0, (s8 *)(&s16Tmp1), 1);
	else if (memoryType == FLASH)
		Error(WRITE_DISABLED);
	else Error(MEMORY_UNKNOWN);
	}

const s8 FETCH[] PROGMEM          = "@";
void F_FetchWord(void)
	{
	// (addr -- w)
	s16Tmp0 = Pop();
	if (memoryType == RAM)
		Push( *((s16 *)(s16Tmp0)) );
	else if (memoryType == EEPROM)
		{
		EEPROM_Read((s8 *)(&s16Tmp1), s16Tmp0, 2);
		Push(s16Tmp1);
		}
	else if (memoryType == FLASH)
		{
		s16Tmp1 = pgm_read_word((s16 *)(s16Tmp0));
		Push(s16Tmp1);
		}
	else Error(MEMORY_UNKNOWN);
	}

const s8 FETCH_BYTE[] PROGMEM     = "@B";
void F_FetchByte(void)
	{
	// (addr -- b)
	s16Tmp0 = Pop();
	if (memoryType == RAM)
		Push(*((s16 *)(s16Tmp0)) & 0xFF);
	else if (memoryType == EEPROM)
		{
		EEPROM_Read((s8 *)(&s16Tmp1), s16Tmp0, 2);
		Push(s16Tmp1 & 0xFF);
		}
	else if (memoryType == FLASH)
		{
		s16Tmp1 = pgm_read_word((s16 *)(s16Tmp0));
		Push(s16Tmp1 & 0xFF);
		}
	else Error(MEMORY_UNKNOWN);
	}

// dataTOS content display
const s8 DOT_TOS[] PROGMEM          = ".";
void F_Dot_TOS(void)
	{
	// (w -- ) Destructive Print, using BASE, of the contents of dataTOS
	s16Tmp0 = Pop();
	itoa(s16Tmp0, value_String, numberBase);
	USART_PutRamString_UpperCase(value_String);
	USART_Tx(' ');
	}

// dataTOS print as a character
const s8 DOT_TOS_CHAR[] PROGMEM     = ".C";     // like Forth 'emit'
void F_Dot_TOS_Char(void)
	{
	// (b -- ) ASCII Character PrintOUT of dataTOS
	s16Tmp0 = Pop();
	USART_Tx( (s8)(s16Tmp0 & 0xFF) );
	}

// CR print
const s8 CR_[] PROGMEM            = "CR";     // like Forth 'CR'
void F_PutCR(void)
	{
	USART_Tx(CR);
	}

// dataTOS indirect string print, dataTOS dropped
const s8 DOT_TOS_STRING[] PROGMEM     = ".\"";
void F_Dot_String(void)
	{
	// (strAddr -- ) ASCII String PrintOUT with dataTOS used as pointer to RAM
	s16Tmp0 = Pop();
	USART_PutRamString( (s8 *)(s16Tmp0) );
	}

const s8 ASK_DATA_STACK[] PROGMEM         = ".S";
void F_Ask_DataSTACK(void)
	{
	// ( -- ) Non Destructive Print, using BASE, of dataSTACK contents
	// USART_Tx(CR);
	u8Tmp0 = dataTOS;
	while (u8Tmp0)
		{
		itoa(dataSTACK[--u8Tmp0], value_String, numberBase);
		USART_PutRamString_UpperCase(value_String);
		USART_Tx(' ');
		}
	}

// return stack display
const s8 ASK_RETURN_STACK[] PROGMEM         = ".R";
void F_Ask_ReturnSTACK(void)
	{
	// ( -- ) Non Destructive Print of all returnSTACK contents
	// USART_Tx(CR);
	u8Tmp0 = returnTOS;
	while (u8Tmp0)
		{
		USART_PutRamString(stdin_TokenPointersRingBuffer[returnSTACK[--u8Tmp0]]);
		USART_Tx(' ');
		}
	}

// print and clear interpreter stacks
const s8 CLEAR_STACKS[] PROGMEM  = ".CS";
void F_Print_And_ClearSTACKs(void)
	{
	// ( ... -- ) Destructive Display of all STACKs contents
	F_Ask_DataSTACK();
	dataTOS = 0;
	F_Ask_ReturnSTACK();
	returnTOS = 0;
	}

// data stack operators
const s8 DUP_[] PROGMEM           = "DUP";
void F_Dup(void)
	{
	if (dataTOS >= 1)
		{
		s16Tmp0 = dataSTACK[dataTOS - 1];
		dataSTACK[dataTOS++] = s16Tmp0;
		}
	else Error(INSUFFICIENT_PARAMETERS);
	}

const s8 DROP_[] PROGMEM          = "DROP";
void F_Drop(void)
	{
	s16Tmp0 = Pop();
	}

const s8 SWAP_[] PROGMEM          = "SWAP";
void F_Swap(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp0);
	Push(s16Tmp1);
	}


// unary operators ( w -- wRes )
const s8 INC_[] PROGMEM           = "+1";
 void F_Inc(void)
	{
	s16Tmp0 = Pop();
	Push(++s16Tmp0);
	}

const s8 DEC_[] PROGMEM           = "-1";
void F_Dec(void)
	{
	s16Tmp0 = Pop();
	Push(--s16Tmp0);
	}

const s8 NOT_[] PROGMEM           = "NOT";
void F_Not(void)
	{
	s16Tmp0 = Pop();
	Push(s16Tmp0 ^ 0xFFFF);
	}

const s8 ABS_[] PROGMEM           = "ABS";
void F_Abs(void)
	{
	s16Tmp0 = Pop();
	Push(s16Tmp0 & 0x7FFF);
	}

 // binary operators ( w1 w2 -- wRes )
const s8 OR__[] PROGMEM           = "OR";
void F_Or(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 | s16Tmp0);
	}

const s8 XOR_[] PROGMEM           = "XOR";
void F_Xor(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 ^ s16Tmp0);
	}

const s8 AND_[] PROGMEM           = "AND";
void F_And(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 & s16Tmp0);
	}

const s8 ADD_[] PROGMEM           = "+";
void F_Add(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 + s16Tmp0);
	}

const s8 DIFF[] PROGMEM           = "-";
void F_Diff(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 - s16Tmp0);
	}

const s8 MULT[] PROGMEM           = "*";
void F_Mult(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 * s16Tmp0);
	}

const s8 DIV_[] PROGMEM           = "/";
void F_Div(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 / s16Tmp0);
	}

const s8 SCALE_[] PROGMEM        = "*/";  // ( a b c -- r ) r=(a*b)/c is s16, but calculations use s32
void F_Scaling(void)
	{
	u32 tmpA, tmpB, tmpC;

	s16Tmp0 = Pop();  // c
	s16Tmp1 = Pop();  // b
	s16Tmp2 = Pop();  // a
	tmpC = (u32)(s16Tmp0);
	tmpB = (u32)(s16Tmp1);
	tmpA = (u32)(s16Tmp2) & 0xFFFF;
	tmpA = tmpA * tmpB;
	tmpA = tmpA / tmpC;
	tmpA &= 0xFFFF;
	Push((s16)(tmpA));
	}


const s8 MOD_[] PROGMEM           = "MOD";
void F_Mod(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	Push(s16Tmp1 % s16Tmp0);
	}

const s8 LSH_[] PROGMEM           = "LSH";
void F_ShiftLEFT(void)
	{
	s16Tmp0 = Pop() & 0x1F;
	s16Tmp1 = Pop();
	Push(s16Tmp1 <<= s16Tmp0);
	}

const s8 RSH_[] PROGMEM           = "RSH";
void F_ShiftRIGTH(void)
	{
	s16Tmp0 = Pop() & 0x1F;
	s16Tmp1 = Pop();
	Push(s16Tmp1 >>= s16Tmp0);
	}

// comparisons
const s8 EQUAL[] PROGMEM          = "==";
void F_Equal(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 == s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}

const s8 NOT_EQUAL[] PROGMEM      = "<>";
void F_NotEqual(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 != s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}

const s8 GREATHER[] PROGMEM       = ">";
void F_Greather(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 > s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}

const s8 GREATHER_OR_EQUAL[] PROGMEM = ">=";
void F_GreatherOrEqual(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 >= s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}

const s8 LOWER[] PROGMEM          = "<";
void F_Lower(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 < s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}

const s8 LOWER_OR_EQUAL[] PROGMEM = "<=";
void F_LowerOrEqual(void)
	{
	s16Tmp0 = Pop();
	s16Tmp1 = Pop();
	if (s16Tmp1 <= s16Tmp0)
		Push(-1);                         // Forth 'True'
	else
		Push(0);                          // Forth 'False'
	}


//** builtin interpreter I and J variables *********************************

const s8 ADDR_I[] PROGMEM         = "I";  // push I variable address on TOS
void F_Addr_I(void)
	{
	Push((s16)(&i_var));
	}

const s8 STORE_I[] PROGMEM        = "I!";  // store TOS into I variable
void F_Store_I(void)
	{
	i_var = Pop();
	}

const s8 FETCH_I[] PROGMEM        = "I@";  // fetch into TOS the cirrent value of the I variable
void F_Fetch_I(void)
	{
	Push(i_var);
	}

const s8 DISPLAY_I[] PROGMEM      = "I.";  // display the I variable current value
void F_Display_I(void)
	{
	Push(i_var);
	F_Dot_TOS();
	}

const s8 INC_I[] PROGMEM          = "I+";  // increment the I variable
void F_Inc_I(void)
	{
	++i_var;
	}

const s8 DEC_I[] PROGMEM          = "I-";  // decrement the I variable
void F_Dec_I(void)
	{
	--i_var;
	}


const s8 ADDR_J[] PROGMEM         = "J";  // push J variable address on TOS
void F_Addr_J(void)
	{
	Push((s16)(&j_var));
	}

const s8 STORE_J[] PROGMEM        = "J!";  // store TOS into J variable
void F_Store_J(void)
	{
	j_var = Pop();
	}

const s8 FETCH_J[] PROGMEM        = "J@";  // fetch into TOS the current value of the J variable
void F_Fetch_J(void)
	{
	Push(j_var);
	}

const s8 DISPLAY_J[] PROGMEM      = "J.";  // display the J variable current value
void F_Display_J(void)
	{
	Push(j_var);
	F_Dot_TOS();
	}

const s8 INC_J[] PROGMEM          = "J+";  // increment the J variable
void F_Inc_J(void)
	{
	++j_var;
	}

const s8 DEC_J[] PROGMEM          = "J-";  // decrement the J variable
void F_Dec_J(void)
	{
	--j_var;
	}


//** variable I indirect operators ********************************************
const s8 IND_I_FETCH[] PROGMEM   = "@I";  // fetch s16 memory &MEM into TOS using &I variable as 'I INDIRECT FETCH' address
void F_I_IndirectFetch(void)
	{
	Push(i_var);
	F_FetchWord();
	}

const s8 IND_I_FETCH_INC[] PROGMEM   = "@I+";  // ... same as '@I' with two post-increment of I
void F_I_IndirectFetchInc(void)
	{
	Push(i_var);
	F_FetchWord();
	i_var += 2;
	}

const s8 IND_I_FETCH_BYTE[] PROGMEM = "@IB";  // fetch s8 memory &MEM into TOS using &I variable as 'I INDIRECT FETCH' address
void F_I_IndirectFetchByte(void)
	{
	Push(i_var);
	F_FetchByte();
	}

const s8 IND_I_FETCH_BYTE_INC[] PROGMEM = "@IB+";  // ... same as '@IB' with post-increment of I
void F_I_IndirectFetchByteInc(void)
	{
	Push(i_var);
	F_FetchByte();
	++i_var;
	}

const s8 IND_I_STORE[] PROGMEM   = "!I";  // store TOS to s16 memory &MEM using &I variable as 'I INDIRECT STORE' address
void F_I_IndirectStore(void)
	{
	Push(i_var);
	F_StoreWord();
	}

const s8 IND_I_STORE_INC[] PROGMEM       = "!I+";  // same as '!I' but with two I post-increment
void F_I_IndirectStoreInc(void)
	{
	Push(i_var);
	F_StoreWord();
	i_var += 2;
	}

const s8 IND_I_STORE_BYTE[] PROGMEM = "!IB";  // store TOS to s8 memory &MEM using &I variable as 'I INDIRECT STORE' address
void F_I_IndirectStoreByte(void)
	{
	Push(i_var);
	F_StoreByte();
	}

const s8 IND_I_STORE_BYTE_INC[] PROGMEM  = "!IB+";  // same as '!IB' but with I post-increment
void F_I_IndirectStoreByteInc(void)
	{
	Push(i_var);
	F_StoreByte();
	++i_var;
	}


// EXIT if TOS is 0
const s8 ZERO_TOS_EXIT[] PROGMEM                = "==0_EXIT";
void F_Zero_T0S_Exit(void)
	{
	s16Tmp0 = Pop();
	if (s16Tmp0 == 0)
		resCode = EXIT;  // Exit if TOS is zero
	}

// Ask for User Entry
const s8 USER_ENTRY[] PROGMEM    = "?USER";
void F_User_Entry(void)
	{
	u8 mem_Forth_Interpreter_Stdin_Channel = forth_Interpreter_Stdin_Channel;

	USART_PutFlashString(PSTR("\rUser >"));
	forth_Interpreter_Stdin_Channel = TERMINAL_STDIN;
	STDIN_Get_String();
	forth_Interpreter_Stdin_Channel = mem_Forth_Interpreter_Stdin_Channel;
	}


// IF ... [ELSE ...] ENDIF construct
/*
TOS is used for set the behaviour of the IF ... [ELSE ...] ENDIF construct.

When word IF happen :
	if TOS is TRUE
		flag 'forth_ElseEndifSkiping' is set to TRUE,
		so next words inside IF-ELSE(or IF-ENDIF) are executed while the ELSE or ENDIF words happen.
	if TOS is FALSE
		flag 'forth_ElseEndifSkiping' is set to FALSE and 'forth_StateFlags' is set to SKIP_NOT_ALWAYS_EXECUTABLE_WORDS,
		so next words inside IF-ELSE(or IF-ENDIF) are silently discarted while the ELSE or ENDIF words happen.
	drop TOS

When ELSE word happen :
	if flag 'forth_ElseEndifSkiping' is set to TRUE,
		the flag 'forth_StateFlags' is set to SKIP_NOT_ALWAYS_EXECUTABLE_WORDS,
		so next words inside ELSE-ENDIF are silently discarted while the ENDIF word happen.
	if flag 'forth_ElseEndifSkiping' is set to FALSE,
		the flag 'forth_StateFlags' is set to EXEC_ALL_WORDS,
		so next words inside ELSE-ENDIF are executed.

When ENDIF word happen :
	the flag 'forth_StateFlags' is inconditionally set to EXEC_ALL_WORDS,
	so execution continue from word that follow ENDIF.

Examples:
	"5 1 > IF ..x.. ELSE ..y.. ENDIF"
	Execute the '..x..' words inside IF-ELSE and silently discard the '..y..' words inside ELSE-ENDIF.
*/
const s8 IF[] PROGMEM                = "IF";
void F_If(void)
	{
	s16Tmp0 = Pop();
	if (s16Tmp0 != 0)
		forth_ElseEndifSkiping = TRUE;
	else
		{
		forth_ElseEndifSkiping = FALSE;
		forth_StateFlags = SKIP_NOT_ALWAYS_EXECUTABLE_WORDS;
		}
	}

const s8 ELSE[] PROGMEM              = "ELSE";
void F_Else(void)
	{
	if (forth_ElseEndifSkiping == TRUE)
		forth_StateFlags = SKIP_NOT_ALWAYS_EXECUTABLE_WORDS;
	else
		forth_StateFlags = EXEC_ALL_WORDS;
	}

const s8 ENDIF[] PROGMEM             = "ENDIF";
void F_Endif(void)
	{
	forth_StateFlags = EXEC_ALL_WORDS;
	}

// DO ... LOOP construct
/*
TOS is used for set how many iterations are required on a DO ... LOOP construct.
DO ... LOOP iterations can be aborted using an inner test '?xxxx' (like '?0_EXIT') exit word.

When word DO happen :
 the address of current word DO is pushed on the 'returnSTACK',
 if TOS is lower then or equal to 0 ... next words are silently discarted while the LOOP word happen.
 else ... next words are executed while the LOOP word happen.

When LOOP word happen :
 the address of word DO, previously pushed on the 'returnSTACK', is popped and TOS is decremented by 1,
 if TOS is lower then or equal to 0 or DO_LOOP_BREAK is true ...
	 DO ... LOOP loop is exited continuing into next parser word.
 else ... execute the DO word with the just popped before from the 'returnSTACK'.

Examples:
	"5 DO ... LOOP"   execute five time the '...' inner loop words.
*/
const s8 DO[] PROGMEM                = "DO";
void F_Do(void)
	{
	if (dataTOS >= 1)
		{
		returnSTACK[returnTOS++] = forth_CurrentIndex;
		if (dataSTACK[dataTOS - 1] <= 0)
			forth_StateFlags = SKIP_NOT_ALWAYS_EXECUTABLE_WORDS;
		else
			forth_StateFlags = EXEC_ALL_WORDS;
		}
	else Error(INSUFFICIENT_PARAMETERS);
	}

const s8 LOOP[] PROGMEM              = "LOOP";
void F_Loop(void)
	{
	if ((dataTOS >= 1) && (returnTOS >= 1))
		{
		u8Tmp0 = returnSTACK[--returnTOS];
		if ((--dataSTACK[dataTOS - 1] <= 0) || (forth_StateFlags & DO_LOOP_BREAK))
			{
			--dataTOS;                    // ... drop TOS and exit the DO...LOOP
			forth_StateFlags = EXEC_ALL_WORDS;
			}
		else
			{
			forth_Branching = TRUE;       // ... then skip next 'forth_CurrentIndex' increment
			forth_CurrentIndex = u8Tmp0;  // ... and continue the DO...LOOP
			}
		}
	else Error(INSUFFICIENT_PARAMETERS);
	}

// BREAK a DO ... LOOP if TOS is 0
const s8 ZERO_TOS_BREAK[] PROGMEM   = "==0_BREAK";
void F_Zero_T0S_Break(void)
	{
	s16Tmp0 = Pop();
	if (s16Tmp0 == 0)
		forth_StateFlags = SKIP_NOT_ALWAYS_EXECUTABLE_WORDS + DO_LOOP_BREAK;
	}

const s8 WORDS[] PROGMEM             = "WORDS";
void F_Words_Display(void);

const s8 PAUSE[] PROGMEM             = "PAUSE";
void F_Pause(void);

const s8 CHECK_PAUSE[] PROGMEM       = "?PAUSE";
void F_CheckPause(void);

//------------------------------------------------------------------------------
//    C Functions CALL from Forth and C_Functions_DICTIONARY
//------------------------------------------------------------------------------

// From Forth we can call a C function 'by name' using the following command format :
//
//       **************************************************************
//       ** ...in_param_push... "C_funcName CALL ...out_param_pop... **
//       **************************************************************
//
// ... input parameters, if required, can be pushed on dataSTACK before CALL,
// ... then input the C function name using "C_funcName followed by word CALL,
// ... output parameters, if any, can be popped from the dataSTACK after the CALL
//
// ... "C_funcName string is CASE SENSITIVE like all strings in Forth.


// ... set to TRUE only for 'CALL' from Forth Tests,
// ... can be used as TEMPLATE for write C function callable by Forth
#define  C_CALL_TEMPLATE   TRUE

#if C_CALL_TEMPLATE
 const s8 _C_FUNCTION_0[] PROGMEM          = "C_Func_0";
 void C_Func_0(void)
	{
	// check 'resCode' for EXIT, or 'err' for any ERRORs
	USART_PutFlashString(PSTR("\7C_Func_0() reached !"));
	}

 const s8 _C_FUNCTION_1[] PROGMEM          = "C_Func_1";
 void C_Func_1(void)
	{
	// check 'resCode' for EXIT, or 'err' for any ERRORs
	USART_PutFlashString(PSTR("\7C_Func_1() reached !"));
	}

#endif

typedef struct PROGMEM
	{
	PGM_P       c_STR;             // dictionary C_funcName string
	C_FUNC      c_C_Func;          // dictionary address of called Function
	} BUILTIN_C_DICTIONARY;

const BUILTIN_C_DICTIONARY c_DICT[] PROGMEM = {
	// C functions DICTIONARY --------------------------
  #if C_CALL_TEMPLATE
	{_C_FUNCTION_0,               C_Func_0},
	{_C_FUNCTION_1,               C_Func_1},
  #endif
	// End of Builtin DICTIONARY -----------------------
	{NULL,                        NULL}
	};

const s8 C_FUNCTION_CALL_BY_NAME[] PROGMEM  = "CALL";
void F_C_FunctionCALL_by_Name(void)
	{
	u8          c_dictionary_index = 0;
	PGM_P       c_STR;             // dictionary C_funcName string
	C_FUNC      c_C_Func;          // dictionary address of called Function

	// Search if c_STR it's defined, CASE SENSITIVE, into the C BUILTIN DICTIONARY ...

	s16Tmp0 = Pop();                     // pop the C_funcName string pointer from TOS
	c_dictionary_index = 0;

	do {
		c_STR = (PGM_P)(pgm_read_word(&c_DICT[c_dictionary_index].c_STR));
		if (c_STR)
			{
			if (strcmp_P((s8 *)(s16Tmp0), c_STR) == 0)
				{
				c_C_Func = (C_FUNC)(pgm_read_word(&c_DICT[c_dictionary_index].c_C_Func));
				if (c_C_Func != NULL)
					c_C_Func();    // CALL the required C Function
				break;
				}
			}
		else
			{
			Error(FUNCTION_UNKNOWN);
			break;
			}
		++c_dictionary_index;
		} while (1);
	}


//------------------------------------------------------------------------------
//                   EMBEDDED Forth Words SOURCE Scripts
//------------------------------------------------------------------------------

const s8 MEM_DUMP_[] PROGMEM         = "DUMP";  // (len addr -- ) print memory
const s8 MEM_DUMP[] PROGMEM          = "I! DO I. @IB+ . CR LOOP";

const s8 MEM_STR_DUMP_[] PROGMEM     = "DUMP\"";  // (addr -- ) print string using TOS as the memory pointer
const s8 MEM_STR_DUMP[] PROGMEM      = "I! 100 DO @IB+ DUP ==0_EXIT .C LOOP";

const s8 MEM_FILL_[] PROGMEM         = "FILL";  // (len addr val -- ) fill memory
const s8 MEM_FILL[] PROGMEM          = "J! I! DO J@ !IB+ LOOP";



//------------------------------------------------------------------------------------------------------------------------------
//                                     BuiltIN Forth DICTIONARY
//------------------------------------------------------------------------------------------------------------------------------

#define EXEC_ALWAYS_MASK        0x01
#define EMBEDDED_SOURCE_MASK    0x02

typedef struct PROGMEM
	{
	PGM_P       forth_STR;             // dictionary Command String
	C_FUNC      forth_C_Func;          // dictionary called C Command Function
	u8          forth_ExcecutionFlag;  // if set to EXEC_ALWAYS_MASK unconditionally executed this word,
												  // else the word is executed only if 'forth_StateFlags == EXEC_ALL_TOKENS'
	} BUILTIN_FORTH_DICTIONARY;

const BUILTIN_FORTH_DICTIONARY forth_DICT[] PROGMEM = {

	//** C Functions CALL from Forth using C_Functions_DICTIONARY ************
	{C_FUNCTION_CALL_BY_NAME,  F_C_FunctionCALL_by_Name,  0},

	//** builtin interpreter 'immediate words' executed on the fly ***********
	// dataTOS operation
	{STORE,                    F_StoreWord,               0},
	{STORE_BYTE,               F_StoreByte,               0},
	{FETCH,                    F_FetchWord,               0},
	{FETCH_BYTE,               F_FetchByte,               0},

	// destructive stack(s) print
	{DOT_TOS,                  F_Dot_TOS,                 0},
	{DOT_TOS_CHAR,             F_Dot_TOS_Char,            0},
	{DOT_TOS_STRING,           F_Dot_String,              0},
	{CLEAR_STACKS,             F_Print_And_ClearSTACKs,   EXEC_ALWAYS_MASK},

	// stack(s) display
	{ASK_DATA_STACK,           F_Ask_DataSTACK,           EXEC_ALWAYS_MASK},
	{ASK_RETURN_STACK,         F_Ask_ReturnSTACK,         EXEC_ALWAYS_MASK},

	// CR printing
	{CR_,                      F_PutCR,                   0},

	// stack oparators
	{DUP_,                     F_Dup,                     0},
	{SWAP_,                    F_Swap,                    0},
	{DROP_,                    F_Drop,                    0},

	// unary operators ( w -- wRes )
	{INC_,                     F_Inc,                     0},
	{DEC_,                     F_Dec,                     0},
	{NOT_,                     F_Not,                     0},
	{ABS_,                     F_Abs,                     0},
	// binary operators ( w1 w2 -- wRes )
	{OR__,                     F_Or,                      0},
	{XOR_,                     F_Xor,                     0},
	{AND_,                     F_And,                     0},
	{ADD_,                     F_Add,                     0},
	{DIFF,                     F_Diff,                    0},
	{MULT,                     F_Mult,                    0},
	{DIV_,                     F_Div,                     0},
	{SCALE_,                   F_Scaling,                 0},
	{MOD_,                     F_Mod,                     0},
	{LSH_,                     F_ShiftLEFT,               0},
	{RSH_,                     F_ShiftRIGTH,              0},
	// tests comparison ( w1 w2 -- Res )
	{EQUAL,                    F_Equal,                   0},
	{NOT_EQUAL,                F_NotEqual,                0},
	{GREATHER,                 F_Greather,                0},
	{GREATHER_OR_EQUAL,        F_GreatherOrEqual,         0},
	{LOWER,                    F_Lower,                   0},
	{LOWER_OR_EQUAL,           F_LowerOrEqual,            0},

	//** builtin interpreter I and J variables *********************************
	{ADDR_I,                   F_Addr_I,                  0},
	{STORE_I,                  F_Store_I,                 0},
	{FETCH_I,                  F_Fetch_I,                 0},
	{DISPLAY_I,                F_Display_I,               0},
	{INC_I,                    F_Inc_I,                   0},
	{DEC_I,                    F_Dec_I,                   0},

	{ADDR_J,                   F_Addr_J,                  0},
	{STORE_J,                  F_Store_J,                 0},
	{FETCH_J,                  F_Fetch_J,                 0},
	{DISPLAY_J,                F_Display_J,               0},
	{INC_J,                    F_Inc_J,                   0},
	{DEC_J,                    F_Dec_J,                   0},

  //** builtin interpreter I indirect operators ********************************
	{IND_I_FETCH,              F_I_IndirectFetch,         0},
	{IND_I_FETCH_BYTE,         F_I_IndirectFetchByte,     0},
	{IND_I_STORE,              F_I_IndirectStore,         0},
	{IND_I_STORE_BYTE,         F_I_IndirectStoreByte,     0},
	{IND_I_FETCH_INC,          F_I_IndirectFetchInc,      0},
	{IND_I_FETCH_BYTE_INC,     F_I_IndirectFetchByteInc,  0},
	{IND_I_STORE_INC,          F_I_IndirectStoreInc,      0},
	{IND_I_STORE_BYTE_INC,     F_I_IndirectStoreByteInc,  0},

	// EXIT if TOS is 0
	{ZERO_TOS_EXIT,            F_Zero_T0S_Exit,           0},
	// User Entry, acquire a user string CR or ESC terminated
	{USER_ENTRY,               F_User_Entry,              0},

	// IF ... [ELSE ...] ENDIF construct
	{IF,                       F_If,                      EXEC_ALWAYS_MASK},
	{ELSE,                     F_Else,                    EXEC_ALWAYS_MASK},
	{ENDIF,                    F_Endif,                   EXEC_ALWAYS_MASK},

	// DO ... LOOP construct
	{DO,                       F_Do,                      EXEC_ALWAYS_MASK},
	{LOOP,                     F_Loop,                    EXEC_ALWAYS_MASK},
	// BREAK the DO ... LOOP if TOS is 0
	{ZERO_TOS_BREAK,           F_Zero_T0S_Break,          0},

	//** builtin interpreter variables ***************************************
	{BASE_DEC,                 F_DecimalBASE,             0},
	{BASE_HEX,                 F_HexadecimalBASE,         0},
	{DBG_YES,                  F_DBG_Yes,                 0},
	{DBG_NO,                   F_DBG_No,                  0},
	{RAM_,                     F_Ram,                     0},
	{EEPROM_,                  F_Eeprom,                  0},
	{FLASH_,                   F_Flash,                   0},
	//** USER RAM area *******************************************************
  #if RAM_USER_APPLICATION_BYTES
	{USER_RAM,                  F_UserRamADDR,            0},
  #endif

	//** PAUSE words
	{PAUSE,                    F_Pause,                   0},
	{CHECK_PAUSE,              F_CheckPause,              EXEC_ALWAYS_MASK},

	//** STEP words
	{STEP_YES,                 F_Step_Yes,                0},
	{STEP_NO,                  F_Step_No,                 0},

	//** Words DICTIONARY Display
	{WORDS,                    F_Words_Display,           0},

	//** EMBEDDED Forth Words SOURCE Scripts ************************************
	{MEM_DUMP_,                (C_FUNC)(MEM_DUMP),       EMBEDDED_SOURCE_MASK},
	{MEM_STR_DUMP_,            (C_FUNC)(MEM_STR_DUMP),   EMBEDDED_SOURCE_MASK},
	{MEM_FILL_,                (C_FUNC)(MEM_FILL),       EMBEDDED_SOURCE_MASK},

	// End of Builtin DICTIONARY ------------------------------------------------
	{NULL,                     NULL,                      0}
	};

void F_Words_Display(void)
	{
	// Display all DICTIONARY words
	PGM_P forth_STR; // pointer to current dictionary 'forth_STR'

	s16Tmp0 = 0;
	do {
		forth_STR = (PGM_P)(pgm_read_word(&forth_DICT[s16Tmp0].forth_STR));
		if (forth_STR)
			{
			USART_PutFlashString(forth_STR);
			USART_Tx(CR);
			}
		else
			break;
		++s16Tmp0;
		} while (1);
	}

void F_Pause(void)
	{
	USART_PutFlashString(PSTR(" PAUSE: [E] Exit, user -> "));
	while (1)
		{
		s16Tmp0 = USART_Rx_Check();
		if (s16Tmp0 >= 0)
			{
			if (toupper(s16Tmp0) == 'E')
				{
				USART_PutFlashString(PSTR("E\r"));
				resCode = EXIT;
				}
			else
				USART_PutFlashString(PSTR(".\r"));
			break;
			}
		}
	}

void F_CheckPause(void)
	{
	s16Tmp0 = USART_Rx_Check();
	if (s16Tmp0 >= 0)
		{
		F_Pause();
		}
	}


//------------------------------------------------------------------------------------------------------------------------------
//                                              Forth INTERPRETER
//------------------------------------------------------------------------------------------------------------------------------

// Both the STDIN Channel and the STDOUT Channel of the Forth INTERPRETER use ASCII strings terminated with CR.
void Inc_Stdin_StringPushIndex(void)
	{
	if (++stdin_StringPushIndex >= MAX_STDIN_STRING_LENGTH)
		stdin_StringPushIndex = 0;
	if (stdin_StringPushIndex == stdin_StringPopIndex)
		Error(STDIN_RING_BUFFER_FULL);
	}

void Dec_Stdin_StringPushIndex(void)
	{
	if (stdin_StringPushIndex == 0)
		stdin_StringPushIndex = MAX_STDIN_STRING_LENGTH - 1;
	else
		--stdin_StringPushIndex;
	}

void Inc_Stdin_StringPopIndex(void)
	{
	if (++stdin_StringPopIndex >= MAX_STDIN_STRING_LENGTH)
		stdin_StringPopIndex = 0;
	}

/* --- 'STDIN_Get_String()' ---
 *
 * The STDIN strings are acquired, one at a time, using the 'STDIN_Get_String()' function.
 * The 'STDIN_Get_String()' function use the 'forth_Interpreter_StdinChannel' variable and
 * the 'forth_Interpreter_StdinMemoryAddress' address to get a string fron the current STDIN Channel.
 * The 'STDIN_Get_String()' function use a RAM ring buffer 'stdin_StringRingBuffer' to store the input stream.
 */
void STDIN_Get_String(void)
	{
	s16   rxChar;
	s8    txChar;
	s8    *strMemAddr = forth_Interpreter_Stdin_MemoryAddress;;

	resCode = READY;
	switch (forth_Interpreter_Stdin_Channel)
		{
		default :
		case TERMINAL_STDIN :
			// get ready characters from TERMINAL_STDIN into RAM 'stdin_StringRingBuffer', one at a time
			resCode = CONTINUE;
			rxChar = USART_Rx_Check();
			if (rxChar >= 0)
				{
				txChar = (s8)(rxChar & 0xFF);
				if (txChar == ESC)
					{
					USART_Rx_Flush();
					stdin_StringPushIndex = stdin_StringPopIndex;
					USART_PutFlashString(PSTR("\r>"));
					txChar = BEL;
					}
				else if (txChar == BS)
					{
					if (stdin_StringPushIndex != stdin_StringPopIndex)
						{
						Dec_Stdin_StringPushIndex();
						stdin_StringRingBuffer[stdin_StringPushIndex] = '\0';
						USART_PutFlashString(PSTR("\b "));
						}
					else
						txChar = BEL;
					}
				else if (txChar == CR)
					{
					if (stdin_StringPushIndex != stdin_StringPopIndex)
						resCode = READY;
					else
						txChar = BEL;
					}
				else if (txChar >= ' ')
					{
					stdin_StringRingBuffer[stdin_StringPushIndex] = txChar;
					Inc_Stdin_StringPushIndex();
					}
				else
					txChar = BEL;
				USART_Tx(txChar);
				}
			break;

		case RAM_STDIN :
			do { // copy string from RAM 'forth_Interpreter_Stdin_MemoryAddress' into RAM 'stdin_StringRingBuffer'
				txChar = *strMemAddr;
				if (txChar <= 0)
					break;
				++strMemAddr;
				stdin_StringRingBuffer[stdin_StringPushIndex] = txChar;
				Inc_Stdin_StringPushIndex();
				} while (1);
			break;

		case EEPROM_STDIN :
			do { // copy string from EEPROM 'forth_Interpreter_Stdin_MemoryAddress' into RAM 'stdin_StringRingBuffer'
				EEPROM_Read(&txChar, (s16)(strMemAddr++), 1);
				if (txChar <= 0)
					break;
				stdin_StringRingBuffer[stdin_StringPushIndex] = txChar;
				Inc_Stdin_StringPushIndex();
				} while (1);
			break;

		case FLASH_STDIN :
			do { // copy string from FLASH 'forth_Interpreter_Stdin_MemoryAddress' into RAM 'stdin_StringRingBuffer'
				txChar = pgm_read_byte(strMemAddr++);
				if (txChar <= 0)
					break;
				stdin_StringRingBuffer[stdin_StringPushIndex] = txChar;
				Inc_Stdin_StringPushIndex();
				} while (1);
			break;
		}

	if (resCode == READY)
		{ // Terminate string
		stdin_StringRingBuffer[stdin_StringPushIndex] = '\0';
		Inc_Stdin_StringPushIndex();
		}
	}

/* --- 'STDIN_Parse_String()' ---
 *
 * When an STDIN string is acquired by 'STDIN_Get_String()' into the RAM ring buffer 'stdin_StringRingBuffer'
 * the string is PARSED into Token(s) and Token(s) pointers are stored in another RAM ring buffer named
 * 'stdin_TokenPointersRingBuffer' where the Forth INTERPRETER interpret in place the STDIN input Token(s) stream.
 */
void STDIN_Parse_String(void)
	{
	s8    c, inToken = 0;
	// Parser operate always on RAM string in the 'stdin_StringRingBuffer'
	while (stdin_StringPopIndex != stdin_StringPushIndex)
		{
		c = stdin_StringRingBuffer[stdin_StringPopIndex];
		if (c <= 0)
			break;
		if (inToken == 0)
			{
			if (c <= ' ')
				stdin_StringRingBuffer[stdin_StringPopIndex] = '\0';
			else
				{
				++inToken;
				if (parser_NextIndex >= MAX_TOKEN_POINTERS_IN_THE_PARSER_RING_BUFFER)
					parser_NextIndex = 0;
				stdin_TokenPointersRingBuffer[parser_NextIndex++] = &stdin_StringRingBuffer[stdin_StringPopIndex];
				}
			}
		else
			{
			if (c <= ' ')
				{
				inToken = 0;
				stdin_StringRingBuffer[stdin_StringPopIndex] = '\0';
				}
			}
		Inc_Stdin_StringPopIndex();
		}
	stdin_StringRingBuffer[stdin_StringPopIndex] = '\0';
	Inc_Stdin_StringPopIndex();
	resCode = READY;
	}

/* --- 'Forth_Interpreter()' ---
 *
 * When a call to 'STDIN_Get_String()' return a ready string in the 'stdin_StringRingBuffer' from the STDIN current
 * channel, the Forth INTERPRETER call the 'STDIN_Parse_String()' to parse Token(s) on 'stdin_TokenPointersRingBuffer'
 * where the Forth INTERPRETER interpret the STDIN input Token(s) stream.
 * Any Token can be a NUMBER, a STRING pointer or a Forth WORD that Forth INTERPRETER process ... then it call
 * 'STDIN_Get_String()' for a next STDIN string ... call 'STDIN_Parse_String()' when a string is ready, interpret and
 * execute the new Token(s) ... and so on.
 */

// 'forth_StateFlags' flags
#define EXEC_ALL_WORDS                    0x00
#define SKIP_NOT_ALWAYS_EXECUTABLE_WORDS  0x01
#define DO_LOOP_BREAK                     0x02

s8 forth_StateFlags;                    // default EXEC_ALL_WORDS
s8 forth_Branching;
s8 forth_ElseEndifSkiping;

void Forth_Interpreter(void)
	{
	u8          forth_Dictionary_Index;    // current builtin Dictionary index
	C_FUNC      forth_C_Func;              // dictionary called C Command Function
	PGM_P       forth_STR;                 // pointer to current dictionary 'forth_STR'
	u8          forth_ExcecutionFlag;      // if set to EXEC_ALWAYS_MASK unconditionally executed this word,
														// else the word is executed only if 'forth_StateFlags == EXEC_ALL_TOKENS'
	s8          *current_Token;            // current pointer into Token(s) Ring Buffer


	// Get next string from current 'forth_Interpreter_Stdin_Channel'
	STDIN_Get_String();
	if (resCode == READY)
		{
		// Parse the new string into 'stdin_TokenPointersRingBuffer'
		STDIN_Parse_String();
		if (resCode == READY)
			{
			// Interpret all incoming Token(s)
			while (resCode == READY)
				{
				current_Token = stdin_TokenPointersRingBuffer[forth_CurrentIndex];
				if (forth_CurrentIndex == parser_NextIndex)
					{
					resCode = CONTINUE;
					USART_PutFlashString(PSTR("\r\r>"));
					break;
					}

				if (debug)
					{
					// Display the current Token (can be a Word, a Number or a String)
					USART_PutFlashString(PSTR("\r["));
					USART_PutRamString(stdin_TokenPointersRingBuffer[forth_CurrentIndex]);
					USART_PutFlashString(PSTR("] "));
					}

				if (isdigit(current_Token[0]))
					{
					// Token NUMBER start with a digit character ...
					// ... then convert to 's16' number based on current number BASE ...
					// ... and push it on the Interpreter DATA STACK
					if (forth_StateFlags == EXEC_ALL_WORDS)
						{
						if (dataTOS < (DATA_STACK_MAX_ENTRIES-1))
							{
							if (numberBase == 10)
								dataSTACK[dataTOS++] = atoi (current_Token);
							else
								dataSTACK[dataTOS++] = xtoi (current_Token);
							}
						else
							Error(STACK_FULL);
						}
					}
				else if (current_Token[0] == '\\')
					{
					// Token EXADECIMAL CHARACTER CODE start with a '\' character ...
					// ... push it's 'hexadecimal escape code' on the Interpreter DATA STACK independently of current BASE for numbers
					if (forth_StateFlags == EXEC_ALL_WORDS)
						{
						if (dataTOS < (DATA_STACK_MAX_ENTRIES-1))
							dataSTACK[dataTOS++] = xtoi (current_Token + 1);
						else
							Error(STACK_FULL);
						}
					}
				else if (current_Token[0] == '"')
					{
					// Token STRING start with a " character ...
					// ... push it's address on the Interpreter DATA STACK
					if (forth_StateFlags == EXEC_ALL_WORDS)
						{
						if (dataTOS < (DATA_STACK_MAX_ENTRIES-1))
							dataSTACK[dataTOS++] = (s16)(current_Token + 1);
						else
							Error(STACK_FULL);
						}
					}
				else
					{
					// if not NUMBER or STRING ...
					// ... then search if it's defined, CASE INSENSITIVE, into the Forth BUILTIN DICTIONARY ...
					forth_Dictionary_Index = 0;
					do {
						forth_STR = (PGM_P)(pgm_read_word(&forth_DICT[forth_Dictionary_Index].forth_STR));
						if (forth_STR)
							{
							if (strcasecmp_P(stdin_TokenPointersRingBuffer[forth_CurrentIndex], forth_STR) == 0)
								{
								forth_C_Func = (C_FUNC)(pgm_read_word(&forth_DICT[forth_Dictionary_Index].forth_C_Func));
								forth_ExcecutionFlag = pgm_read_byte(&forth_DICT[forth_Dictionary_Index].forth_ExcecutionFlag);
								if (forth_StateFlags == EXEC_ALL_WORDS)
									{
									if (forth_ExcecutionFlag & EMBEDDED_SOURCE_MASK)
										{ // parse the EMBEDDED Forth SOURCE string in FLASH and Continue
										forth_Interpreter_Stdin_Channel = FLASH_STDIN;
										forth_Interpreter_Stdin_MemoryAddress = (s8 *)(forth_C_Func);
										STDIN_Get_String();
										forth_Interpreter_Stdin_Channel = TERMINAL_STDIN;
										forth_Interpreter_Stdin_MemoryAddress = NULL;
										STDIN_Parse_String();
										}
									else
										forth_C_Func();        // exec current word
									}
								else
									{
									if (forth_ExcecutionFlag & EXEC_ALWAYS_MASK)
										forth_C_Func();        // exec current word
									}
								break;
								}
							}
						else
							{
							Error(WORD_UNKNOWN);
							break;
							}
						++forth_Dictionary_Index;
						} while (1);
					}

				if (forth_Branching == TRUE)
					// ... then skip next 'forth_CurrentIndex' increment
					forth_Branching = FALSE;
				else
					++forth_CurrentIndex;

				// Do a user STEP or PAUSE/EXIT behaviour
				if (step)
					F_Pause();
				else
					F_CheckPause();
				}
			}
		}
	if (resCode == EXIT)
		{
		// Restart the Forth INTERPRETER
		forth_StateFlags = EXEC_ALL_WORDS;
		dataTOS = 0;
		returnTOS = 0;

		// Set the STDIN current channel in use
		forth_Interpreter_Stdin_Channel = TERMINAL_STDIN;
		forth_Interpreter_Stdin_MemoryAddress = NULL;

		// Clear system RING buffers and variables
		memset(stdin_TokenPointersRingBuffer, 0, sizeof(stdin_TokenPointersRingBuffer));
		memset(stdin_StringRingBuffer, 0, sizeof(stdin_StringRingBuffer));
		parser_NextIndex = forth_CurrentIndex = 0;
		stdin_StringPushIndex = stdin_StringPopIndex = 0;
		USART_PutFlashString(PSTR("\r\r>"));
		}
	}

//=========================================================================================================================
#endif // STDIO_USED



//==============================================================================================================================
//                         APPLICATION is Driven by RTC Timer and Peripherals Interrupts
//==============================================================================================================================

// SX Timer Interrupt Tick occurs any MSEC_SYSTEM_TICK (running from the RTC 32.768kHz crystal)
ISR(TIMER2_OVF_vect)
	{
	//--- Do Timing Services based on MSEC_SYSTEM_TICK ---
	#if STDIO_USED
	 Forth_Interpreter();
	#endif
	}

s16 main( void )
	{
	// calibrate the OSCCAL byte
	OSCCAL_calibration();
	// set Clock Prescaler Change Enable
	CLKPR = (1<<CLKPCE);
	// set prescaler = 8, Internal RC 8MHz / 8 = 1MHz
	CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

	// SX Timer Interrupt Tick occurs any MSEC_SYSTEM_TICK, running from the 32.768kHz crystal on Asyncronous Timer 2
	RTC_init();

	#if STDIO_USED
	 // UBRR = Fosc / (8 * baud) - 1 ... for U2XN=1, page 172 and 195 of ATmega169V(P)
	 USART_Init(12);  // UBRR for 'baudrate' 9600bps N81 at 1MHZ
	 USART_PutFlashString(PSTR("\rAVR SX-Forth 20060713 [V.Foresto - Comm.SEC]\r\r>"));
	 numberBase = EXADECIMAL;
	#endif

	sei();

	while (1)
		{
		sleep_mode();
		}

	return (0);
	}
