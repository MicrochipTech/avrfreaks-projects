/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef OSCCAL_H
#define OSCCAL_H

	// INCLUDES:
	#include <avr/io.h>
	#include <util/atomic.h>
	
	#include "GlobalMacros.h"

	// DEFINES:
	#define OSCCAL_TARGETCOUNT         (uint16_t)(F_CPU / (32768 / 256)) // (Target Freq / Reference Freq)	
	#define OSCCAL_TOLLERANCE          2
	
	// PROTOTYPES:
	void OSCCAL_Calibrate(void);
	
#endif
