/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef TAGMGR_H
#define TAGMGR_H

	// INCLUDES:
	#include <avr/io.h>
	
	#include "ButtLoadTag.h"
	#include "Dataflash.h"
	#include "GlobalMacros.h"
	#include "Main.h"
	#include "SPI.h"
	#include "StorageManager.h"
	
	// PROTOTYPES:
	void   TM_ShowTags(void);
	
	#if defined(INC_FROM_TM)
	  static void    TM_FindNextTag(void);
	  static uint8_t TM_GetNextByte(void) ATTR_WARN_UNUSED_RESULT;
	#endif
	
#endif
