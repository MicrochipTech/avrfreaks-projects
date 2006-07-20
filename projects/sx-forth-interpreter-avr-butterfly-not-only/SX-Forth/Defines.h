// ***********************
// ***    Defines.h    ***
// ***********************

/*------------------------------------------------------------------
	SX-OS the Events Driven Multitasking System eXecutive
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
Multitasking Operating System eXecutive written by Valter Foresto.

--------------------------------------------------------------------
Comm.SEC di Valter Foresto
Via Quittengo 37 - 10154 Torino
valter.foresto@tematsrl.it
Tel/Fax +39 011 232539
http://digilander.libero.it/Comm.SEC/
-------------------------------------------------------------------*/



//==============================================================================================================================
/*
  This file is used to define the DEVELOPMENT ENVIRONMENT and TARGET CONFIGURATION and include :
		- the Target MICROCONTROLLER Files,
		- the Used COMPILER / DEVELOPMEMT TOOLS CHAINS Files,
		- Common DEFINES used in the APPLICATION CONFIGURATION 'System.h' file,
		- Usefull MACRO definitions for User Projects
*/


//==============================================================================================================================
// TARGET PROJECT and HW PLATFORM :    MICROCONTROLLER and COMPILER
//==============================================================================================================================
#define  PC        0  // Microcontrollers x86,         Compiler NI LabWindows CVI 6.0 or above
#define  AVR       1  // Microcontrollers AVR Family,  Compiler GNU_GCC WinAVR on AVR Studio 4
#define  H8        0  // Microcontrollers H8_TINY,     Compiler RENESAS HEW4


//==============================================================================================================================
// PORTs 'stdin', 'stdout' and 'stderr' CHANNEL used with 'Sx_Stdio' module
//==============================================================================================================================

#define PORT_0      0                   // Communication Channel (RS232 typ) 'Port 0'
#define PORT_1      1                   // Communication Channel (RS232 typ) 'Port 1'
#define PORT_2      2                   // Communication Channel (RS232 typ) 'Port 2'
#define PORT_3      3                   // Communication Channel (RS232 typ) 'Port 3'

#define STDIO_CONSOLE    		PORT_0    // The Default SX-OS STDIO CONSOLE


//==============================================================================================================================
// ALWAYS Include these HEADER FILES :
//==============================================================================================================================

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>



//==============================================================================================================================
// TARGET PROJECT and HW PLATFORM Includes and Defines :
//==============================================================================================================================

//------------------------------------------------------------------------------------------------------------------------------
#if PC                      // Microcontrollers x86,   HW Platform PC,   Compiler NI LabWindows CVI 6.0 or above
	#define LITTLE_ENDIAN   1
	#define BIG_ENDIAN      0

	typedef char               s8;
	typedef unsigned char      u8;
	typedef short int          s16;
	typedef unsigned short int u16;
	typedef int                s32;
	typedef unsigned int       u32;
	typedef float              f32;
	typedef double             f64;

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <mmsystem.h>
	#include <cvirte.h>
	#include <utility.h>
	#include <userint.h>
	#include <formatio.h>
	#include <ansi_c.h>
	#include <rs232.h>
	#include <lowlvlio.h>

	#include <asynctmr.h>
	#include <toolbox.h>

	// used STDIO PORTs and associated CHANNELs
	#define MAX_USED_STDIO_PORTS     1
	#define MAX_USED_STDIO_CHANNELS  255

	#define PORT_0_USED              TRUE
	#define PORT_1_USED              FALSE
	#define PORT_2_USED              FALSE
	#define PORT_3_USED              FALSE
	#define DEFAULT_PORT_0_CHANNEL   0  // 0 == PC Win32 Console, 1 ... n == COMx RS232 Communication Channel
	#define DEFAULT_PORT_1_CHANNEL   5  // 0 == PC Win32 Console, 1 ... n == COMx RS232 Communication Channel
	#define DEFAULT_PORT_2_CHANNEL   0  // 0 == PC Win32 Console, 1 ... n == COMx RS232 Communication Channel
	#define DEFAULT_PORT_3_CHANNEL   0  // 0 == PC Win32 Console, 1 ... n == COMx RS232 Communication Channel

	#pragma pack(1);

#endif

