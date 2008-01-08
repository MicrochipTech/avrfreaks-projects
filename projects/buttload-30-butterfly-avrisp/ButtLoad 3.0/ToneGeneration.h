/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef TONEGENERATION_H
#define TONEGENERATION_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	#include "EEPROMVariables.h"
	#include "GlobalMacros.h"
	#include "Main.h"
	
	// DEFINES AND MACROS:
	#define TONEGEN_SEQ_STARTUP    ToneSeq_Startup
	#define TONEGEN_SEQ_SYNCFAIL   ToneSeq_SyncFail
	#define TONEGEN_SEQ_PROGDONE   ToneSeq_ProgDone
	#define TONEGEN_SEQ_PROGFAIL   ToneSeq_ProgFail
	#define TONEGEN_SEQ_VOLTEST    ToneSeq_VolTest
	#define TONEGEN_SEQ_SLEEP      ToneSeq_Sleep
	#define TONEGEN_SEQ_RESUME     ToneSeq_Resume
	#define TONEGEN_SEQ_ERROR      ToneSeq_Error
	#define TONEGEN_SEQ_ERASINGDF  ToneSeq_VolTest

	#define TONEGEN_GET_TONE_VOL() MACROS{ ToneVol = eeprom_read_byte(&EEPROMVars.ToneVolume); }MACROE
	
	// EXTERNAL VARIABLES:
	extern const uint8_t ToneSeq_Startup[]  PROGMEM;
	extern const uint8_t ToneSeq_SyncDone[] PROGMEM;
	extern const uint8_t ToneSeq_SyncFail[] PROGMEM;
	extern const uint8_t ToneSeq_ProgDone[] PROGMEM;
	extern const uint8_t ToneSeq_ProgFail[] PROGMEM;
	extern const uint8_t ToneSeq_VolTest[]  PROGMEM;
	extern const uint8_t ToneSeq_Sleep[]    PROGMEM;
	extern const uint8_t ToneSeq_Resume[]   PROGMEM;	
	extern const uint8_t ToneSeq_Error[]    PROGMEM;

	extern uint8_t ToneVol;
	
	// PROTOTYPES:
	void TG_PlayToneSeq(const uint8_t* Sequence) ATTR_NON_NULL_PTR_ARGS(1);

#endif
