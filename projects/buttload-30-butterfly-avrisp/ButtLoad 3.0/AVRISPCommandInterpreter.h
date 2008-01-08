/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef AICI_H
#define AICI_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	#include "AVRISPCommandBytes.h"
	#include "Delay.h"
	#include "ISPChipComm.h"
	#include "LCD_Driver.h"
	#include "ToneGeneration.h"
	#include "V2Protocol.h"
	
	// EXTERNAL VARIABLES:
	extern const char AVRISPModeMessage[] PROGMEM;
	
	// PROTOTYPES:
	       void    AICI_InterpretPacket(void);
	extern uint8_t AICI_SendCalibrationClocks(void);
	
#endif