//------------------------------------------------------------------------------------------------------------------------------
#if AVR                       // Microcontrollers AVR Family, Compiler GNU_GCC WinAVR on AVR Studio
	#define LITTLE_ENDIAN   1
	#define BIG_ENDIAN      0

	#define GCC_COMPILER_USED	TRUE

	// AVR used Microcontroller
	#if H8
		#define  AT_MEGA_169       1
		#define  AT_MEGA_128       0
	#endif

	typedef char               s8;
	typedef unsigned char      u8;
	typedef int                s16;
	typedef unsigned int       u16;
	typedef long int           s32;
	typedef unsigned long int  u32;
	typedef float              f32;
	typedef double             f64;      // like 'f32', see LIBC Library Docs


	// AVR Libraries INCLUDES
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/boot.h>
	#include <avr/pgmspace.h>
	#include <avr/portpins.h>
	#include <avr/sleep.h>
	#include <avr/wdt.h>
	#include <avr/version.h>   // 'libc'

	// AVR Utilities Libraries INCLUDES
	#include <util/crc16.h>
	#include <util/parity.h>
	#include <util/twi.h>

	// AVR Utilities Libraries INCLUDES for Timing DELAYs
	#define F_CPU 1000000UL    // CPU Core Frequency Clock
	#include <util/delay.h>
	#define DelayMicroseconds(us) _delay_us(us)
	#define DelayMilliseconds(ms) _delay_ms(ms)

	#define SetPortBit(port,bit)     (port |=  (1<<bit))  // set 'bit' in 'port'
	#define ClearPortBit(port,bit)   (port &= ~(1<<bit))  // clear 'bit' in 'port'

	// used STDIO PORTs and associated STDIO CHANNELs
	#define MAX_USED_STDIO_PORTS     1
	#define MAX_USED_STDIO_CHANNELS  2

	#define PORT_0_USED              TRUE
	#define PORT_1_USED              FALSE
	#define PORT_2_USED              FALSE
	#define PORT_3_USED              FALSE
	#define DEFAULT_PORT_0_CHANNEL   0  // 0 ... n == COMx RS232 Communication Channel
	#define DEFAULT_PORT_1_CHANNEL   1  // 0 ... n == COMx RS232 Communication Channel

	#define STDIO_CHANNELS_RX_INTERRUPT    FALSE
	#define STDIO_CHANNELS_TX_INTERRUPT    FALSE

#endif

//------------------------------------------------------------------------------------------------------------------------------
#if H8
	#define LITTLE_ENDIAN   0
	#define BIG_ENDIAN      1

	// H8 used Microcontroller
	#if H8
		#define  H8_3664F          1
		#define  H8_3687F          0
	#endif

	#include "3664s.h"
	#include <machine.h>
	#include "H8_3664F.h"

	typedef char               s8;
	typedef unsigned char      u8;
	typedef int                s16;
	typedef unsigned int       u16;
	typedef long int           s32;
	typedef unsigned long int  u32;
	typedef float              f32;
	typedef double             f64;

	// used PORTs and associated CHANNELs
	#define MAX_USED_STDIO_PORTS     1
	#define MAX_USED_STDIO_CHANNELS  6

	#define PORT_0_USED              TRUE
	#define PORT_1_USED              FALSE
	#define PORT_2_USED              FALSE
	#define PORT_3_USED              FALSE
	#define DEFAULT_PORT_0_CHANNEL   0  // 0 ... n == COMx RS232 Communication Channel
	#define DEFAULT_PORT_1_CHANNEL   1  // 0 ... n == COMx RS232 Communication Channel

	#define STDIO_CHANNELS_RX_INTERRUPT    TRUE
	#define STDIO_CHANNELS_TX_INTERRUPT    FALSE

	#pragma pack 1
#endif


//==============================================================================================================================
// SYSTEM ENVIRONMENT Defines & Macros
//==============================================================================================================================

// SX COMPONENT PUBLIC INTERFACE :
#define  TASK
#define  FRAME
#define  EVENT_HANDLER
#define  COMMAND_HANDLER
#define  FUNCTION

#define  PUBLIC_TASK
#define  PUBLIC_FRAME
#define  PUBLIC_EVENT_HANDLER
#define  PUBLIC_COMMAND_HANDLER
#define  PUBLIC_FUNCTION

// SX COMPONENT LOCALS :
#define  LOCAL_TASK              static
#define  LOCAL_FRAME             static
#define  LOCAL_TIMER_HANDLER     static
#define  LOCAL_EVENT_HANDLER     static
#define  LOCAL_COMMAND_HANDLER   static
#define  LOCAL_FUNCTION          static

// CLASSICs Defines :
#ifndef NULL
	#define NULL   0
#endif

#ifndef TRUE
	#define TRUE   1
