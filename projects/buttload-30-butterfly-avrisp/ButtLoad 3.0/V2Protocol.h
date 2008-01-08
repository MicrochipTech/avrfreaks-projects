/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef V2PROTOCOL_H
#define V2PROTOCOL_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>
	
	#include "AVRISPCommandBytes.h"
	#include "AVRISPCommandInterpreter.h"
	#include "Delay.h"
	#include "EEPROMVariables.h"
	#include "LCD_Driver.h"
	#include "ProgramManager.h"
	#include "Timeout.h"
	#include "USART.h"

	// DEFINES AND MACROS:
	#define V2P_MAXBUFFSIZE              275 // Maximum message size length (275 is the same as the STK500)
	
	#define V2P_STATE_IDLE               0
	#define V2P_STATE_START              1
	#define V2P_STATE_GETSEQUENCENUM     2
	#define V2P_STATE_GETMESSAGESIZE1    3
	#define V2P_STATE_GETMESSAGESIZE2    4
	#define V2P_STATE_GETTOKEN           5
	#define V2P_STATE_GETDATA            6
	#define V2P_STATE_GETCHECKSUM        7
	#define V2P_STATE_BADCHKSUM          8
	#define V2P_STATE_TIMEOUT            9
	#define V2P_STATE_PACKERR            10
	#define V2P_STATE_PACKOK             11
	
	#define V2P_LOAD_EXTENDED_ADDR_CMD   0x4D
	
	#define V2P_HW_VERSION               2
	#define V2P_SW_VERSION_MAJOR         2
	#define V2P_SW_VERSION_MINOR_DEFAULT 10
	
	// EXTERNAL VARIABLES:
	extern uint8_t  PacketBytes[V2P_MAXBUFFSIZE];
	extern uint16_t MessageSize;
	extern uint8_t  InProgrammingMode;
	extern uint32_t CurrAddress;
	
	// PROTOTYPES:
	void V2P_RunStateMachine(const FuncPtr PacketDecodeFunction) ATTR_NON_NULL_PTR_ARGS(1);
	void V2P_SendPacket(void);
	void V2P_CheckForExtendedAddress(void);
	void V2P_ClearCurrAddress(void);

	#if defined(INC_FROM_V2P)
	  static uint8_t V2P_GetChecksum(void) ATTR_WARN_UNUSED_RESULT;
	  static void    V2P_ProcessPacketData(const FuncPtr PacketDecodeFunction) ATTR_NON_NULL_PTR_ARGS(1);
	  static void    V2P_GetSetParameter(void);
	#endif

	// PUBLIC INLINE FUNCTIONS:
	static inline void V2P_IncrementCurrAddress(void) ATTR_ALWAYS_INLINE;
	static inline void V2P_IncrementCurrAddress(void)
	{
		/* Since CurrAddress is incremented often (and requires many bytes to do so), I've opted
		   to cut down on the overhead via optimized inline assembly to do the increment, so only
		   the lower 24 bits (that are used, upper byte is for flags) are loaded, incremented and
		   stored back to SRAM.                                                                   */

		uint16_t TempWord;
	
		asm volatile ( "LD	  r24, %a1+            \n\t"
					   "LD	  r25, %a1+            \n\t"
					   "LD	  r23, %a1+            \n\t"
					   "ADIW  r24, 0x01            \n\t"
					   "ADC	  r23, __zero_reg__    \n\t"
					   "ST	 -%a1, r23             \n\t"
					   "ST	 -%a1, r25             \n\t"
					   "ST	 -%a1, r24             \n\t"
					   : "=&w" (TempWord) : "e" (&CurrAddress) : "r23", "r24", "r25" );
	}
	
#endif
