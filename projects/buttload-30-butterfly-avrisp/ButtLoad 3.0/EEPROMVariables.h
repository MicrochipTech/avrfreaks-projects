/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef EEVARS_H
#define EEVARS_H

	// INCLUDES:
	#include <avr/eeprom.h>
		
	// TYPE DEFINITIONS:
	typedef struct
	{
		uint8_t  JUNKVAR1;          // Unused location to guard against EEPROM corruption - this is mainly a
		                            // problem because the Butterflies come shipped with the BOD fuse unprogrammed
	
		uint8_t  ResetPolarity;
		uint8_t  SCKDuration;
		uint8_t  WriteProgram[10];
		uint8_t  WriteEEPROM[10];
		uint8_t  EraseCmdStored;
		uint8_t  EraseChip[6];
		uint8_t  EnterProgMode[12];
		uint8_t  TotalFuseBytes;
		uint8_t  TotalLockBytes;
		uint8_t  FuseBytes[20][4];
		uint8_t  LockBytes[20][4];
		uint16_t PageLength;
		uint16_t EPageLength;
		uint8_t  LCDContrast;
		uint8_t  FirmVerMinor;
		uint8_t  AutoSleepValIndex;
		uint8_t  SPIResetMode;
		uint8_t  ToneVolume;
		uint8_t  PGOptions;
		uint8_t  StartupMode;
		uint8_t  StoredData;
		uint8_t  StoredEEPROM;
		
		uint16_t MagicNumber;
		uint8_t  VersionNumber;
	} EEPROMVarsType;
	
#endif