#endif

#ifndef FALSE
	#define FALSE  0
#endif

// BINARY SEMAPHORES Values for Shared Resource Access :
#define GREEN        0
#define RED          1

// CONTINUATION and EXIT (used on Callback Events Functions or return value)
#ifndef CONTINUE
	#define CONTINUE  1
#endif

#ifndef EXIT
	#define EXIT      -1    // like FAIL
#endif

#ifndef READY
	#define READY     2
#endif

// Activity-Service RESULT Defines :
#ifndef PASS
	#define PASS      0		
#endif

#ifndef FAIL
	#define FAIL      -1
#endif

#ifndef FREE
	#define FREE       1    // like TRUE
#endif

#ifndef BUSY
	#define BUSY      -1    // like FAIL
#endif


// Memories
#define  KBYTES   1024


//==============================================================================================================================
// u8 ... u64 BITWISE Macros
//==============================================================================================================================

#ifndef BIT
 #define BIT(b)          (1 << b)
#endif

#ifndef BIT_TEST
 #define BIT_TEST(a,b)   ((a) & (1 << b))
#endif

#ifndef BIT_SET
 #define BIT_SET(a,b)    ((a) |= (1 << b))
#endif

#ifndef BIT_CLEAR
 #define BIT_CLEAR(a,b)  ((a) &= ~(1 << b))
#endif

#ifndef BIT_TOGGLE
	#define BIT_TOGGLE(a,b) (BIT_TEST(a,b) ? BIT_CLEAR(a,b) : BIT_SET(a,b))
#endif

//==============================================================================================================================
// u8 ARRAY BITWISE Macros
//==============================================================================================================================

#ifndef ARRAY_BIT_TEST
	#define ARRAY_BIT_TEST(a,b)   ( *(a+(b/8))  &  (1<<(b%8)) )
#endif

#ifndef ARRAY_BIT_SET
	#define ARRAY_BIT_SET(a,b)    ( *(a+(b/8)) |=  (1<<(b%8)) )
#endif

#ifndef ARRAY_BIT_CLEAR
	#define ARRAY_BIT_CLEAR(a,b)  ( *(a+(b/8)) &= ~(1<<(b%8)) )
#endif

#ifndef ARRAY_BIT_TOGGLE
	#define ARRAY_BIT_TOGGLE(a,b) (ARRAY_BIT_TEST(a,b) ? ARRAY_BIT_CLEAR(a,b) : ARRAY_BIT_SET(a,b))
#endif


//==============================================================================================================================
// UNIONS and ENDIAN Macros
//==============================================================================================================================

typedef union
	{
	f64 d;
	//
	s32 sw[2];
	s16 ss[4];
	s8  sb[8];
	//
	u32 uw[2];
	u16 us[4];
	u8  ub[8];
	} union64;

typedef union
	{
	f32 f;
	//
	s32 sw;
	s16 ss[2];
	s8  sb[4];
	//
	u32 uw;
	u16 us[2];
	u8  ub[4];
	} union32;

typedef union
	{
	s16 ss;
	s8  sb[2];
	//
	u16 us;
	u8  ub[2];
	} union16;

typedef union
	{
	s8  sb;
	//
	u8  ub;
	} union8;



//--- union16/32 Endian Macros ---

#if LITTLE_ENDIAN     // => 16bit: (LSB,MSB), 32bit: (LSW,MSW) or (LSB0,LSB1,LSB2,LSB3) or (MSB3,MSB2,MSB1,MSB0)
	#define MSB(union16)        ((union16).sb[1])
	#define LSB(union16)        ((union16).sb[0])

	#define MSW(union32)        ((union32).ss[1])
	#define LSW(union32)        ((union32).ss[0])
	#define MSB0(union32)       ((union32).sb[3])
	#define MSB1(union32)       ((union32).sb[2])
	#define MSB2(union32)       ((union32).sb[1])
	#define MSB3(union32)       ((union32).sb[0])
	#define LSB0(union32)       ((union32).sb[0])
	#define LSB1(union32)       ((union32).sb[1])
	#define LSB2(union32)       ((union32).sb[2])
	#define LSB3(union32)       ((union32).sb[3])
