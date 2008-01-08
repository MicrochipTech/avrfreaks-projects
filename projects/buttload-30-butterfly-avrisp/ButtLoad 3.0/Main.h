/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef MAIN_H
#define MAIN_H

	// DEBUG SWITCHES
	 //#define DEBUG_JTAGON
	 //#define DEBUG_MEMFILLON
	 //#define DEBUG_ISRCATCHALL
	// END DEBUG SWITCHES
	
	// INCLUDES:
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/version.h>
	#include <util/atomic.h>

	#include "EEPROMVariables.h"
	#include "GlobalMacros.h"

	#if defined(INC_FROM_MAIN)
		#include "Dataflash.h"
		#include "Delay.h"
		#include "LCD_Driver.h"
		#include "OSCCal.h"
		#include "ProgramManager.h"
		#include "Settings.h"
		#include "SPI.h"
		#include "StorageManager.h"
		#include "Support/ButtLoadTag.h"
		#include "TagManager.h"
		#include "ToneGeneration.h"
		#include "USART.h"
		#include "USI.h"
		#include "V2Protocol.h"
		#include "VirtualAVRMemManager.h"
	#endif
	
	#if defined(INC_FROM_MAIN)
		// LIB C VERSION CHECK:
		#if (!defined(__AVR_LIBC_VERSION__) || (__AVR_LIBC_VERSION__ < 10401UL)) // In future AVRLibC version requirements may be increased with changes
			#error avr-libc version 1.4.1 or higher is required to compile this project.
		#endif
	
		// DEBUG MODE CHECKS:
		#if defined(DEBUG_JTAGON)
			#warning DEBUG_JTAGON option is activated - JTAG system is enabled. Remove before releasing.
		#endif
	#endif
	
	// EXTERNAL VARIABLES:
	extern const char BusyText[];
	extern const char VersionInfo[];

	#define JoyStatus                  GPIOR1 // Psudo-variable, GPIO register for speed

	extern EEPROMVarsType EEPROMVars   EEMEM;
	extern const uint8_t  BitTable[]   PROGMEM;
	
	// DEFINES AND MACROS:
	#define MAIN_STATUSLED_PORT        PORTF
	#define MAIN_STATUSLED_PIN         PINF
	
	#define MAIN_SETSTATUSLED(mask)    MACROS{ MAIN_STATUSLED_PORT = ((MAIN_STATUSLED_PORT & ~MAIN_STATLED_ORANGE) | (mask)); }MACROE
	#define MAIN_TOGGLESTATUSLED(mask) MACROS{ MAIN_STATUSLED_PIN |= mask; }MACROE

	#define MAIN_STATLED_GREEN         (1 << 4)
	#define MAIN_STATLED_RED           (1 << 5)
	#define MAIN_STATLED_ORANGE        (MAIN_STATLED_GREEN | MAIN_STATLED_RED)
	#define MAIN_STATLED_OFF           0
	
	#define MAIN_RESET_ACTIVE          0
	#define MAIN_RESET_INACTIVE        1
		
	// PROTOTYPES:
	int main(void) ATTR_NAKED ATTR_NO_RETURN; // Remove main prologue designed to allow for recursive use of main (not needed)

	void MAIN_SetTargetResetLine(const uint8_t ActiveInactive);
	void MAIN_WaitForJoyRelease(void);
	void MAIN_IntToStr(uint16_t IntV, char *Buff) ATTR_NON_NULL_PTR_ARGS(2);
	void MAIN_ShowError(const char *ErrorStr) ATTR_NON_NULL_PTR_ARGS(1);
	
	void MAIN_Delay10MS(uint8_t loops);
	void MAIN_Delay1MS(uint8_t loops);
	
	void MAIN_MenuSleep(void);
	void MAIN_SleepMode(void);

	#if defined(INC_FROM_MAIN)
	  static void MAIN_ShowAbout(void);

	  static void MAIN_AVRISPMode(void);
	  static void MAIN_ProgramAVR(void);
	  static void MAIN_StoreProgram(void);
	  static void MAIN_ChangeSettings(void);

	  static void MAIN_StorageInfo(void);

	  static void MAIN_ClearMem(void);
	  static void MAIN_GoBootloader(void) ATTR_NO_RETURN;
	#endif

	void MAIN_Util_RAMFill(void) ATTR_INIT_SECTION(0);
#endif
