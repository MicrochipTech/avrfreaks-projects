/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef USI_H
#define USI_H
	
	// INCLUDES:
	#include <avr/io.h>

	// DEFINES:
	#define USIDelay              GPIOR0 // Psudo-variable, GPIO register for speed

	#define USICONTROLREGS        ((1 << USIWM0) | (1 << USITC))

	#define USI_CLOCK_OUT_PORT    PINF   // PIN register - toggles pin when written to on the M169 if pin is an output
	#define USI_CLOCK_BIT         7

	#define USI_DATAOUT_PIN	      6
	#define USI_DATAIN_PIN	      5
	#define USI_CLOCK_PIN	      4

	#ifndef __ASSEMBLER__
	
		// INCLUDES:
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <avr/pgmspace.h>
			
		#include "AVRISPCommandBytes.h"
		#include "Delay.h"
		#include "GlobalMacros.h"
		#include "Main.h"
		#include "Settings.h"

		// OTHER DEFINES:
		#define USI_PRESET_SPEEDS  7
		#define USI_STUDIO_SPEEDS  5

		// PROTOTYPES:
		void           USI_SPIInitMaster(void);
		void           USI_SPIOff(void);
		void           USI_SPIToggleClock(void);
		extern uint8_t USI_SPITransmit(const uint8_t val);

		// PUBLIC INLINE FUNCTIONS:
		static inline uint8_t USI_SPITransmitWord(const uint16_t val) ATTR_ALWAYS_INLINE;
		static inline uint8_t USI_SPITransmitWord(const uint16_t val)
		{
			USI_SPITransmit(val >> 8);
			return USI_SPITransmit(val & 0xFF);
		}
	
	#endif
#endif
