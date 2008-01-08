/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef TIMEOUT_H
#define TIMEOUT_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	
	#include "Delay.h"
	#include "GlobalMacros.h"
	#include "Main.h"
	
	// DEFINES AND MACROS:
	#define TIMEOUT_HZ_TO_COMP(Hz, Src, Psc) ((((Src == TIMEOUT_SRC_CPU)? F_CPU : 32768UL) / Psc) / Hz)
	#define TIMEOUT_SRC_CPU                  0
	#define TIMEOUT_SRC_RTC                  1
	
	#define TIMEOUT_PACKET_TIMEOUTTICKS      (32 * 5)   // Timeout every 5 secs (which is close to the computer timout period)
	#define TIMEOUT_PACKET_TIMER_OFF()       MACROS{ TCCR0A = 0; TIMSK0 = 0; }MACROE
	#define TIMEOUT_PACKET_TIMER_ON()        MACROS{ PacketTimeOut = FALSE;  \
												     PacketTimeOutTicks = 0; \
												     TCNT0  = 0;             \
												     OCR0A  = TIMEOUT_HZ_TO_COMP(32, TIMEOUT_SRC_CPU, 1024);         \
												     TIMSK0 = (1 << OCIE0A); \
												     TCCR0A = ((1 << WGM01) | (1 << CS02) | (1 << CS01) | (1 << CS00)); }MACROE
	
	#define TIMEOUT_SLEEP_TIMER_OFF()        MACROS{ TCCR2A = 0; TIMSK2 = 0; ASSR = 0; }MACROE
	#define TIMEOUT_SLEEP_TIMER_ON()         MACROS{ ASSR   = (1 << AS2);      \
	                                                 TCCR2A = ((1 << WGM21) | (1 << CS22) | (1 << CS21));            \
													 while (ASSR & ((1 << TCN2UB) | (1 << TCR2UB) | (1 << OCR2UB))); \
													 TCNT0  = 0;                                                     \
													 TIMSK2 = (1 << OCIE2A);                                            }MACROE
	#define TIMEOUT_SLEEP_TIMEOUT_RESET()    MACROS{ SleepTimeOutSecs = 0; TCNT2 = 0; }MACROE
	
	// EXTERNAL VARIABLES:
	extern volatile uint8_t PacketTimeOut;
	extern volatile uint8_t PacketTimeOutTicks;
	extern volatile uint8_t SleepTimeOutSecs;
	extern volatile uint8_t SecsBeforeAutoSleep;

	extern const    uint8_t AutoSleepTOValues[5] PROGMEM;

	// PROTOTYPES:
	void TOUT_SetupSleepTimer(void);
		
#endif
