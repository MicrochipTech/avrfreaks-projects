/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef STOREMGR_H
#define STOREMGR_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/eeprom.h>
	
	#include "AVRISPCommandBytes.h"
	#include "Dataflash.h"
	#include "EEPROMVariables.h"
	#include "GlobalMacros.h"
	#include "V2Protocol.h"
	#include "VirtualAVRMemManager.h"
	
	// DEFINES:
	#define SM_NO_SETUP             0
	#define SM_DATAFLASH_WRITE      1
	#define SM_DATAFLASH_READ       2
	#define SM_LOCKFUSEBITS_WRITE   3
	#define SM_LOCKFUSEBITS_READ    4
	
	#define SM_MAX_FUSELOCKBITS     20
	
	#define SM_EEPROM_OFFSET        (((unsigned long)DF_DATAFLASH_PAGES / 2) * DF_INTERNALDF_BUFFBYTES)
	
	// EXTERNAL VARIABLES:
	extern uint8_t MemoryType;
	
	extern const char StorageText[] PROGMEM;

	// PROTOTYPES:
	uint32_t SM_GetStoredDataSize(const uint8_t Type) ATTR_WARN_UNUSED_RESULT;
	void     SM_InterpretAVRISPPacket(void);
	
	#if defined(INC_FROM_SM)
	  static void SM_CheckEndOfFuseLockData(void);
	#endif
	
#endif
