/*
								********************
								**** SX-Forth.H ****
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

#ifdef   SX_FORTH_MODULE
	#define  SX_FORTH_VARIABLE   //
#else
	#define  SX_FORTH_VARIABLE   extern
#endif

//==============================================================================================================================

// Internal OSCCAL calibration at 1MHz
void OSCCAL_calibration(void);

// SX Timer Interrupt Tick occurs any MSEC_SYSTEM_TICK, running from the 32.768kHz crystal on Asyncronous Timer 2
void RTC_init(void);

// Assume 1 MHz CPU Core Clock for DelayMSEC() function in [msec]
void DelayMSEC(u16 millisec);

// Internal EEPROM Access Functions (dest, source, length format)
void EEPROM_Read(s8 *ramDest, s16 eepromAddr, s16 readLen);
void EEPROM_Write(s16 eepromAddr, s8 *ramSource, s16 writeLen);

#if STDIO_USED
	// USART functions (polling)
	void USART_Init(u16 ubrrBaudrate);      // 9600 N81 max with 1 MHz clock
	void USART_Tx(s8 txChar);               // return after 'txChar' transmission
	s16  USART_Rx_Check(void);              // return -1 if not 'rxChar' ready, else 0x0000 ... 0x00FF 'rxChar'
	void USART_Rx_Flush(void);              // flush all received characters (10 msec delay)

	void USART_PutRamString(s8 *s);           // put string from RAM
	void USART_PutRamString_UpperCase(s8 *s); // put string from RAM, convert to upper case alphabetic characters
	void USART_PutFlashString(const s8 *s);   // put string from FLASH

	#if 0
		void USART_PutRamBuffer(s8 *b, u8 len);   // put buffer RAM for a given length
	#endif
	s16 xtoi (s8 *s);

	void STDIN_Parse_String(void);
	void STDIN_Get_String(void);
	void Forth_Interpreter(void);

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

	/*
		--- Values to ASCII String ----
		char *itoa (int val, char *s, int radix);
		char *ltoa (long int val, char *s, int radix);
		char *utoa (unsigned int val, char *s, int radix);
		char *ultoa (unsigned long int val, char *s, int radix);
		... all functions return the supplied '*s' pointer at call.
	*/
	SX_FORTH_VARIABLE s8  value_String[1 + 10 + 1]; // sign + asciiValue + '\0', use only 'radix' 10 and 16

	#define  MAX_STDIN_STRING_LENGTH    (3+128+2)   // like X-Modem 128/CRC block size max
	SX_FORTH_VARIABLE s8 stdin_StringRingBuffer[MAX_STDIN_STRING_LENGTH + 1];
	SX_FORTH_VARIABLE u8 stdin_StringPushIndex;
	SX_FORTH_VARIABLE u8 stdin_StringPopIndex;

	#define  MAX_TOKEN_POINTERS_IN_THE_PARSER_RING_BUFFER      64
	SX_FORTH_VARIABLE u8 parser_NextIndex;
	SX_FORTH_VARIABLE u8 forth_CurrentIndex;
	SX_FORTH_VARIABLE s8 *stdin_TokenPointersRingBuffer[MAX_TOKEN_POINTERS_IN_THE_PARSER_RING_BUFFER];

#endif // STDIO_USED

/*
 * All C and Forth functions / words are 'void Func (void)', input and outputs parameters are passed on the Data STACK.
 * C and Forth functions / words functions can access input and outputs parameters using unified Pop() and Push() primitive.
 * Using this approach :
 *       C can call Forth,
 *       Forth can call C,
 *       Forth can call Forth string scripts,
 *       and obviously C can call C
 * ... and C functions can have many outputs parameters instead of only one.
 *
 */

// C and Forth Data STACK (Last IN, First OUT behaviour)
#define DATA_STACK_MAX_ENTRIES      16
SX_FORTH_VARIABLE u8    dataTOS;    // Top Of C and Forth DATA Stack
SX_FORTH_VARIABLE s16   dataSTACK[DATA_STACK_MAX_ENTRIES];

// standardized access to 'Data STACK' from Forth or C function
void Push(s16 value);
s16  Pop(void);

typedef void (*C_FUNC)(void);   // C Functions prototype interoperative with Forth
typedef void (*F_FUNC)(void);   // Forth written in C language Functions

//------------------------------------------------------------------------------------------------------------------------------
//                                        ERROR CODEs and Trap Function
//------------------------------------------------------------------------------------------------------------------------------

//** RESULT CODE for C and Forth functions / words

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

// 'errCode' defines the index in the user terminal messages table used if STDIO_USED is set to TRUE
#define  STACK_FULL                          0
#define  WORD_UNKNOWN                        1
#define  FUNCTION_UNKNOWN                    2
#define  MEMORY_UNKNOWN                      3
#define  INSUFFICIENT_PARAMETERS             4
#define  WRITE_DISABLED                      5
#define  STDIN_RING_BUFFER_FULL              6
#define  PARSER_RING_BUFFER_FULL             7

// readables from the Forth terminal as '.ERR' and '.ERRCNT' ... cleared when readed
SX_FORTH_VARIABLE u8 err;
SX_FORTH_VARIABLE u8 errCnt;
void Error(u8 errCode);

