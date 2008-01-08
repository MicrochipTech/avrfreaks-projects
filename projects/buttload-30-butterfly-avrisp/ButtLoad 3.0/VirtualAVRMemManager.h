/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef VAMM_H
#define VAMM

	// INCLUDES:
	#include <avr/pgmspace.h>

	#include "Main.h"
	#include "LCD_Driver.h"
	#include "Dataflash.h"
	#include "EEPROMVariables.h"
	#include "V2Protocol.h"

	// DEFINES:
	#define VAMM_SETUP_NA             0
	#define VAMM_SETUP_WRITE          1
	#define VAMM_SETUP_READ           2
	#define VAMM_SETUP_ADDR_DONE      3
	
	#define VAMM_ERASE_STOREPACKET    0
	#define VAMM_ERASE_ERASEDATA      1
	
	
	// EXTERNAL VARIABLES:
	uint8_t EraseDataflash;
	
	// PROTOTYPES:
	void    VAMM_EraseAVRMemory(uint8_t Mode);
	void    VAMM_SetAddress(void);

	void    VAMM_StoreByte(const uint8_t Data);
	uint8_t VAMM_ReadByte(void) ATTR_WARN_UNUSED_RESULT;
	uint8_t VAMM_ReadConsec(void) ATTR_WARN_UNUSED_RESULT;

	void    VAMM_Cleanup(void);
	
#endif