#else // BIG_ENDIAN         => 16bit: (MSB,LSB), 32bit: (MSW,LSW) or (LSB3,LSB2,LSB1,LSB0) or (MSB0,MSB1,MSB2,MSB3)
	#define MSB(union16)        ((union16).sb[0])
	#define LSB(union16)        ((union16).sb[1])

	#define MSW(union32)        ((union32).ss[0])
	#define LSW(union32)        ((union32).ss[1])
	#define MSB0(union32)       ((union32).sb[0])
	#define MSB1(union32)       ((union32).sb[1])
	#define MSB2(union32)       ((union32).sb[2])
	#define MSB3(union32)       ((union32).sb[3])
	#define LSB0(union32)       ((union32).sb[3])
	#define LSB1(union32)       ((union32).sb[2])
	#define LSB2(union32)       ((union32).sb[1])
	#define LSB3(union32)       ((union32).sb[0])
#endif


//--- Host to Network conversion ---

// Used for Intel HEX format, TCP/IP, ... convert a 16-bit value from host-byte order to network-byte order

// EC16 is a 16 bits Endian Converter
#define EC16(b) ( ((u16)((b) & 0xFF)     <<  8)    |  (((u16)(b) & 0xFF00)      >> 8) )

// EC32 is a 32 bits Endian Converter
#define EC32(b) ( ((u32)((b) & 0xFF)     << 24)    |  ((u32)((u16)(b) & 0xFF00) << 8) | \
						((u32)((b) & 0xFF0000) >>  8)    |  ((u32)((b) & 0xFF000000)  >> 24) )

#if LITTLE_ENDIAN
	// Host to Net Short (16 Bits)
	#define htons(a)    EC16(a)
	// Net to Host Short (16 Bits)
	#define ntohs(a)    EC16(a)

	// Host to Net Long (32 Bits)
	#define htonl(a)    EC32(a)
	// Net to Host Long (32 Bits)
	#define ntohl(a)    EC32(a)
#else
	// Host to Net Short (16 Bits)
	#define htons(a)    (a)
	// Net to Host Short (16 Bits)
	#define ntohs(a)    (a)

	// Host to Net Long (32 Bits)
	#define htonl(a)    (a)
	// Net to Host Long (32 Bits)
	#define ntohl(a)    (a)
#endif


//==============================================================================================================================
// Structures and Unions :   OFFSET_OF(s, sm)    and   SIZE_OF(s, sm)
//==============================================================================================================================

/* -------------------------------------------
	--- OFFSET_OF(s, sm) and SIZE_OF(s, sm) ---
	-------------------------------------------

	The macro 'offsetof()' is defined usually in the 'stddef.h' standard header.

	The two following macros are the Comm.SEC enhancement used for doing STRONG PORTABILITY and
	COMMUNICATION INDEPENDENCE.
	Compiler to Compiler structure allocation and access differences can be overridden using
	the following two macros.

	OFFSET_OF(s, sm) is a macro that returns the number of bytes that the structure member field
	'sm' is offset from the beginning of the structure 's'.
	OFFSET_OF(s, m) may return an offset for 'sm' that is larger than the sum of the sizes of
	all the members that precede it. This will be due to the fact that some Compilers implementations
	insert padding into a structure to ensure that they are properly aligned for the used
	Microcontroller.

	SIZE_OF(s, sm) is a macro that returns the number of bytes that the field 'sm' really use in
	the structure 's'.

*/
#define OFFSET_OF(s, sm)  ((size_t) ((char *)&(((s *)8)->sm) - (char *)8))
#define SIZE_OF(s, sm)    ((size_t) sizeof(((s *)0)->sm))

/*
	// ********************************************************************
	// *** EXAMPLE of Use of OFFSET_OF() and SIZE_OF() structure macros ***
	// ********************************************************************

	struct s {
		  char   a[13];
		  long   b;
		  char   c[7];
		  short  d;
		  char   e[3];
		  };

	void main (void)
		{
		int A, B, C, D, E;
		int sA, sB, sC, sD, sE;

		// *****************
		// *** OFFSET_OF ***
		// *****************
		A = OFFSET_OF(struct s, a[0]);
		B = OFFSET_OF(struct s, b);
		C = OFFSET_OF(struct s, c[0]);
		D = OFFSET_OF(struct s, d);
		E = OFFSET_OF(struct s, e[0]);

		printf ("%d %d %d %d %d\n", A, B, C, D, E);
		// *** If defined 1 byte packing the output is :   0 13 17 24 26
		// *** If defined 4 byte packing the output is :   0 16 20 28 30

		// ***************
		// *** SIZE_OF ***
		// ***************
		sA = SIZE_OF(struct s, a);
		sB = SIZE_OF(struct s, b);
		sC = SIZE_OF(struct s, c);
		sD = SIZE_OF(struct s, d);
		sE = SIZE_OF(struct s, e);

		printf ("%d %d %d %d %d\n", sA, sB, sC, sD, sE);
		// *** ALWAYS the output is :                      13 4 7 2 3
		}
*/