//==============================================================================================================================
//                         APPLICATION is Driven by RTC Timer and Peripherals Interrupts
//==============================================================================================================================

// SX Timer Interrupt Tick occurs any MSEC_SYSTEM_TICK (running from the RTC 32.768kHz crystal)
#if AVR
 ISR(TIMER2_OVF_vect);
#endif

s16 main( void );


//------------------------------------------------------------------------------------------------------------------------------
//                                              SX-Forth -> C Functions
//------------------------------------------------------------------------------------------------------------------------------

#if STDIO_USED
	// Forth RETURN STACK (Last IN, First OUT behaviour)
	#define RETURN_STACK_MAX_ENTRIES    4
	SX_FORTH_VARIABLE u8    returnTOS;    // Top Of Forth RETURN Stack
	SX_FORTH_VARIABLE u8    returnSTACK[RETURN_STACK_MAX_ENTRIES];

	// common scratch variables for Forth and C
	SX_FORTH_VARIABLE s16 s16Tmp0, s16Tmp1, s16Tmp2;
	SX_FORTH_VARIABLE u8  u8Tmp0;

	// BUILTIN 'numberBase'
	#define  DECIMAL     10
	#define  EXADECIMAL  16 // default BASE
	SX_FORTH_VARIABLE s16 numberBase;

	// BUILTIN 'debug' and 'step'
	SX_FORTH_VARIABLE s8 debug;   // default 0=FALSE
	SX_FORTH_VARIABLE s8 step;

	// builtin I and J variables
	SX_FORTH_VARIABLE s16 i_var, j_var;

	void F_DecimalBASE(void);
	void F_HexadecimalBASE(void);

	// BUILTIN 'debug'
	void F_DBG_Yes(void);
	void F_DBG_No(void);

	// BUILTIN 'step'
	void F_Step_Yes(void);
	void F_Step_No(void);

	// BUILTIN 'memoryType'
	void F_Ram(void);

	// BUILTIN 'ramUser'
	void F_UserRamADDR(void);

	// MEMORY ADRESSES
	void F_Eeprom(void);

	void F_Flash(void);

	// data stack operation
	// NOTE: Store (!) and Fetch (@) words are executed based on the current 'memoryType' value (default to RAM)
	void F_StoreWord(void);
	void F_StoreByte(void);
	void F_FetchWord(void);
	void F_FetchByte(void);

	// dataTOS content display
	void F_Dot_TOS(void);

	// dataTOS print as a character
	void F_Dot_TOS_Char(void);

	// CR print
	void F_PutCR(void);

	// dataTOS indirect string print, dataTOS dropped
	void F_Dot_String(void);

	// Non Destructive Print, using BASE, of dataSTACK contents
	void F_Ask_DataSTACK(void);

	// Non Destructive Print, using BASE, of returnSTACK contents
	void F_Ask_ReturnSTACK(void);

	// print and clear interpreter stacks
	void F_Print_And_ClearSTACKs(void);

	// data stack operators
	void F_Dup(void);
	void F_Drop(void);
	void F_Swap(void);

	// unary operators ( w -- wRes )
	void F_Inc(void);
	void F_Dec(void);
	void F_Not(void);
	void F_Abs(void);

	// binary operators ( w1 w2 -- wRes )
	void F_Or(void);
	void F_Xor(void);
	void F_And(void);
	void F_Add(void);
	void F_Diff(void);
	void F_Mult(void);
	void F_Div(void);
	void F_Mod(void);
	void F_ShiftLEFT(void);
	void F_ShiftRIGTH(void);

	// trinary operator
	void F_Scaling(void);

	// binary comparisons
	void F_Equal(void);
	void F_NotEqual(void);
	void F_Greather(void);
	void F_GreatherOrEqual(void);
	void F_Lower(void);
	void F_LowerOrEqual(void);

	// builtin  I variable operators
	void F_Addr_I(void);
	void F_Store_I(void);
	void F_Fetch_I(void);
	void F_Display_I(void);
	void F_Inc_I(void);
	void F_Dec_I(void);

	// builtin J variable operators
	void F_Addr_J(void);
	void F_Store_J(void);
	void F_Fetch_J(void);
	void F_Display_J(void);
	void F_Inc_J(void);
	void F_Dec_J(void);

	// variable I indirect operators
	void F_I_IndirectFetch(void);
	void F_I_IndirectFetchInc(void);
	void F_I_IndirectFetchByte(void);
	void F_I_IndirectFetchByteInc(void);
	void F_I_IndirectStore(void);
	void F_I_IndirectStoreInc(void);
	void F_I_IndirectStoreByte(void);
	void F_I_IndirectStoreByteInc(void);

	// EXIT if TOS is 0
	void F_Zero_T0S_Exit(void);

	// Ask for User Entry
	void F_User_Entry(void);

	// IF ... [ELSE ...] ENDIF construct
	void F_If(void);
	void F_Else(void);
	void F_Endif(void);

	// DO ... LOOP construct
	void F_Do(void);
	void F_Loop(void);
	void F_Zero_T0S_Break(void);

	void F_Words_Display(void);
	void F_Pause(void);
	void F_CheckPause(void);
	void F_C_FunctionCALL_by_Name(void);

	void Forth_Interpreter(void);
	void STDIN_Get_String(void);
	void STDIN_Parse_String(void);

#endif // STDIO_USED




