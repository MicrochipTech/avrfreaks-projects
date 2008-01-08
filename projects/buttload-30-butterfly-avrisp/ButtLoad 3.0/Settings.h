/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>

	#include "EEPROMVariables.h"
	#include "GlobalMacros.h"
	#include "LCD_Driver.h"
	#include "Main.h"
	#include "ToneGeneration.h"
	#include "USI.h"
	#include "V2Protocol.h"

	// EXTERNAL VARIABLES:
	extern const char USISpeedVals[];
	extern const char USISpeedIndex[];

	// PROTOTYPES:
	void SET_SetContrast(void);
	void SET_SetISPSpeed(void);
	void SET_SetResetMode(void);
	void SET_SetFirmMinorVer(void);
	void SET_SetAutoSleepTimeOut(void);
	void SET_SetToneVol(void);
	void SET_SetStartupMode(void);

#endif