//==============================================================================================================================
// OTHER USEFULL Defines and Macros ... here :
//==============================================================================================================================

#ifndef MAX
	#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
	#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS_VAL
	#define ABS_VAL(a)   ((a) < 0 ? (-(a)) : (a))
#endif


//=======================================================================================
//         MACRO 'NEG_CHECK()' and 'NULL_CHECK()' for FUNCTIONS ERROR HANDLING
//=======================================================================================

/*
The NEG_CHECK(), NULL_CHECK() and POS_CHECK() macros are useful for implementing a consistent error
handling system.  These macros can be place around function calls to
force an  automatic jump to a builtin function's error handling code when an error
occurs.  This is analogous to exception handling in Java and is even easier to use.

These macros make the following assumptions:

	1)  Every function declare an automatic internal signed 32 bits integer variable named 'err'.

	2)  Every function in which they are used contains a goto label
		 named 'ErrorsHandler:' which precedes the error handling code for the function.

	3)  Every function call or error code enclosed in a xxx_CHECK() macro
		 is assumed to return a max signed 32 bits integer which, the code is used for
		 the error trap.
*/

#define NEG_CHECK(fCall)         err = (s32)(fCall);        \
											if (err < 0)               \
												{ goto ErrorsHandler; }

#define NULL_CHECK(fCall)        err = (s32)(fCall);        \
											if (err == 0)              \
												{ goto ErrorsHandler; }

#define POS_CHECK(fCall)         err = (s32)(fCall);        \
											if (err > 0)               \
												{ goto ErrorsHandler; }

// Macro used to directly reach the internal function 'ErrorsHandler' section.
#define ErrorsHandler(e)         { err=e; goto ErrorsHandler; }


//==============================================================================================================================
//--- IA5 Control Set Characters ---
//==============================================================================================================================

#define  NUL      0x00  // CTRL-@
#define  SOH      0x01  // CTRL-A
#define  STX      0x02  // CTRL-B
#define  ETX      0x03  // CTRL-C
#define  EOT      0x04  // CTRL-D
#define  ENQ      0x05  // CTRL-E
#define  ACK      0x06  // CTRL-F
#define  BEL      0x07  // CTRL-G
#define  BS       0x08  // CTRL-H
#define  HT       0x09  // CTRL-I
#define  LF       0x0A  // CTRL-J
#define  VT       0x0B  // CTRL-K
#define  FF       0x0C  // CTRL-L
#define  CR       0x0D  // CTRL-M
#define  SO       0x0E  // CTRL-N
#define  SI       0x0F  // CTRL-O
#define  DLE      0x10  // CTRL-P
#define  DC1      0x11  // CTRL-Q
#define  DC2      0x12  // CTRL-R
#define  DC3      0x13  // CTRL-S
#define  DC4      0x14  // CTRL-T
#define  NAK      0x15  // CTRL-U
#define  SYN      0x16  // CTRL-V
#define  ETB      0x17  // CTRL-W
#define  CAN      0x18  // CTRL-X
#define  EM       0x19  // CTRL-Y
#define  SUB      0x1A  // CTRL-Z
#define  ESC      0x1B  // CTRL-[
#define  IS4      0x1C  /* CTRL-\ */
#define  IS3      0x1D  // CTRL-]
#define  IS2      0x1E  // CTRL-^
#define  IS1      0x1F  // CTRL-_

#define  CTRL_A   SOH
#define  CTRL_B   STX
#define  CTRL_C   ETX
#define  CTRL_D   EOT
#define  CTRL_E   ENQ
#define  CTRL_F   ACK
#define  CTRL_G   BEL
#define  CTRL_H   BS
#define  CTRL_I   HT
#define  CTRL_J   LF
#define  CTRL_K   VT
#define  CTRL_L   FF
#define  CTRL_M   CR
#define  CTRL_N   SO
#define  CTRL_O   SI
#define  CTRL_P   DLE
#define  CTRL_Q   DC1
#define  CTRL_R   DC2
#define  CTRL_S   DC3
#define  CTRL_T   DC4
#define  CTRL_U   NAK
#define  CTRL_V   SYN
#define  CTRL_W   ETB
#define  CTRL_X   CAN
#define  CTRL_Y   EM
#define  CTRL_Z   SUB


